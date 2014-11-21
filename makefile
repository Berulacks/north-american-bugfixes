.PHONY: all pedestal bullet_demo clean

all:
	mkdir -p build && \
	gcc -c -std=c99 -o c.o ./src/libs/gl_core_3_3.c && \
	g++ -c ./src/data_structs/material.cpp -ggdb -Wwrite-strings -std=c++11 -lGL -lGLU -lSDL2 -lSOIL -lassimp -o material.o && \
	g++ -c ./src/data_structs/model.cpp -ggdb -Wwrite-strings -std=c++11 -lGL -lGLU -lSDL2 -lSOIL -lassimp -o model.o && \
	g++ -c ./src/data_structs/object.cpp -ggdb -Wwrite-strings -std=c++11 -lGL -lGLU -lSDL2 -lSOIL -lassimp -o object.o && \
	g++ -c ./src/renderer/program.cpp -ggdb -Wwrite-strings -std=c++11 -lGL -lGLU -lSDL2 -lSOIL -lassimp -o program.o && \
	g++ -c ./src/engine/storage.cpp -ggdb -Wwrite-strings -std=c++11 -lGL -lGLU -lSDL2 -lSOIL -lassimp -o storage.o && \
	g++ -c ./src/renderer/renderer.cpp -ggdb -Wwrite-strings -std=c++11 -lGL -lGLU -lSDL2 -lSOIL -lassimp -o renderer.o && \
	g++ -c ./src/engine/engine.cpp -ggdb -Wwrite-strings -std=c++11 -lGL -lGLU -lSDL2 -lSOIL -lassimp -o engine.o && \
	ar rcs build/libNab.a c.o material.o model.o object.o program.o storage.o renderer.o engine.o && \
	rm -f *.o
pedestal:
	make && \
	g++ -L./build -I./src ./examples/pedestal.cpp -Wwrite-strings -std=c++11 -ggdb -lGL -lGLU -lSDL2 -lSOIL -lassimp -I./src -lNab -o build/pedestal && \
	rm -f *.o

bullet_demo:
	make && \
	g++ -L./build ./examples/bullet_demo.cpp -Wwrite-strings -std=c++11 -ggdb -I/usr/include/bullet -lBulletDynamics -lBulletCollision -lLinearMath -lGL -lGLU -lSDL2 -lSOIL -lassimp -I./src -lNab -o ./build/bullet_demo && \
	rm -f *.o

clean:
	rm -rf *.o build *.pyc ./examples/*.pyc
