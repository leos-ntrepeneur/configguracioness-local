# Makefile simple para compilar con g++ (MinGW en Windows, o g++ en Linux/Mac).
# Uso: make        -> compila a bin/inventario_pos(.exe)
#      make clean  -> borra los binarios generados

CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Iinclude
SRC := $(wildcard src/*.cpp)
BIN := bin/inventario_pos

ifeq ($(OS),Windows_NT)
    BIN := bin/inventario_pos.exe
endif

.PHONY: all clean

all: $(BIN)

$(BIN): $(SRC)
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) $(SRC) -o $(BIN)

clean:
	rm -rf bin
