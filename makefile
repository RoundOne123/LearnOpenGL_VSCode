#g++ src/main.cpp src/Chapter1-Hello-Window/HelloWindow.cpp -g -std=c++17 -I ./include -L ./lib -o main -lglad -lglfw3dll
CXX		:= g++
CXX_FLAGS       := -g -std=c++17 #-Wextra -Wall

INCLUDE         := ./include
LIB		:= ./lib
LIBRARIES	:= -lglad -lglfw3dll

SRC = $(wildcard src/*.cpp src/Chapter1-Hello-Window/*.cpp)
EXECUTABLE := main

$(EXECUTABLE): $(SRC)
	$(CXX) $(SRC) -g $(CXX_FLAGS) -I $(INCLUDE) -L $(LIB) -o $(EXECUTABLE) $(LIBRARIES)