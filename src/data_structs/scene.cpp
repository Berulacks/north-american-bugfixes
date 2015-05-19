#include "scene.h"

template <class T, typename L> int Scene::getIndexInList( T toFind, L list )
{
    auto itr = std::find(list.begin(), list.end(), toFind);

    if( itr != list.end())
    {
        size_t position = list.end() - itr;

        return (int)position;
    }
    return -1;
}

template <typename L> bool Scene::removeFromList( size_t index, L* list )
{
    if( index >= list->size() )
        return false;

    list->erase( list->begin() + index );
    return true;
}

std::vector<DisplayObject> Scene::getDisplayObjects()
{
    std::vector<DisplayObject> toReturn;

    for( int i = 0; i < objects.size(); i++ )
    {
        if(DisplayObject* obj = dynamic_cast<DisplayObject*>(objects[i]) )
            toReturn.push_back( *obj );
    }

    return toReturn;
}

bool Scene::registerObject(Object* toAdd)
{
    if( getObjectIndex(toAdd) == -1 )
    {
        objects.push_back(toAdd);
        return true;
    }
    return false;
}

bool Scene::removeObject(Object* toRemove)
{
    int index = getObjectIndex( toRemove );
    return removeObject( index );
}

bool Scene::removeObject( int index )
{
    /*if( index >= objects.size() || index < 0 )
        return false;

    objects.erase( objects.begin() + index );
    return true;*/
    return removeFromList( index, &objects );
}

int Scene::getObjectIndex( Object* toFind )
{

    /*auto itr = std::find(objects.begin(), objects.end(), toFind);

    if( itr != objects.end())
    {
        size_t position = objects.end() - itr;

        return (int)position;
    }
    return -1;*/

    return getIndexInList( toFind, objects );
}

bool Scene::clearObjects()
{
    if( objects.size() == 0 )
        return false;
    objects = std::vector<Object*>();
    return true;
}

void Scene::addCamera(Camera toAdd)
{
    cameras.push_back(toAdd);
}

bool Scene::setActiveCamera( int index )
{
    if( index > cameras.size() )
    {
        activeCamera = index;
        return true;
    }

    return false;
}

Camera Scene::getCamera( int index )
{
    return cameras[index];
}

Camera* Scene::getCameraPtr( int index )
{
    return &(cameras[index]);
}

Camera Scene::getActiveCamera()
{
    return cameras[activeCamera];
}

Camera* Scene::getActiveCameraPtr()
{
    return &(cameras[activeCamera]);
}

int Scene::getCameraIndex( Camera toFind )
{
    return getIndexInList( toFind, cameras );
    return -1;
}

bool Scene::removeCamera( Camera toRemove )
{
    return removeCamera( getCameraIndex(toRemove) );
}

bool Scene::removeCamera( int index )
{
    return removeFromList( index, &cameras );
    return false;
}

Scene::Scene()
{

}
