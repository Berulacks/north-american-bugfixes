#include "../program.h"
#include <glm/glm.hpp>
//#include "../storage.h"

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

		//Update the uniforms in our shader object
		//Each material should be able to extract the required
		//information from each object for updating each uniform.
		//virtual void updateUniforms(Object dataStore);

};
