#include "material.h"

void Material::updateVariables(aiMaterial* mtl, std::map<const char *, GLuint> textureMap)
{
	aiString texPath;   //contains filename of texture
        if(AI_SUCCESS == mtl->GetTexture(aiTextureType_DIFFUSE, 0, &texPath)){
                //bind texture
		texDiffuse_name = texPath.data;
		if(std::find(textureMap.begin(), textureMap.end(), texPath.data) == textureMap.end())
		{
			unsigned int texId = textureMap[texPath.data];
			texDiffuse = texId;
		}
            }
 
        aiColor4D _diffuse;
        if(AI_SUCCESS == mtl->Get(AI_MATKEY_COLOR_DIFFUSE, _diffuse))
	{
		diffuse.x = _diffuse[0];
		diffuse.x = _diffuse[1];
		diffuse.x = _diffuse[2];

	}
 
        aiColor4D _specular;
        if(AI_SUCCESS == mtl->Get(AI_MATKEY_COLOR_DIFFUSE, _specular))
	{
		specular.x = _specular[0];
		specular.x = _specular[1];
		specular.x = _specular[2];

	}
 
        aiColor4D _emissive;
        if(AI_SUCCESS == mtl->Get(AI_MATKEY_COLOR_DIFFUSE, _emissive))
	{
		emissive.x = _emissive[0];
		emissive.x = _emissive[1];
		emissive.x = _emissive[2];

	}

        aiColor4D _ambient;
        if(AI_SUCCESS == mtl->Get(AI_MATKEY_COLOR_DIFFUSE, _ambient))
	{
		ambient.x = _ambient[0];
		ambient.x = _ambient[1];
		ambient.x = _ambient[2];
	}

	aiString _name;
	mtl->Get( AI_MATKEY_NAME, _name );
	name = _name.C_Str();
	printf("Initialized material %s!\n", name);

        unsigned int max;
        aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS, &shininess, &max);
}
