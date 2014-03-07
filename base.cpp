#include "base.h"

bool Base::init()
{
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		quit();
	}
	printf("SDL Initialized!\n");

	//Mesa currently only supports a 3.3 core profile, AFAIK
	//So, being a Linux user with an Intel CPU (and mesa)
	//I'm going to have to stick with OpenGL 3.3
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	printf("Creating main window... ");
	mainWindow = SDL_CreateWindow("SDL is fun", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	//mainWindow = SDL_CreateWindow("SDL is fun", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP);

	if(!mainWindow)
	{
		printf("Woops, couldn't create the main window.");
		quit();
	}	
	printf("Done.\n");

	printf("Creating GL context... ");
	glContext = SDL_GL_CreateContext(mainWindow);
	printf("Done.\n");

	glewExperimental = GL_TRUE; 
	printf("Initializing GLEW... ");
	glewInit();
	printf("Done.\n");
	printf("GLSL version is %s\n",  glGetString(GL_SHADING_LANGUAGE_VERSION));

	GLenum err = glewInit();

	printf("Loading main object... ");

	//Filenames for the shapes to load
	std::vector<const char*> files;
	//files.push_back( "./rungholt/rungholt.obj");
	//files.push_back( "./models/suzanne.obj");
	files.push_back("./models/sphere/sphere.obj");
	files.push_back("./models/sphere/sphere.obj");

	object tempObj;
	std::vector<tinyobj::shape_t> tempMesh;

	for(int i = 0; i < files.size(); i++)
	{
		std::string error = tinyobj::LoadObj(tempMesh, files[i]);
		printf("Loaded file %s.\n", files[i]);
		if(error.size() > 0)
			printf("Error: %s", error.c_str() );

		for(int j = 0; j < tempMesh.size(); j++)
			if(tempMesh[j].mesh.normals.size() <= 0 )
			{
				printf("No normals found for shape %i. Generating our own...\n", j);
				generateNormals(&tempMesh[j].mesh);
			}

		tempObj.shapes = tempMesh;
		tempObj.transform = glm::mat4();

		objs.push_back( tempObj );
		

		printf("Object %i (%s) has %lu shapes:\n", i, files[i], objs[i].shapes.size() );
		for(int j = 0; j < objs[i].shapes.size(); j++ )
			printf("Shape %i has %lu vertices, %lu indices, %lu texture coordinates, and %lu normals.\n", j, objs[i].shapes[j].mesh.positions.size(), objs[i].shapes[j].mesh.indices.size(), objs[i].shapes[j].mesh.texcoords.size(), objs[i].shapes[j].mesh.normals.size() );


	}


	//Does our initial model have texturecoords?
	//I mean, if you're loading a model in without 
	//texturecoords, things are going to break
	//so we might as well assume that either all 
	//your models have textures, or none do
	if(objs[0].shapes[0].mesh.texcoords.size() <= 0)
	{
		hasTexture = false;
		printf("[NOTICE]: No texcoords found in object 0. Textures will not be loaded.\n");
	}
	else
	{
		hasTexture = true;
		printf("[NOTICE]: Found texturecoords; textures are enabled.\n");
	}

        objs[0].transform = glm::scale( objs[0].transform, glm::vec3(0.5, 0.5, 0.5) );
	objs[0].translate( glm::vec3(4.0f,0.0f,0.0f));
        objs[1].transform = glm::scale( objs[1].transform, glm::vec3(0.5, 0.5, 0.5) );
	objs[1].translate( glm::vec3(-3.0f,0.0f,0.0f));
	//objs[0].transform = glm::rotate( objs[0].transform, 0.8f, glm::vec3(0,1,0) ); 
	//objs[0].transform = glm::rotate( objs[0].transform, 0.5f, glm::vec3(0,1,0) ); 
	//objs[0].transform = glm::rotate( objs[0].transform, 0.8f, glm::vec3(1,0,0) ); 
	//objs[0].transform = glm::rotate( objs[0].transform, 0.8f, glm::vec3(0,0,1) ); 
	//objs[0].transform = glm::translate( objs[0].transform, glm::vec3(0, -20, 0) );

	if (GLEW_OK != err)
	{
		  /* Problem: glewInit failed, something is seriously wrong. */
		  printf("\nError: %s\n", glewGetErrorString(err));
	}

	SDL_GL_SetSwapInterval(1);

	glm::vec3 vertical = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 horizontal = glm::vec3(1.0f, 0.0f, 0.0f);

	cubePlanes[0] = { vertical * 2.0f, -vertical  };//top
	cubePlanes[1] = { -vertical * 2.0f, vertical };//bottom
	cubePlanes[2] = { horizontal * 2.0f, -horizontal };//right
	cubePlanes[3] = { -horizontal * 2.0f, horizontal };//left

	initGL();

	printf("Completed initialization!\n");

	loop( SDL_GetTicks() );

	return 1;
}

void Base::loop(int lastFrame)
{
	//Lets process our events, first
	processEvents();

	//Because we're using a constant 
	//deltaT with a variable fps
	//we have no choice but to
	//do multiple physics steps if
	//our dF > dT (or else we could
	//potentially pass in really big
	//or really small values to the physics
	//simulation)
	int currentTime = SDL_GetTicks();
	//The change between frames
	int deltaF = currentTime - lastFrame;
	timeStepsToProcess += deltaF;


	while(timeStepsToProcess >= deltaT)
	{
		//Note: Since we're using 
		//a constant deltaT declared
		//in the header file, we're
		//not passing dT as an argument
		physics();
		timeStepsToProcess -= deltaT;

	}

	render();


	if(active)
		loop(currentTime);
	else
		quit();

}

bool Base::initGL()
{
	printf("Initializing openGL...\n");
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_FLAT);

	program = glCreateProgram();

	initShader( GL_VERTEX_SHADER, "./shaders/vertex.vs" );
	checkGLErrors("End of Vertex shader init");
	printf("Vertex shader, done...\n");

	if(hasTexture)
		initShader( GL_FRAGMENT_SHADER, "./shaders/fragment.fs" );
	else
		initShader( GL_FRAGMENT_SHADER, "./shaders/fragment.notex.fs" );
	checkGLErrors("End of fragment shader init");
	printf("Fragment shader, done...\n");
	printf("Loaded and initialized shaders...\n");

	

	glLinkProgram( program );

	int maxAt;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxAt);

	gpuLocations.insert( std::pair<const char*, GLuint>("vertex_attrib", glGetAttribLocation(program, "theV") ) );
	gpuLocations.insert( std::pair<const char*, GLuint>("normals_attrib", glGetAttribLocation(program, "theN") ) );
	gpuLocations.insert( std::pair<const char*, GLuint>("textures_attrib", glGetAttribLocation(program, "tex_in") ) );


	printf("Setting up initial buffers... \n");
	initBuffers();
	printf("Done.\n");

	glUseProgram( program );
	checkGLErrors("Use program");

	//Initialize the actual matrices
	model = glm::mat4();
	model = glm::rotate(model, 1.570f, glm::vec3(0.0f, 1.0f, 0.0f) );
	projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 50.f);
	camera = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -5.0f));

	cubeMatrix = glm::mat4();

	gpuLocations.insert( std::pair<const char*, GLuint>("mvp", glGetUniformLocation(program, "mvp")) );
	gpuLocations.insert( std::pair<const char*, GLuint>("mv", glGetUniformLocation(program, "mv")) );
	gpuLocations.insert( std::pair<const char*, GLuint>("normal_matrix", glGetUniformLocation(program, "normal_matrix")) );
	gpuLocations.insert( std::pair<const char*, GLuint>("LightPosition", glGetUniformLocation(program, "LightPosition")) );
	gpuLocations.insert( std::pair<const char*, GLuint>("Kd", glGetUniformLocation(program, "Kd")) );
	gpuLocations.insert( std::pair<const char*, GLuint>("Ld", glGetUniformLocation(program, "Ld")) );
	
	//Position of light
	glUniform4fv(gpuLocations.at("LightPosition"), 1, glm::value_ptr( glm::column(-camera, 3) ) ); 
	checkGLErrors("Init LP uniform");
	//Light constant
	glUniform3fv(gpuLocations.at("Kd"), 1, glm::value_ptr( glm::vec3(0.9, 0.9, 0.7) ) ); 
	checkGLErrors("Init Kd uniform");
	//Light intensity
	glUniform3fv(gpuLocations.at("Ld"), 1, glm::value_ptr( glm::vec3(0.9, 0.9, 0.9) ) ); 
	checkGLErrors("Init Ld uniform");

	glUniform1i(glGetUniformLocation(program, "texSampler"), 0);

	
	printf("OpenGL initialized!\n");

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//glEnable(GL_CULL_FACE);

	return checkGLErrors("End of initGL");;
}

bool Base::checkGLErrors(const char* description)
{
	GLenum error = glGetError();
	bool hadError = false;

	while(error != GL_NO_ERROR)
	{
		printf("[ERROR]@[%s]: %s\n", description, gluErrorString(error));
		error = glGetError();
		hadError = true;
	}

	return hadError;
}

void Base::initBuffers()
{

	GLuint vbo;
        GLuint ibo;
	GLuint vao;
	GLuint nbo;
	GLuint tbo;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	checkGLErrors("VAO creation");

	glGenBuffers( 1, &nbo );
	glBindBuffer( GL_ARRAY_BUFFER, nbo );
	checkGLErrors("NBO binding");
	glBufferData( GL_ARRAY_BUFFER, objs[0].shapes[0].mesh.normals.size() * sizeof(float), &objs[0].shapes[0].mesh.normals[0], GL_STATIC_DRAW);
	checkGLErrors("NBO buffer data");
	glVertexAttribPointer( gpuLocations.at("normals_attrib"), 3, GL_FLOAT, 0, 0, 0 );
	checkGLErrors("NBO creation");
	gpuLocations.insert( std::pair<const char*, GLuint>("nbo", nbo) );


	if(hasTexture)
	{
		glGenBuffers( 1, &tbo );
		glBindBuffer( GL_ARRAY_BUFFER, tbo );
		checkGLErrors("TBO binding");
		glBufferData( GL_ARRAY_BUFFER, objs[0].shapes[0].mesh.texcoords.size() * sizeof(float), &objs[0].shapes[0].mesh.texcoords[0], GL_STATIC_DRAW);
		checkGLErrors("TBO buffer data");
		gpuLocations.insert( std::pair<const char*, GLuint>("tbo", tbo) );
		glVertexAttribPointer( gpuLocations.at("textures_attrib"), 2, GL_FLOAT, 0, 0, 0);
		checkGLErrors("Describe texture vertex attrib");
		glEnableVertexAttribArray( gpuLocations.at("textures_attrib") );
		checkGLErrors("Enable texture vertex attrib array");
	}

	glGenBuffers( 1, &vbo );
	glBindBuffer( GL_ARRAY_BUFFER, vbo );
	glBufferData( GL_ARRAY_BUFFER, objs[0].shapes[0].mesh.positions.size() * sizeof(float), &objs[0].shapes[0].mesh.positions[0], GL_STATIC_DRAW );
	checkGLErrors("VBO Creation");
	gpuLocations.insert( std::pair<const char*, GLuint>("vbo", vbo) );

	glVertexAttribPointer( gpuLocations.at("vertex_attrib"), 3, GL_FLOAT, 0, 0, 0 );
	checkGLErrors("Configured vbo attribpointer");

	glGenBuffers( 1, &ibo );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, objs[0].shapes[0].mesh.indices.size() * sizeof(unsigned int), &objs[0].shapes[0].mesh.indices[0], GL_STATIC_DRAW );
	checkGLErrors("IBO creation");

	glEnableVertexAttribArray(gpuLocations.at("vertex_attrib"));
	glEnableVertexAttribArray(gpuLocations.at("normals_attrib"));
	checkGLErrors("Enabled ibo attrib array");

	//GLuint tex;
	//glGenTextures(1, &tex);

	//Normally, here, we'd want to read the diffuse_texname
	//of our loaded object
	//But SOIL, for some odd reason, won't accept char* variables
	if(hasTexture)
	{
		GLuint tex = SOIL_load_OGL_texture
		(
			"earthmap1k.jpg",
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
		);


		checkGLErrors("Generating texture");
		glBindTexture(GL_TEXTURE_2D, tex);
		checkGLErrors("Binding texture");
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		checkGLErrors("Setting texture wrap mode");

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		checkGLErrors("Setting texture filtering mode");
	}

	glBindVertexArray(vao);
	checkGLErrors("Binding vertex array");
}

bool Base::initShader(GLenum type, std::string file)
{
	GLuint shader = glCreateShader( type );
	checkGLErrors("initShader: created shader");

	std::string source;

	if(!readFile(file, &source))
	{
		printf("ERROR: Could not read file %s", file.c_str());
		quit();
	}

	glShaderSource( shader, 1, (const GLchar**)&source, NULL );
	checkGLErrors("initShader: set shader source");
	

	glCompileShader( shader );
	checkGLErrors("initShader: compiled shader");

	GLint shaderCompiled = GL_FALSE;
	glGetShaderiv( shader, GL_COMPILE_STATUS, &shaderCompiled ); 
	if( shaderCompiled != GL_TRUE ) 
	{ 
		printf( "ERROR: Unable to compile shader %d!\n\nSource:\n%s\n", shader, source.c_str() );
		int infoLength = 0;
		int maxL = infoLength;

		glGetShaderiv(program, GL_INFO_LOG_LENGTH, &maxL);
		char* infoLog = new char[maxL];

		glGetShaderInfoLog(program, maxL, &infoLength, infoLog);
		//if (infoLength > 0)
			printf("GLSHADERIV:%s\n", infoLog);

		glDeleteShader( shader );
		shader = 0;

		quit();
	}

	glAttachShader( program, shader );
	checkGLErrors("initShader: attached shader");

	return true;
}

void Base::physics()
{


}

void Base::render()
{
	glClearColor(1.0f,0.8f,0.8f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glm::mat4 mv, mvp;
	glm::mat3 rot, normal;

	for(int i = 0; i < objs.size(); i++)
	{
		//Uniforms
		mv = camera * objs[i].transform;
		mvp = projection * mv;
		rot = (glm::mat3)mv;
		normal = glm::inverseTranspose(rot);

		glUniformMatrix4fv(gpuLocations.at("mvp"), 1, GL_FALSE, glm::value_ptr(mvp) );
		glUniformMatrix4fv(gpuLocations.at("mv"), 1, GL_FALSE, glm::value_ptr(mv) );
		glUniformMatrix3fv(gpuLocations.at("normal_matrix"), 1, GL_FALSE, glm::value_ptr(normal) );

		for(int j = 0; j < objs[i].shapes.size(); j++)
		{
			//Normals
			glBindBuffer( GL_ARRAY_BUFFER, gpuLocations.at("nbo") );
			glBufferData( GL_ARRAY_BUFFER, objs[i].shapes[j].mesh.normals.size() * sizeof(float), &objs[i].shapes[j].mesh.normals[0], GL_STATIC_DRAW);
			glVertexAttribPointer( gpuLocations.at("normals_attrib"), 3, GL_FLOAT, 0, 0, 0 );

			if(hasTexture)
			{
				//Texture Coords
				glBindBuffer( GL_ARRAY_BUFFER, gpuLocations.at("tbo") );
				glBufferData( GL_ARRAY_BUFFER, objs[i].shapes[j].mesh.texcoords.size() * sizeof(float), &objs[i].shapes[j].mesh.texcoords[0], GL_STATIC_DRAW);
				glVertexAttribPointer( gpuLocations.at("textures_attrib"), 2, GL_FLOAT, 0, 0, 0);
			}

			//Vertices
			glBindBuffer( GL_ARRAY_BUFFER, gpuLocations.at("vbo") );
			glBufferData( GL_ARRAY_BUFFER, objs[i].shapes[j].mesh.positions.size() * sizeof(float), &objs[i].shapes[j].mesh.positions[0], GL_STATIC_DRAW );
			glVertexAttribPointer( gpuLocations.at("vertex_attrib"), 3, GL_FLOAT, 0, 0, 0 );

			//Indices
			glBufferData( GL_ELEMENT_ARRAY_BUFFER, objs[i].shapes[j].mesh.indices.size() * sizeof(unsigned int), &objs[i].shapes[j].mesh.indices[0], GL_STATIC_DRAW );

			glDrawElements(GL_TRIANGLES, objs[i].shapes[j].mesh.indices.size(), GL_UNSIGNED_INT, NULL); 
		}
	}

	mv = camera * cubeMatrix;
	mvp = projection * mv;
	rot = (glm::mat3)mv;
	normal = glm::inverseTranspose(rot);

	glUniformMatrix4fv(gpuLocations.at("mvp"), 1, GL_FALSE, glm::value_ptr(mvp) );
	glUniformMatrix4fv(gpuLocations.at("mv"), 1, GL_FALSE, glm::value_ptr(mv) );
	glUniformMatrix3fv(gpuLocations.at("normal_matrix"), 1, GL_FALSE, glm::value_ptr(normal) );
	
	//Render cube
	glBufferData( GL_ARRAY_BUFFER, 16 * sizeof(glm::vec3), &cube[0], GL_STATIC_DRAW );
	glVertexAttribPointer( gpuLocations.at("vertex_attrib"), 3, GL_FLOAT, 0, 0, 0 );
	glDrawArrays(GL_LINE_STRIP, 0, 16);

	checkGLErrors("Post render");

	SDL_GL_SwapWindow(mainWindow);

}

void Base::processEvents()
{
	SDL_Event event;
	SDL_Keycode key;

	while (SDL_PollEvent(&event)) 
	{
		SDL_Keycode key = event.key.keysym.sym;
		switch( event.type )
		{
			case SDL_KEYDOWN:
				if(key == SDLK_SPACE
						||
					key == SDLK_RETURN
						||
					key == SDLK_ESCAPE)

					active = false;

				if(key == SDLK_w)
					camera = glm::translate(camera, glm::vec3(0.0f, 0.0f, 0.1f) * glm::mat3(camera));

				if(key == SDLK_s)
					camera = glm::translate(camera, glm::vec3(0.0f, 0.0f, -0.1f)* glm::mat3(camera));

				if(key == SDLK_q)
					camera = glm::translate(camera, glm::vec3(0.1f, 0.0f, 0.0f)* glm::mat3(camera));

				if(key == SDLK_e)
					camera = glm::translate(camera, glm::vec3(-0.1f, 0.0f, 0.0f)* glm::mat3(camera));

				if(key == SDLK_d)
					camera = glm::rotate(camera, 0.01f, glm::vec3(0.0f,1.0f,0.0f) );

				if(key == SDLK_a)
					camera = glm::rotate(camera, -0.01f, glm::vec3(0.0f,1.0f,0.0f) );

				float mod = 0.1f;

				if(event.key.keysym.mod & KMOD_SHIFT)
					mod = -0.1f;

				if(key == SDLK_x)
					objs[0].transform = glm::translate( objs[0].transform, glm::vec3(0.10 * mod, 0, 0) );
				if(key == SDLK_y)
					objs[0].transform = glm::translate( objs[0].transform, glm::vec3(0, 0.10 * mod, 0) );
				if(key == SDLK_z)
					objs[0].transform = glm::translate( objs[0].transform, glm::vec3(0, 0, 0.10 * mod) );

				if(key == SDLK_UP)
					objs[0].transform = glm::scale( objs[0].transform, glm::vec3(1.f, 1.1f, 1.f) );
				if(key == SDLK_DOWN)
					objs[0].transform = glm::scale( objs[0].transform, glm::vec3(1.f, .9f, 1.f) );

				glm::vec3 planetPos = objs[0].position();
				float d = cubePlanes[2].distanceFromPlane( planetPos );
				printf("Planet at %f, %f, %f. Distance of %f away from right plane.\n", planetPos[0], planetPos[1], planetPos[2], d); 

				break;
		}
	}
}

void Base::quit()
{
	printf("\nQuitting!\n");
	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(mainWindow);
	SDL_Quit();

}

Base::Base()
{
}


bool Base::readFile(std::string filename, std::string* target)
{

	std::ifstream shaderSource(filename.c_str());

	target->assign( ( std::istreambuf_iterator< char >( shaderSource ) ), std::istreambuf_iterator< char >() );
	if(target->length() > 0)
		return true;

	return false;

}

void Base::generateNormals(tinyobj::mesh_t *mesh)
{

	for(int i = 0; i < mesh->indices.size(); i += 3)
	{
			glm::vec3 p1, p2, p3, u, v, n;
			int iP1, iP2, iP3;

			iP1 = mesh->indices[i];
			iP2 = mesh->indices[i+1];
			iP3 = mesh->indices[i+2];


			p1.x = mesh->positions[iP1];
			p1.y = mesh->positions[iP1+1];
			p1.z = mesh->positions[iP1+2];

			p2.x = mesh->positions[iP2];
			p2.y = mesh->positions[iP2+1];
			p2.z = mesh->positions[iP2+2];

			p3.x = mesh->positions[iP3];
			p3.y = mesh->positions[iP3+1];
			p3.z = mesh->positions[iP3+2];

			n = glm::cross( (p2-p1), (p3-p1) );
			/*u = p2 - p1;
			v = p3 - p1;	

			n.x = u.y * v.z - u.z * v.y;
			n.y = u.z * v.x - u.x * v.z;
			n.z = u.x * v.y - u.y * v.x;*/

			n = glm::normalize(n);

			for(int m = 0; m < 3; m++)
			{
				mesh->normals.push_back(n.x);
				mesh->normals.push_back(n.y);
				mesh->normals.push_back(n.z);
			}
			
	}

}

