#include "base.h"

bool Base::init()
{
	srand (time(NULL));
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

	isFullscreen = false;

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
	//files.push_back( "./models/rungholt/rungholt.obj");
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
		tempObj.position = glm::vec3(0,0,0);
		tempObj.velocity = glm::vec3(0,0,0);
		tempObj.orientation = glm::vec3(0,0,0);
		tempObj.rotVelocity = glm::vec3(0,0,0);
		tempObj.scale = glm::vec3(1,1,1);

		objs.push_back( tempObj );
		

		printf("Object %i (%s) has %lu shapes:\n", i, files[i], objs[i].shapes.size() );
		for(int j = 0; j < objs[i].shapes.size(); j++ )
			printf("Shape %i has %lu vertices, %lu indices, %lu texture coordinates, and %lu normals.\n", j, objs[i].shapes[j].mesh.positions.size(), objs[i].shapes[j].mesh.indices.size(), objs[i].shapes[j].mesh.texcoords.size(), objs[i].shapes[j].mesh.normals.size() );


	}

	objs[0].mass = MASS_1;
	objs[1].mass = MASS_2;


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

        //objs[0].transform = glm::scale( objs[0].transform, glm::vec3(0.5, 0.5, 0.5) );
        //objs[1].transform = glm::scale( objs[1].transform, glm::vec3(0.5, 0.5, 0.5) );
	
	//objs[0].scale *= 0.5f;
	//objs[1].scale *= 0.5f;

	objs[0].velocity = {0.01,0,0};
	objs[0].velocity = glm::vec3(getRandom(-2,2), getRandom(-2,2), getRandom(-2,2) );
	printf("obj[0]'s velocity is %f, %f, %f\n", objs[0].velocity.x, objs[0].velocity.y, objs[0].velocity.z );
	objs[1].velocity = glm::vec3(getRandom(-2,2), getRandom(-2,2), getRandom(-2,2) );
	printf("obj[1]'s velocity is %f, %f, %f\n", objs[1].velocity.x, objs[1].velocity.y, objs[1].velocity.z );

	objs[0].rotVelocity = glm::vec3(0.1f,0.0f,0.0f);
	objs[1].rotVelocity = glm::vec3(0.0f, 0.1f, 0.0f);
	
	//objs[0].translate( glm::vec3(2,0,0) );
	//objs[1].translate( glm::vec3(-2,0,0) );
	objs[0].position = glm::vec3(2,0,0);
	objs[1].position = glm::vec3(-2,0,0);

	//objs[0].velocity = glm::vec3(0.1,0,0);
	//objs[1].velocity = glm::vec3(-0.1,0,0);;

	if (GLEW_OK != err)
	{
		  /* Problem: glewInit failed, something is seriously wrong. */
		  printf("\nError: %s\n", glewGetErrorString(err));
	}

	SDL_GL_SetSwapInterval(1);

	glm::vec3 vertical = glm::vec3(0.0f, 4.0f, 0.0f);
	glm::vec3 horizontal = glm::vec3(4.0f, 0.0f, 0.0f);
	glm::vec3 theotherthing = glm::vec3(0.0f,0.0f,4.0f);

	cubePlanes[0] = { vertical, glm::normalize(-vertical)  };//top
	cubePlanes[1] = { -vertical, glm::normalize(vertical) };//bottom
	cubePlanes[2] = { horizontal, glm::normalize(-horizontal) };//right
	cubePlanes[3] = { -horizontal, glm::normalize(horizontal) };//left
	cubePlanes[4] = { theotherthing, glm::normalize(-theotherthing) };//top
	cubePlanes[5] = { -theotherthing, glm::normalize(theotherthing) };//bottom

	initGL();

	SDL_SetRelativeMouseMode( SDL_TRUE );


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
	

	cameraPos = glm::vec3(0.0f, 0.0f, -6.0f);

	if(xRot < -M_PI)
		xRot += M_PI * 2;

	else if(xRot > M_PI)
		xRot -= M_PI * 2;

	if(yRot < -M_PI / 2)
		yRot = -M_PI / 2;
	if(yRot > M_PI / 2)
		yRot = M_PI / 2;

	glm::vec3 lookat;
	lookat.x = sinf(xRot) * cosf(yRot);
	lookat.y = sinf(yRot);
	lookat.z = cosf(xRot) * cosf(yRot);

	camera = glm::lookAt(cameraPos, cameraPos + lookat, glm::vec3(0, 1, 0));
	cubeMatrix = glm::mat4();

	gpuLocations.insert( std::pair<const char*, GLuint>("mvp", glGetUniformLocation(program, "mvp")) );
	gpuLocations.insert( std::pair<const char*, GLuint>("mv", glGetUniformLocation(program, "mv")) );
	gpuLocations.insert( std::pair<const char*, GLuint>("normal_matrix", glGetUniformLocation(program, "normal_matrix")) );
	gpuLocations.insert( std::pair<const char*, GLuint>("LightPosition", glGetUniformLocation(program, "LightPosition")) );
	gpuLocations.insert( std::pair<const char*, GLuint>("Kd", glGetUniformLocation(program, "Kd")) );
	gpuLocations.insert( std::pair<const char*, GLuint>("Ld", glGetUniformLocation(program, "Ld")) );
	
	//Position of light
	//glUniform4fv(gpuLocations.at("LightPosition"), 1, glm::value_ptr( glm::column(-camera, 3) ) ); 
	glUniform4fv(gpuLocations.at("LightPosition"), 1, glm::value_ptr( glm::vec3(0,0,0) ) ); 
	checkGLErrors("Init LP uniform");
	//Light constant
	glUniform3fv(gpuLocations.at("Kd"), 1, glm::value_ptr( glm::vec3(0.8) ) ); 
	checkGLErrors("Init Kd uniform");
	//Light intensity
	glUniform3fv(gpuLocations.at("Ld"), 1, glm::value_ptr( glm::vec3(0.9) ) ); 
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

void Base::render()
{
	glClearColor(1.0f,0.8f,0.8f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glm::mat4 mv, mvp;
	glm::mat3 rot, normal;

	for(int i = 0; i < objs.size(); i++)
	{
		//Uniforms
		mv = camera * objs[i].transform();
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
	glm::vec3 lookat;

	while (SDL_PollEvent(&event)) 
	{
		switch(event.type)
		{
			case SDL_KEYDOWN:
				key = event.key.keysym.sym;
				if(key == SDLK_SPACE
						||
					key == SDLK_RETURN
						||
					key == SDLK_ESCAPE)

					active = false;

				if(key == SDLK_w)
					cameraPos -= glm::vec3(0.0f, 0.0f, 3.0f) *  glm::mat3(camera) * physT;

				if(key == SDLK_s)
					cameraPos += glm::vec3(0.0f, 0.0f, 3.0f) *  glm::mat3(camera) * physT;

				if(key == SDLK_a)
					cameraPos -= glm::vec3(3.0f, 0.0f, 0.0f) *  glm::mat3(camera) * physT;

				if(key == SDLK_d)
					cameraPos += glm::vec3(3.0f, 0.0f, 0.0f) *  glm::mat3(camera) * physT;
				
				if(key == SDLK_z)
					objs[0].velocity = {-0.1,0,0};
				if(key == SDLK_x)
					objs[0].velocity = {0.1,0,0};

				if(key == SDLK_f)
					toggleFullScreen();

				break;

			case SDL_MOUSEMOTION:

				xRot -= event.motion.xrel * 0.001;
				yRot -= event.motion.yrel * 0.001;
				break;
		}

	}



	if(xRot < -M_PI)
		xRot += M_PI * 2;

	else if(xRot > M_PI)
		xRot -= M_PI * 2;

	if(yRot < -M_PI / 2)
		yRot = -M_PI / 2;
	if(yRot > M_PI / 2)
		yRot = M_PI / 2;

	lookat.x = sinf(xRot) * cosf(yRot);
	lookat.y = sinf(yRot);
	lookat.z = cosf(xRot) * cosf(yRot);

	camera = glm::lookAt(cameraPos, cameraPos + lookat, glm::vec3(0, 1, 0));
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

void Base::physics()
{
	int i, j;
	int numObjects = objs.size();
	float d;
	
	for(i = 0; i < numObjects; i++)
	{
		//objs[i].rotate( objs[i].rotVelocity * physT );
		//objs[i].translate( objs[i].velocity * physT );

		objs[i].position += objs[i].velocity * physT;
		objs[i].orientation += objs[i].rotVelocity * physT;

		glm::vec3 planetPos = objs[i].position;
		//Iterate over planes
		for(j = 0; j < 6; j++)
		{
			//We multiply our distance with 2 since it needs to be scaled
			//up for the sphere
			d = cubePlanes[j].distanceFromPlane( planetPos * objs[i].scale);

			if(abs(d) < sphereRadius * objs[i].scale.x)
			{
				//printf("[d=%f] Planet %i is colliding with plane %i!\n", d, i, j);
				handlePlaneCollision(&objs[i], cubePlanes[j]);
			}
		}
	}

	if( checkForSphereCollision(objs[0], objs[1] ) )
	{
		handleSphereCollision(&objs[0], &objs[1], E_CONSTANT);
		//printf("BOOM!\n");
	}
	

}

void Base::handlePlaneCollision(object* s, plane p)
{
	float dot = glm::dot(p.n, s->velocity);
	//If our dot product is > 0, then we're
	//moving away from the plane
	if(dot > 0)
		return;
	
	s-> velocity = -2.0f * glm::dot(s->velocity, p.n) * p.n + s->velocity;


}


float Base::checkForSphereCollision(object s1, object s2)
{
	glm::vec3 d = s1.position - s2.position;
	float distanceDot = glm::dot(d, d);

	float radiusSum = s1.radius * s1.scale.x + s2.radius * s2.scale.x;

	return distanceDot <= pow(radiusSum, 2);
}

void Base::toggleFullScreen()
{
	int w, h;
	if(!isFullscreen)
	{
		SDL_SetWindowFullscreen(mainWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
		projection = glm::perspective(45.0f, 16.0f / 9.0f, 0.1f, 50.f);
		SDL_GetWindowSize(mainWindow, &w, &h);
		glViewport(0,0,w,h);
		isFullscreen = true;
	}
	else
	{
		SDL_SetWindowFullscreen(mainWindow, 0);
		SDL_SetWindowSize( mainWindow, 800, 600);
		projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 50.f);
		glViewport(0,0,800,600);
		isFullscreen = false;
	}
}


void Base::handleSphereCollision(object* s1, object* s2, float e)
{
	glm::vec3 vAP = s1->velocity;
	glm::vec3 vBP = s2->velocity;
	glm::vec3 vAB = vAP - vBP;

	glm::vec3 N = glm::normalize( s1->position - s2->position );

	//Our balls both have mass=1, okay? 
	float j = - (1.0f + e) * glm::dot(vAB, N)
		/ 
		  (glm::dot(N,N) * (1/s1->mass + 1/s2->mass));

	s1->velocity = s1->velocity + j * N;
	s2->velocity = s2->velocity - j * N;

}

float Base::getRandom(float low, float high)
{
	int intComponent = (rand() % (int)(high + abs(low)) - abs(low));
	float floatComponent = 1.0f / (rand() % 100);

	return (float)(intComponent + floatComponent);
}
