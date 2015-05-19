#pragma once

#include <typeinfo>

//#include "displayobject.h"
#include "camera.h"


class Scene
{
    public:
        Scene();

        //Gets the list of currently registered
        //display objects
        std::vector<DisplayObject> getDisplayObjects(void);

        //Adds an object to the scene's list of 
        //known objects. Then the engine can
        //determine whether to apply physics,
        //collisions, and render the object
        //Returns true if object was added, false
        //if object is already present in list
        bool registerObject(Object* toAdd);

        //Removes an object from the list of added objects
        //Returns true if object was removed, false if 
        //object was not present in list.
        bool removeObject(Object* toRemove);
        bool removeObject(int index);

        int getObjectIndex( Object* toFind );

        /* --- Camera Code --- */

        //Clears the list of objects.
        //Returns false if list was empty.
        bool clearObjects(void);

        //Adds a camera to the scene.
        void addCamera(Camera toAdd);

        //Sets the active camera to a camera in the list.
        bool setActiveCamera( int index );
        //Switches to this camera in the list if present
        //Adds it to the list, if not
        void setActiveCamera( Camera toActivate );

        //Gets a camera from the scene's camera list
        Camera getCamera( int index );
        Camera* getCameraPtr( int index );

        Camera getActiveCamera( void );
        Camera* getActiveCameraPtr( void );

        //Removes a camera from the scene's camera list
        bool removeCamera(Camera toRemove);
        bool removeCamera(int index);

        int getCameraIndex( Camera toFind );

    private:

        //The list of objects currently registered with the scene.
        std::vector<Object*> objects;
        //The list of potential cameras that may be used within the scene.
        std::vector<Camera> cameras;

        size_t activeCamera = 0;

        template <class T, typename L> int getIndexInList( T toFind, L list );
        template <typename L> bool removeFromList( size_t index, L* list );
        

};
