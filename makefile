all:
	g++ tiny_obj_loader.cc base.cpp main.cpp \
		-ggdb\
		-std=c++11\
		-lGLEW -lGL -lGLU -lSDL2 -lSOIL\
		-o runfile\
	&& ./runfile 
