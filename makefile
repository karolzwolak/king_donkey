COMPILER=g++
FLAGS= -O2 -I./SDL2-2.0.10/include -L.-lm -lSDL2 -lpthread -ldl -lrt -o bin
WARNING_FLAGS = -Wall -Wextra -Werror -Wno-error=unused-variable -Wno-error=unused-parameter

all: main

main: main.cpp
	$(COMPILER) -g3 $(FLAGS) main.cpp

release:
	$(COMPILER) $(FLAGS) main.cpp

example:
	$(COMPILER) $(FLAGS) example.cpp

run: main
	./bin

clean:
	rm bin
