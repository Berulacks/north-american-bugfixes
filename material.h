#include "program.h"
#include <glm/glm.hpp>
#include <assimp/scene.h>

class Storage;

class Material
{
	public:
		Program* shader;
		const char* name;

		glm::vec3 diffuse;
		glm::vec3 specular;
		glm::vec3 transparent;
		glm::vec3 emissive;

		GLuint texDiffuse;
		GLuint texSpecular;

		Material(Program* _shader)
		{
			shader = _shader;
		};

		//Update our materials variables
		//(e.g. diffuse, specular)
		//with data stored in our aiScene
		void updateVariables(aiMaterial* mat);

};
