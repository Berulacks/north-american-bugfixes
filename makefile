all:
	gcc -c -std=c99 -o c.o libs/gl_core_3_3.c && \
	g++ c.o base.cpp main.cpp \
		-ggdb\
		-Wwrite-strings\
		-std=c++11\
		-lGLEW -lGL -lGLU -lSDL2 -lSOIL -lassimp\
		-o runfile \
	&& ./runfile 
