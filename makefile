COMPILER=g++
FLAGS= -O2 -I./SDL2-2.0.10/include -L.-lm -lSDL2 -lpthread -ldl -lrt -o bin main.cpp 
WARNING_FLAGS = -Wall -Wextra -Werror -Wno-error=unused-variable -Wno-error=unused-parameter

all: main

main: main.cpp
	$(COMPILER) -g3 $(FLAGS)

release:
	$(COMPILER) $(FLAGS)

run: main
	./bin

clean:
	rm bin
