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
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	mainWindow = SDL_CreateWindow("SDL is fun", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

	SDL_SetRelativeMouseMode( SDL_TRUE );

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

	//glewExperimental = gl::TRUE; 
	//printf("Initializing GLEW... ");
	//glewInit();
	printf("Done.\n");
	//printf("OpenGL version is %s\n", gl::GetString(gl::VERSION) );
	//printf("GLSL version is %s\n",  gl::GetString(gl::SHADING_LANGUAGE_VERSION));

	//GLenum err = glewInit();

	printf("Loading functions!\n");
	gl::exts::LoadTest didLoad = gl::sys::LoadFunctions();
	if(!didLoad)
	{
		printf("Could not load!\n");
		exit(1);
	}
	printf("Functions loaded!\n");

	printf("Number of functions that failed to load: %i.\n", didLoad.GetNumMissing());

	printf("Loading main object... ");

	//Filenames for the shapes to load
	std::vector<const char*> files;
	//files.push_back( "./models/rungholt/rungholt.obj");
	//files.push_back( "./models/sibenik.obj" );
	files.push_back( "./models/suzanne.obj");
	//files.push_back("./models/sphere/sphere.obj");

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
		//hasTexture = false; //REMOVE THIS
		printf("[NOTICE]: Found texturecoords; textures are enabled.\n");
	}

        objs[0].transform = glm::scale( objs[0].transform, glm::vec3(0.05, 0.05, 0.05) );
	//objs[0].transform = glm::rotate( objs[0].transform, 0.8f, glm::vec3(0,1,0) ); 
	//objs[0].transform = glm::rotate( objs[0].transform, 0.5f, glm::vec3(0,1,0) ); 
	//objs[0].transform = glm::rotate( objs[0].transform, 0.8f, glm::vec3(1,0,0) ); 
	//objs[0].transform = glm::rotate( objs[0].transform, 0.8f, glm::vec3(0,0,1) ); 
	//objs[0].transform = glm::translate( objs[0].transform, glm::vec3(0, -20, 0) );

	/*if (GLEW_OK != err)
	{
		  Problem: glewInit failed, something is seriously wrong.
		  printf("\nError: %s\n", glewGetErrorString(err));
	}*/

	SDL_GL_SetSwapInterval(1);

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
	gl::Enable(gl::DEPTH_TEST);
	//glShadeModel(gl::FLAT);

	program = gl::CreateProgram();

	initShader( gl::VERTEX_SHADER, "./shaders/vertex.vs" );
	checkGLErrors("End of Vertex shader init");
	printf("Vertex shader, done...\n");

	if(hasTexture)
		initShader( gl::FRAGMENT_SHADER, "./shaders/fragment.fs" );
	else
		initShader( gl::FRAGMENT_SHADER, "./shaders/fragment.notex.fs" );
	checkGLErrors("End of fragment shader init");
	printf("Fragment shader, done...\n");
	printf("Loaded and initialized shaders...\n");

	

	gl::LinkProgram( program );

	int maxAt;
	gl::GetIntegerv(gl::MAX_VERTEX_ATTRIBS, &maxAt);

	gpuLocations.insert( std::pair<const char*, GLuint>("vertex_attrib", gl::GetAttribLocation(program, "theV") ) );
	gpuLocations.insert( std::pair<const char*, GLuint>("normals_attrib", gl::GetAttribLocation(program, "theN") ) );
	gpuLocations.insert( std::pair<const char*, GLuint>("textures_attrib", gl::GetAttribLocation(program, "tex_in") ) );


	printf("Setting up initial buffers... \n");
	initBuffers();
	printf("Done.\n");

	gl::UseProgram( program );
	checkGLErrors("Use program");

	//Initialize the actual matrices
	model = glm::mat4();
	//model = glm::scale(model, glm::vec3(0.05,0.05,0.05));
	model = glm::rotate(model, 1.570f, glm::vec3(0.0f, 1.0f, 0.0f) );
	projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 50.f);
	//camera = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -1.0f));
	cameraPos = glm::vec3(0.0f, 1.0f, -1.0f);

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

	gpuLocations.insert( std::pair<const char*, GLuint>("mvp", gl::GetUniformLocation(program, "mvp")) );
	gpuLocations.insert( std::pair<const char*, GLuint>("mv", gl::GetUniformLocation(program, "mv")) );
	gpuLocations.insert( std::pair<const char*, GLuint>("normal_matrix", gl::GetUniformLocation(program, "normal_matrix")) );
	gpuLocations.insert( std::pair<const char*, GLuint>("LightPosition", gl::GetUniformLocation(program, "LightPosition")) );
	gpuLocations.insert( std::pair<const char*, GLuint>("Kd", gl::GetUniformLocation(program, "Kd")) );
	gpuLocations.insert( std::pair<const char*, GLuint>("Ld", gl::GetUniformLocation(program, "Ld")) );
	
	//Position of light
	gl::Uniform4fv(gpuLocations.at("LightPosition"), 1, glm::value_ptr( glm::column(-camera, 3) ) ); 
	checkGLErrors("Init LP uniform");
	//Light constant
	gl::Uniform3fv(gpuLocations.at("Kd"), 1, glm::value_ptr( glm::vec3(0.9, 0.9, 0.7) ) ); 
	checkGLErrors("Init Kd uniform");
	//Light intensity
	gl::Uniform3fv(gpuLocations.at("Ld"), 1, glm::value_ptr( glm::vec3(0.9, 0.9, 0.9) ) ); 
	checkGLErrors("Init Ld uniform");

	gl::Uniform1i(gl::GetUniformLocation(program, "texSampler"), 0);

	
	printf("OpenGL initialized!\n");

	gl::PolygonMode(gl::FRONT_AND_BACK, gl::FILL);
	//gl::Enable(gl::CULL_FACE);

	return checkGLErrors("End of initGL");;
}

bool Base::checkGLErrors(const char* description)
{
	GLenum error = gl::GetError();
	bool hadError = false;

	while(error != gl::NO_ERROR_)
	{
		printf("[ERROR]@[%s]: %s\n", description, gluErrorString(error));
		error = gl::GetError();
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

	gl::GenVertexArrays(1, &vao);
	gl::BindVertexArray(vao);
	checkGLErrors("VAO creation");

	gl::GenBuffers( 1, &nbo );
	gl::BindBuffer( gl::ARRAY_BUFFER, nbo );
	checkGLErrors("NBO binding");
	gl::BufferData( gl::ARRAY_BUFFER, objs[0].shapes[0].mesh.normals.size() * sizeof(float), &objs[0].shapes[0].mesh.normals[0], gl::STATIC_DRAW);
	checkGLErrors("NBO buffer data");
	gl::VertexAttribPointer( gpuLocations.at("normals_attrib"), 3, gl::FLOAT, 0, 0, 0 );
	checkGLErrors("NBO creation");
	gpuLocations.insert( std::pair<const char*, GLuint>("nbo", nbo) );


	if(hasTexture)
	{
		gl::GenBuffers( 1, &tbo );
		gl::BindBuffer( gl::ARRAY_BUFFER, tbo );
		checkGLErrors("TBO binding");
		gl::BufferData( gl::ARRAY_BUFFER, objs[0].shapes[0].mesh.texcoords.size() * sizeof(float), &objs[0].shapes[0].mesh.texcoords[0], gl::STATIC_DRAW);
		checkGLErrors("TBO buffer data");
		gpuLocations.insert( std::pair<const char*, GLuint>("tbo", tbo) );
		gl::VertexAttribPointer( gpuLocations.at("textures_attrib"), 2, gl::FLOAT, 0, 0, 0);
		checkGLErrors("Describe texture vertex attrib");
		gl::EnableVertexAttribArray( gpuLocations.at("textures_attrib") );
		checkGLErrors("Enable texture vertex attrib array");
	}

	gl::GenBuffers( 1, &vbo );
	gl::BindBuffer( gl::ARRAY_BUFFER, vbo );
	gl::BufferData( gl::ARRAY_BUFFER, objs[0].shapes[0].mesh.positions.size() * sizeof(float), &objs[0].shapes[0].mesh.positions[0], gl::STATIC_DRAW );
	checkGLErrors("VBO Creation");
	gpuLocations.insert( std::pair<const char*, GLuint>("vbo", vbo) );

	gl::VertexAttribPointer( gpuLocations.at("vertex_attrib"), 3, gl::FLOAT, 0, 0, 0 );
	checkGLErrors("Configured vbo attribpointer");

	gl::GenBuffers( 1, &ibo );
	gl::BindBuffer( gl::ELEMENT_ARRAY_BUFFER, ibo );
	gl::BufferData( gl::ELEMENT_ARRAY_BUFFER, objs[0].shapes[0].mesh.indices.size() * sizeof(unsigned int), &objs[0].shapes[0].mesh.indices[0], gl::STATIC_DRAW );
	checkGLErrors("IBO creation");

	gl::EnableVertexAttribArray(gpuLocations.at("vertex_attrib"));
	gl::EnableVertexAttribArray(gpuLocations.at("normals_attrib"));
	checkGLErrors("Enabled ibo attrib array");

	//GLuint tex;
	//gl::GenTextures(1, &tex);

	//Normally, here, we'd want to read the diffuse_texname
	//of our loaded object
	//But SOIL, for some odd reason, won't accept char* variables
	if(hasTexture)
	{
		char* texpath = new char[ objs[0].shapes[0].material.diffuse_texname.length() + 1 ];
		std::strcpy ( texpath, objs[0].shapes[0].material.diffuse_texname.c_str() );
		printf("Loading texture ./%s ... \n", texpath);
		GLuint tex = SOIL_load_OGL_texture
		(
			texpath,
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
		);

		checkGLErrors("Generating texture");
		gl::BindTexture(gl::TEXTURE_2D, tex);
		checkGLErrors("Binding texture");
		gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_WRAP_S, gl::REPEAT);
		gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_WRAP_T, gl::REPEAT);
		checkGLErrors("Setting texture wrap mode");

		gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_MIN_FILTER, gl::LINEAR);
		gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_MAG_FILTER, gl::LINEAR);
		checkGLErrors("Setting texture filtering mode");
	}

	gl::BindVertexArray(vao);
	checkGLErrors("Binding vertex array");
}

bool Base::initShader(GLenum type, std::string file)
{
	GLuint shader = gl::CreateShader( type );
	checkGLErrors("initShader: created shader");

	std::string source;

	if(!readFile(file, &source))
	{
		printf("ERROR: Could not read file %s", file.c_str());
		quit();
	}

	gl::ShaderSource( shader, 1, (const GLchar**)&source, NULL );
	checkGLErrors("initShader: set shader source");
	

	gl::CompileShader( shader );
	checkGLErrors("initShader: compiled shader");

	GLint shaderCompiled = gl::FALSE_;
	gl::GetShaderiv( shader, gl::COMPILE_STATUS, &shaderCompiled ); 
	if( shaderCompiled != gl::TRUE_ ) 
	{ 
		printf( "ERROR: Unable to compile shader %d!\n\nSource:\n%s\n", shader, source.c_str() );
		int infoLength = 0;
		int maxL = infoLength;

		gl::GetShaderiv(program, gl::INFO_LOG_LENGTH, &maxL);
		char* infoLog = new char[maxL];

		gl::GetShaderInfoLog(program, maxL, &infoLength, infoLog);
		//if (infoLength > 0)
			printf("GLSHADERIV:%s\n", infoLog);

		gl::DeleteShader( shader );
		shader = 0;

		quit();
	}

	gl::AttachShader( program, shader );
	checkGLErrors("initShader: attached shader");

	return true;
}

void Base::physics()
{


}

void Base::render()
{
	gl::ClearColor(1.0f,0.8f,0.8f,1.0f);
	gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);


	glm::mat4 mv, mvp;
	glm::mat3 rot, normal;

	for(int i = 0; i < objs.size(); i++)
	{
		//Uniforms
		mv = camera * objs[i].transform;
		mvp = projection * mv;
		rot = (glm::mat3)mv;
		normal = glm::inverseTranspose(rot);

		gl::UniformMatrix4fv(gpuLocations.at("mvp"), 1, gl::FALSE_, glm::value_ptr(mvp) );
		gl::UniformMatrix4fv(gpuLocations.at("mv"), 1, gl::FALSE_, glm::value_ptr(mv) );
		gl::UniformMatrix3fv(gpuLocations.at("normal_matrix"), 1, gl::FALSE_, glm::value_ptr(normal) );

		for(int j = 0; j < objs[i].shapes.size(); j++)
		{
			//Normals
			gl::BindBuffer( gl::ARRAY_BUFFER, gpuLocations.at("nbo") );
			gl::BufferData( gl::ARRAY_BUFFER, objs[i].shapes[j].mesh.normals.size() * sizeof(float), &objs[i].shapes[j].mesh.normals[0], gl::STATIC_DRAW);
			gl::VertexAttribPointer( gpuLocations.at("normals_attrib"), 3, gl::FLOAT, 0, 0, 0 );

			if(hasTexture)
			{
				//Texture Coords
				gl::BindBuffer( gl::ARRAY_BUFFER, gpuLocations.at("tbo") );
				gl::BufferData( gl::ARRAY_BUFFER, objs[i].shapes[j].mesh.texcoords.size() * sizeof(float), &objs[i].shapes[j].mesh.texcoords[0], gl::STATIC_DRAW);
				gl::VertexAttribPointer( gpuLocations.at("textures_attrib"), 2, gl::FLOAT, 0, 0, 0);
			}

			//Vertices
			gl::BindBuffer( gl::ARRAY_BUFFER, gpuLocations.at("vbo") );
			gl::BufferData( gl::ARRAY_BUFFER, objs[i].shapes[j].mesh.positions.size() * sizeof(float), &objs[i].shapes[j].mesh.positions[0], gl::STATIC_DRAW );
			gl::VertexAttribPointer( gpuLocations.at("vertex_attrib"), 3, gl::FLOAT, 0, 0, 0 );

			//Indices
			gl::BufferData( gl::ELEMENT_ARRAY_BUFFER, objs[i].shapes[j].mesh.indices.size() * sizeof(unsigned int), &objs[i].shapes[j].mesh.indices[0], gl::STATIC_DRAW );

			gl::DrawElements(gl::TRIANGLES, objs[i].shapes[j].mesh.indices.size(), gl::UNSIGNED_INT, NULL); 
		}
	}
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
			glm::vec3 p1, p2, p3, n;
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
			n = glm::cross( (p3-p1), (p2-p1) );

			n = glm::normalize(n);

			for(int m = 0; m < 3; m++)
			{
				mesh->normals.push_back(n.x);
				mesh->normals.push_back(n.y);
				mesh->normals.push_back(n.z);
			}
			
	}

}

void Base::toggleFullScreen()
{
	int w, h;
	if(!isFullscreen)
	{
		SDL_SetWindowFullscreen(mainWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
		projection = glm::perspective(45.0f, 16.0f / 9.0f, 0.1f, 50.f);
		SDL_GetWindowSize(mainWindow, &w, &h);
		gl::Viewport(0,0,w,h);
		isFullscreen = true;
	}
	else
	{
		SDL_SetWindowFullscreen(mainWindow, 0);
		SDL_SetWindowSize( mainWindow, 800, 600);
		projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 50.f);
		gl::Viewport(0,0,800,600);
		isFullscreen = false;
	}
}

