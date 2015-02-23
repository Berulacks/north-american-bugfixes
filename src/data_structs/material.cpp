#include "material.h"

void Material::updateVariables(aiMaterial* mtl)
{
    aiString texPath;   //contains filename of texture

    if(AI_SUCCESS == mtl->GetTexture(aiTextureType_DIFFUSE, 0, &texPath))
    {
        texDiffuse_name = std::string(texPath.data);
    }
    else
        texDiffuse_name = std::string("NONE");
 
    aiColor4D _tempVector;

    if(AI_SUCCESS == mtl->Get(AI_MATKEY_COLOR_DIFFUSE, _tempVector))
    {
        diffuse.x = _tempVector[0];
        diffuse.y = _tempVector[1];
        diffuse.z = _tempVector[2];
    }
 
    if(AI_SUCCESS == mtl->Get(AI_MATKEY_COLOR_SPECULAR, _tempVector))
    {
        specular.x = _tempVector[0];
        specular.y = _tempVector[1];
        specular.z = _tempVector[2];
    }
 
    if(AI_SUCCESS == mtl->Get(AI_MATKEY_COLOR_EMISSIVE, _tempVector))
    {
        emissive.x = _tempVector[0];
        emissive.y = _tempVector[1];
        emissive.z = _tempVector[2];
    }

    if(AI_SUCCESS == mtl->Get(AI_MATKEY_COLOR_AMBIENT, _tempVector))
    {
        ambient.x = _tempVector[0];
        ambient.y = _tempVector[1];
        ambient.z = _tempVector[2];
    }

    if(AI_SUCCESS == mtl->Get(AI_MATKEY_COLOR_TRANSPARENT, _tempVector))
    {
        transparent.x = _tempVector[0];
        transparent.y = _tempVector[1];
        transparent.z = _tempVector[2];
    }

    if(AI_SUCCESS == mtl->Get(AI_MATKEY_COLOR_REFLECTIVE, _tempVector))
    {
        reflective.x = _tempVector[0];
        reflective.y = _tempVector[1];
        reflective.z = _tempVector[2];
    }

    aiString _name;
    mtl->Get( AI_MATKEY_NAME, _name );
    name = std::string(_name.C_Str());
    printf("Initialized material %s!\n", name.c_str());

    unsigned int max;
    aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS, &shininess, &max);

    ready = true;
}
