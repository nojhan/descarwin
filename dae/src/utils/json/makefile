# This makefile is here to unit test the JSON serialization module.
# Don't use it if your will is to compile DAE with it.

OBJ=Entity.o Array.o String.o Object.o Parser.o test.o
CXX=g++
CXXFLAGS=-Wall -ggdb
EXEC=json

all: $(OBJ)
	$(CXX) -o $(EXEC) $^

%.o: %.cpp %.h
	$(CXX) -c $< -o $@ $(CXXFLAGS)

.PHONY:clean

clean:
	rm -rf *.o

.PHONY: mrproper

mrproper: clean
	rm -rf $(EXEC)
