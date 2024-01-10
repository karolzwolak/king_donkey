COMPILER=g++
FLAGS= -O2 -I./SDL2-2.0.10/include -L.-lm -lSDL2 -lpthread -ldl -lrt -o bin
WARNING_FLAGS = -Wall -Wextra -Werror -Wno-error=unused-variable -Wno-error=unused-parameter
SRC_DIR = src

all: main

main: src/main.cpp
	$(COMPILER) -g3 $(FLAGS) $(SRC_DIR)/main.cpp

release:
	$(COMPILER) $(FLAGS) $(SRC_DIR)/main.cpp

example:
	$(COMPILER) $(FLAGS) $(SRC_DIR)/example.cpp

run: main
	./bin

clean:
	rm bin
