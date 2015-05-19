#include "displayobject.h"

DisplayObject::DisplayObject(Model mod, Material* mat)
{
    model = mod;
    if(mat != NULL)
        customMat = mat;
}

Material* DisplayObject::getMaterial()
{
    return customMat;
}
