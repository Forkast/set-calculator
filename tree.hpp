#pragma once
#include <vector>
#include <string>

enum CNUM {
	SUM,
	INT,
	SUB,
	END
};

const
std::string CMD[] = {
	"SUM",
	"INTERSECT",
	"SUBTRACT"
};

// states:
// 1 - begin
// 2 - after open braket
// 3 - after command
// 4 - after file

// symbols:
// 0 - [
// 1 - cmd
// 2 - ]
// 3 - file
const int trans[6][4] = {
	{-1, -1, -1, -1},
	{2, -1, -1, -1},
	{2, 3, -1, -1},
	{2, -1, -1, 4},
	{2, -1, 5, 4},
	{2, -1, 5, 4}
};

inline 
void error(const char * msg)
{
	fprintf(stderr, "%s\n", msg);
	exit(-1);
}

struct Node
{
	Node() : parent(nullptr), ready(false), val(-1) {}
	Node * parent;
	std::vector <Node *> children;
	virtual int oper(int time) {
		return -1;
	};
	void set_false();
	bool ready;
	int val;
};

struct SubNode : public Node
{
	int oper(int time);
};

struct IntNode : public Node
{
	int oper(int time);
};

struct SumNode : public Node
{
	int oper(int time);
};

struct Leaf : public Node
{
	Leaf() {}
	int oper(int time);
};

class Tree
{
public:
	Tree(int argc, char * argv[], std::vector <std::string> & files);
	~Tree();
	void update(int time, const std::vector <int> & changed, const std::vector <int> & values);
	void read(int time);

private:
	Node * root;
	std::vector <Node *> leaves[101];
};
