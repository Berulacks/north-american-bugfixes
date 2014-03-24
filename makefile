all:
	gcc -c -std=c99 -o c.o gl_core_3_3.c && \
	g++ c.o tiny_obj_loader.cc base.cpp main.cpp \
		-ggdb\
		-Wwrite-strings\
		-std=c++11\
		-lGLEW -lGL -lGLU -lSDL2 -lSOIL\
		-o runfile \
	&& ./runfile 
