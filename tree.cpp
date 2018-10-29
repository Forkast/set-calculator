#include "tree.hpp"
#include <regex>

using namespace std;

Node * create_node(const string & name)
{
	if (!CMD[SUM].compare(name)) {
		return new SumNode{};
	} else if (!CMD[INT].compare(name)) {
		return new IntNode{};
	} else if (!CMD[SUB].compare(name)) {
		return new SubNode{};
	}
	return nullptr;
}

void
Node::set_false()
{
	ready = false;
	if (parent->ready)
		parent->set_false();
}

int
SubNode::oper(int time)
{
	if (ready)
		return val;

	int first = children[0]->oper(time);
	
	if (time < first)
		return -1;
	
	for (size_t i = 1; i < children.size(); i++) {
		if (first == children[i]->oper(time))
			return -1;
	}
	ready = true;
	val = first;
	return first;
}

int
IntNode::oper(int time)
{
	if (ready)
		return val;

	int first = children[0]->oper(time);

	if (first > time)
		return -1;

	for (size_t i = 1; i < children.size(); i++) {
		int var = children[i]->oper(time);
		if (first != var)
			return -1;
	}
	ready = true;
	val = first;
	return first;
}

int
SumNode::oper(int time)
{
	if (ready)
		return val;

	for (size_t i = 0; i < children.size(); i++) {
		int var = children[i]->oper(time);
		if (time == var) {
			ready = true;
			val = time;
			return val;
		}
	}
	return -1;
}

int
Leaf::oper(int time) {
	return val;
}

Tree::Tree(int argc, char * argv[], std::vector <std::string> & files)
	: root{nullptr}
{
	int state = 1; // state machine
	int openbrak = 0; // heap
	std::regex txt_regex{"[a-z]+\\.txt"}; // zakladam ze nazwa pliku to *.txt
	Node * current = nullptr;
	Node * next = nullptr;
	for (int i = 1; i < argc; i++) {
		if (state == -1) {
			error("Unexpected symbol");
		}
		if (!strcmp("[", argv[i])) {
			openbrak++;
			state = trans[state][0];
		} else if ((next = create_node(argv[i])) != nullptr) {
			if (root == nullptr) {
				root = next;
				root->parent = root;
			} else {
				current->children.push_back(next);
				next->parent = current;
			}
			current = next;
			next = nullptr;
			state = trans[state][1];
		} else if (!strcmp("]", argv[i])) {
			openbrak--;
			state = trans[state][2];
			if (openbrak < 0)
				state = -1;
			if (current != nullptr) {
				current = current->parent;
			}
		} else { // expecting file name
			if (regex_match(argv[i], txt_regex)) {
				std::string filename{argv[i]};
				int file_no = -1;
				auto it = std::find(files.begin(), files.end(), filename);
				if (it == files.end()) {
					files.push_back(filename);
					file_no = files.size() - 1;
				} else {
					file_no = it - files.begin();
				}
				next = new Leaf();
				next->parent = current;
				current->children.push_back(next);
				leaves[file_no].push_back(next);
				state = trans[state][3];
			} else {
				state = -1;
			}
		}
	}
	if (state == 5 && openbrak == 0)
		return;
	else
		error("Bad expression");
}

Tree::~Tree()
{
	
}

void
Tree::update(int time, const vector <int> & changed, const vector <int> & values)
{
	for (size_t i = 0; i < changed.size(); i++) {
		for (size_t j = 0; j < leaves[changed[i]].size(); j++) {
			leaves[changed[i]][j]->val = values[changed[i]];
			leaves[changed[i]][j]->set_false();
		}
	}
}

void
Tree::read(int time)
{
	int val = root->oper(time);
	if (val == time)
		printf("%d\n", val);
}
