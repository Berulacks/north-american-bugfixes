all:
	g++ gl_core_3_3.cpp tiny_obj_loader.cc base.cpp main.cpp \
		-ggdb\
		-std=c++11\
		-lGLEW -lGL -lGLU -lSDL2 -lSOIL\
		-o runfile\
	&& ./runfile 
