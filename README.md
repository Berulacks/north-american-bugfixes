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

Running the included makefile without arguments will produce a static-library file (libNab.a) that can be included and used with any project. Just make sure to include the necessary header files with said project.

In order to see a reference implementation of Nab, run

    make pedestal
    
to build Pedestal, a model viewer built with Nab. It takes a model filepath as an argument.
