all: tree
	g++ mnog.cpp tree.o -Wall -o mnog

tree: tree.cpp
	g++ -c tree.cpp
