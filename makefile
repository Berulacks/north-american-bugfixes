all:
	gcc -c -std=c99 -o c.o libs/gl_core_3_3.c && \
	g++ c.o material.cpp model.cpp object.cpp program.cpp storage.cpp renderer.cpp engine.cpp main.cpp \
		-ggdb\
		-Wwrite-strings\
		-std=c++11\
		-lGLEW -lGL -lGLU -lSDL2 -lSOIL -lassimp\
		-o pedestal
test:
	gcc -c -std=c99 -o c.o libs/gl_core_3_3.c && \
	g++ c.o material.cpp model.cpp object.cpp program.cpp storage.cpp renderer.cpp engine.cpp main.cpp \
		-ggdb\
		-Wwrite-strings\
		-std=c++11\
		-lGLEW -lGL -lGLU -lSDL2 -lSOIL -lassimp\
		-o testfile && ./testfile './models/sphere/sphere.obj' ; rm -f testfile
