#include "program.h"
#include <glm/glm.hpp>
#include <assimp/scene.h>
#include <assimp/material.h>

class Storage;

class Material
{
	public:
		Program* shader;
		const char* name;

		glm::vec3 diffuse;
		glm::vec3 specular;
		glm::vec3 ambient;
		glm::vec3 emissive;

		float shininess;

		GLuint texDiffuse;
		const char* texDiffuse_name;
		GLuint texSpecular;
		const char* texSpecular_name;

		Material(Program* _shader)
		{
			shader = _shader;
		};

		//Update our materials variables
		//(e.g. diffuse, specular)
		//with data stored in our aiScene
		void updateVariables(aiMaterial* mtl, std::map<const char*, GLuint> textureMap);

};
