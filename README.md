north-american-bugfixes
=======================

An opengl 3d renderer. Github recommended the title, and I happen to think it's brilliant. Unlike *some* people, I support github's creative endeavors.

###Requirements

Nab requires:

* [ASSIMP](http://assimp.sourceforge.net)
* [SOIL](http://www.lonesock.net/soil.html)
* [SDL2](https://www.libsdl.org/)
* [GLM](http://glm.g-truc.net/0.9.6/index.html)
* OpenGL 3.3+

###Building

As it stands right now, the project only builds on \*nix systems. Running the included makefile without arguments will produce a static-library file (libNab.a) that can be included and used with any project. Just make sure to add the "*/src*" directory as an include folder, and libNab.a as a library.

The project also includes two example implementations: pedestal (a model viewer), and bullet_demo (a small physics playground that requires [bullet](http://bulletphysics.org/))

To build pedestal, run:

    make pedestal

To build the bullet_demo, run:

    make bullet_demo
    
These commands (and just running make without arguments) will produce binaries in the "*build/*" directory. Both demos take a model file path as an argument, which controls what gets rendered on the screen. **NB:** Because of relative path usage in the examples, please run them from inside the main project directory, and not inside the "*build/*" directory (as in, your shell's $PWD should be one directory above "*build/*").
