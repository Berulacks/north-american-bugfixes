all:
	gcc -c -std=c99 -o c.o libs/gl_core_3_3.c && \
	g++ -c material.cpp -ggdb -Wwrite-strings -std=c++11 -lGLEW -lGL -lGLU -lSDL2 -lSOIL -lassimp -o material.o && \
	g++ -c model.cpp -ggdb -Wwrite-strings -std=c++11 -lGLEW -lGL -lGLU -lSDL2 -lSOIL -lassimp -o model.o && \
	g++ -c object.cpp -ggdb -Wwrite-strings -std=c++11 -lGLEW -lGL -lGLU -lSDL2 -lSOIL -lassimp -o object.o && \
	g++ -c program.cpp -ggdb -Wwrite-strings -std=c++11 -lGLEW -lGL -lGLU -lSDL2 -lSOIL -lassimp -o program.o && \
	g++ -c storage.cpp -ggdb -Wwrite-strings -std=c++11 -lGLEW -lGL -lGLU -lSDL2 -lSOIL -lassimp -o storage.o && \
	g++ -c renderer.cpp -ggdb -Wwrite-strings -std=c++11 -lGLEW -lGL -lGLU -lSDL2 -lSOIL -lassimp -o renderer.o && \
	g++ -c engine.cpp -ggdb -Wwrite-strings -std=c++11 -lGLEW -lGL -lGLU -lSDL2 -lSOIL -lassimp -o engine.o && \
	ar rcs libNab.a c.o material.o model.o object.o program.o storage.o renderer.o engine.o 
pedestal:
	make && \
	g++ -L. pedestal.cpp -Wwrite-strings -std=c++11 -lGLEW -lGL -lGLU -lSDL2 -lSOIL -lassimp -lNab -o pedestal

clean:
	rm -f *.o
