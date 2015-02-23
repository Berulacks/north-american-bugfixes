#include "material.h"

void Material::updateVariables(aiMaterial* mtl)
{
    aiString texPath;   //contains filename of texture
        if(AI_SUCCESS == mtl->GetTexture(aiTextureType_DIFFUSE, 0, &texPath)){
        texDiffuse_name = std::string(texPath.data);
            }
    else
        texDiffuse_name = std::string("NONE");
 
        aiColor4D _diffuse;
        if(AI_SUCCESS == mtl->Get(AI_MATKEY_COLOR_DIFFUSE, _diffuse))
    {
        diffuse.x = _diffuse[0];
        diffuse.y = _diffuse[1];
        diffuse.z = _diffuse[2];

    }
 
        aiColor4D _specular;
        if(AI_SUCCESS == mtl->Get(AI_MATKEY_COLOR_DIFFUSE, _specular))
    {
        specular.x = _specular[0];
        specular.y = _specular[1];
        specular.z = _specular[2];

    }
 
        aiColor4D _emissive;
        if(AI_SUCCESS == mtl->Get(AI_MATKEY_COLOR_DIFFUSE, _emissive))
    {
        emissive.x = _emissive[0];
        emissive.y = _emissive[1];
        emissive.z = _emissive[2];

    }

        aiColor4D _ambient;
        if(AI_SUCCESS == mtl->Get(AI_MATKEY_COLOR_DIFFUSE, _ambient))
    {
        ambient.x = _ambient[0];
        ambient.y = _ambient[1];
        ambient.z = _ambient[2];
    }

    aiString _name;
    mtl->Get( AI_MATKEY_NAME, _name );
    name = std::string(_name.C_Str());
    printf("Initialized material %s!\n", name.c_str());

        unsigned int max;
        aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS, &shininess, &max);

        ready = true;
}
