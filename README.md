north-american-bugfixes
=======================

An opengl 3d renderer. Github recommended the title, and I happen to think it's brilliant. Unlike *some* people, I support github's creative endeavors.

###Requirements

Nab requires:

* ASSIMP
* SOIL
* SDL2
* OpenGL 3.3+

###Building

Running the included makefile without arguments will produce a static-library file (libNab.a) that can be included and used with any project. Just make sure to add the "*/src*" directory as an include folder, and libNab.a as a library.

The project also includes two example implementations: pedestal (a model viewer), and bullet_demo (a small physics playground that requires [bullet](http://bulletphysics.org/))

To build pedestal, run:

    make pedestal

To build the bullet_demo, run:

    make bullet_demo
    
These commands (and just running make without arguments) will produce binaries in the "*build/*" directory. Both demoes take a model file path as an argument, which controls what gets rendered on the screen.
