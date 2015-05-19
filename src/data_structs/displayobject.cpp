#include "displayobject.h"

DisplayObject::DisplayObject(Model mod, Material* mat)
{
    model = mod;
    if(mat != NULL)
    {
        customMatAvailable = true;
        customMat = *mat;
    }
}

Material DisplayObject::getMaterial(size_t meshIndex)
{
    //if(customMat != NULL)
    if(customMatAvailable)
        return customMat;
    else
        //To be changed when moving away from ASSIMP
        return (getModel().materials[ meshIndex ]);
}

void DisplayObject::setMaterial( Material mat )
{
    this->customMat = mat;
    customMatAvailable = true;
}
