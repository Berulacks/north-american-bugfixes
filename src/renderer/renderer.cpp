#include "renderer.h"
Renderer::Renderer()
{
	printf("Renderer created!\n");
}

void Renderer::render(std::vector<Object*> objects)
{
	glClearColor(1.0f,0.8f,0.8f,1.0f);
    Storage::checkGLErrors( "glClearColor" );
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    Storage::checkGLErrors( "glClear" );


	glm::mat4 mv, mvp;
	glm::mat3 rot, normal;
	
    ModelData data;
	Model model;
	Material mat;
	Program* shader;

	for(int i = 0; i < objects.size(); i++)
	{
		model = objects[i]->getModel();

		for(int j = 0; j < model.numMeshes(); j++)
		{
			mat = model.materials[ model.getBCombo(j).matIndex ];
			shader = mat.shader;

			setActiveProgram( shader );
            Storage::checkGLErrors( "Setting active program" );

			glBindVertexArray( model.getVAO( j ) );
            Storage::checkGLErrors("Binding vertex array");
			glBindTexture( GL_TEXTURE_2D, mat.texDiffuse );
            Storage::checkGLErrors( "Binding texture" );
			
			//Update uniforms just loads the constant uniforms, e.g. Ld and stuff.
			updateUniforms( *objects[i] );
			
			glDrawElements(GL_TRIANGLES, model.getBCombo(j).numIndices, GL_UNSIGNED_INT, NULL); 

			if( objects[i]->renderBoundingBox )
			{
				setActiveProgram( simplePr );
				glBindVertexArray( bBoxVao );
				glBindTexture( GL_TEXTURE_2D, 0 );
				glBindBuffer( GL_ARRAY_BUFFER, model.getBCombo( j ).boundingBox );
				//When you finally fix updateUniforms such that it isn't horrible, make sure to give a way to only send the mvp matrix in, so we can delete this line
				glm::mat4 mv = projection * (camera * objects[i]->getTransform());
				glUniformMatrix4fv(activeProgram->getUniform("mvp"), 1, GL_FALSE, glm::value_ptr(mv) );

				glVertexAttribPointer( simplePr->getAttrib("theV"),3,GL_FLOAT,0,0,0);
				glEnableVertexAttribArray(simplePr->getAttrib("theV"));
				glDrawElements(GL_LINE_STRIP,20, GL_UNSIGNED_INT, NULL); 

			}

		}

		glBindVertexArray( 0 );
		glBindTexture( GL_TEXTURE_2D, 0 );

		setActiveProgram( 0 );
	}
	glFlush();
	Storage::checkGLErrors("Post render");

}

void Renderer::setActiveProgram(Program *toSet)
{
	if(toSet != 0)
	{
		glUseProgram( toSet->getID() );
        Storage::checkGLErrors( "glUseProgram: toSet->getID()" );
		activeProgram = toSet;
	}
	else
	{
		glUseProgram( 0 );
		activeProgram = NULL;
	}
}

bool Renderer::initGL()
{
	printf("Initializing openGL...\n");
	glEnable(GL_DEPTH_TEST);
	glViewport(0,0,800,600);
	
	projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 50.f);
	cameraPos = glm::vec3(0.0f, 1.0f, -1.0f);

	glm::vec3 lookat;
	lookat.x = sinf(xRot) * cosf(yRot);
	lookat.y = sinf(yRot);
	lookat.z = cosf(xRot) * cosf(yRot);

	camera = glm::lookAt(cameraPos, cameraPos + lookat, glm::vec3(0, 1, 0));

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);

	simplePr = new Program("./src/shaders/simple.vs", "./src/shaders/simple.fs");

	GLuint bBoxIbo;

	glGenVertexArrays(1, &bBoxVao);
	glBindVertexArray(bBoxVao);

	glGenBuffers(1, &bBoxIbo);
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, bBoxIbo );
	GLuint indices[20] = {0, 1, 0, 2, 3, 1, 0, 4, 5, 1, 5, 4, 6, 2, 6, 7, 3, 7, 5, 1};
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, 20 * sizeof(GLuint),indices, GL_STATIC_DRAW); 

	glEnableVertexAttribArray(simplePr->getAttrib("theV"));

	glBindVertexArray(0);

	boundingBox.name = "BoundingBox";
	boundingBox.vao = bBoxVao;
	boundingBox.indices = bBoxIbo;

	//CheckGLErrors returns true if an error was found
	//so we return the opposite
	return !Storage::checkGLErrors("End of initGL");
}

void Renderer::updateProjection(glm::mat4 projection)
{
	this->projection = projection;
}


void Renderer::toggleFullScreen(SDL_Window* mainWindow)
{
	int w, h;
	if(!isFullScreen)
	{
		SDL_SetWindowFullscreen(mainWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
		updateProjection( glm::perspective(45.0f, 16.0f / 9.0f, 0.1f, 50.f) );
		SDL_GetWindowSize(mainWindow, &w, &h);
		printf("Going fullscreen: %ix%i\n", w, h);
		glViewport(0,0,w,h);
		isFullScreen = true;
	}
	else
	{
		SDL_SetWindowFullscreen(mainWindow, 0);
		SDL_SetWindowSize( mainWindow, 800, 600);
		updateProjection( glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 50.f)) ;
		printf("Leaving fullscreen!\n");
		glViewport(0,0,800,600);
		isFullScreen = false;
	}
}


void Renderer::updateUniforms( Object obj, Program* program )
{
	
	bool prSwitch = false;
	Program* backup;
	if(program != NULL)
	{
		prSwitch = true;
		backup = activeProgram;
		setActiveProgram( program );

	}

	//Uniforms
	glm::mat4 mv = camera * obj.getTransform();
	glm::mat4 mvp = projection * mv;
	glm::mat3 rot = (glm::mat3)mv;
	glm::mat3 normal = glm::inverseTranspose(rot);

	glUniformMatrix4fv(activeProgram->getUniform("mvp"), 1, GL_FALSE, glm::value_ptr(mvp) );
	glUniformMatrix4fv(activeProgram->getUniform("mv"), 1, GL_FALSE, glm::value_ptr(mv) );
	glUniformMatrix3fv(activeProgram->getUniform("normal_matrix"), 1, GL_FALSE, glm::value_ptr(normal) );
	
	//FOR TESTING PURPOSES ONLY, REMOVE THIS LINE
	//glUniform4fv(activeProgram->getUniform("LightPosition"), 1, glm::value_ptr( glm::column(-camera, 3) ) );
	glUniform4fv(activeProgram->getUniform("LightPosition"), 1, glm::value_ptr( glm::vec3(0,8,0) ) );

	//THIS NEEDS TO BE MOVED SOMEWHERE ELSE
	//THIS NEEDS TO BE DONE ON A PER-MESH BASIS
	glm::vec3 kD = obj.getModel().materials[0].diffuse;
	//Light constant
	glUniform3fv(activeProgram->getUniform("Kd"), 1, glm::value_ptr( kD ) );
	//Light intensity
	//THIS NEEDS TO BE CONTROLLED BY OUR LIGHTING ENGINE. THIS IS ONLY
	//A STUB AND NEEDS TO BE MOVED SOMEWHERE ELSE WHEN THE PROGRAM
	//SUPPORTS MORE THAN ONE LIGHT
	glUniform3fv(activeProgram->getUniform("Ld"), 1, glm::value_ptr( glm::vec3(0.9, 0.9, 0.9) ) );
	Storage::checkGLErrors("Updating uniforms");

	if( prSwitch )
		setActiveProgram( backup );
}
