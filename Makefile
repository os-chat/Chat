CC := g++
CFLAGS := -std=c++17 -Wall -lpthread -lrt

SRCFILES := $(wildcard src/*.cpp)

all: $(SRCFILES:src/%.cpp=obj/%.o)
	$(CC) $(CFLAGS) obj/*.o -o chat -lpthread -lrt

obj/%.o: src/%.cpp
	@mkdir -p obj
	$(CC) $(CFLAGS) -c $< -o $@ -I./inc

.PHONY: clean
clean:
	rm -rf obj/*