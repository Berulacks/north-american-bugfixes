#include "renderer.h"
Renderer::Renderer()
{
	printf("Renderer created!\n");
}

void Renderer::render(std::vector<Object> objects)
{
	glClearColor(1.0f,0.8f,0.8f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glm::mat4 mv, mvp;
	glm::mat3 rot, normal;
	
	//Eventually this will be replaced
	//with a custom datatype
	const aiScene* scene;
	Model* model;
	Material mat;
	Program* shader;

	for(int i = 0; i < objects.size(); i++)
	{
		model = objects[i].getModel();
		scene = model->getScene();

		for(int j = 0; j < model->numMeshes(); j++)
		{
			mat = model->materials[i];
			shader = mat.shader;

			setActiveProgram( shader );

			glBindVertexArray( model->getVAO( i ) );
			glBindTexture( GL_TEXTURE_2D, mat.texDiffuse );

			//Update uniforms just loads the constant uniforms, e.g. Ld and stuff.
			//This will obviously need to be abstracted in the future
			updateUniforms( objects[i] );
			
			glDrawElements(GL_TRIANGLES, scene->mMeshes[j]->mNumFaces * 3, GL_UNSIGNED_INT, NULL); 
			glFlush();

			glBindVertexArray( 0 );
			glBindTexture( GL_TEXTURE_2D, 0 );
		}

		setActiveProgram( 0 );
	}
	//glFlush();
	Storage::checkGLErrors("Post render");

}

void Renderer::setActiveProgram(Program *toSet)
{
	if(toSet != 0)
	{
		glUseProgram( toSet->getID() );
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

	printf("OpenGL initialized!\n");

	return Storage::checkGLErrors("End of initGL");
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
	glUniform4fv(activeProgram->getUniform("LightPosition"), 1, glm::value_ptr( glm::column(-camera, 3) ) );

	//THIS NEEDS TO BE MOVED SOMEWHERE ELSE
	//THIS NEEDS TO BE DONE ON A PER-MESH BASIS
	glm::vec3 kD = obj.getModel()->materials[0].diffuse;
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
