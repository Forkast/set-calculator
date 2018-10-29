#include <string>
#include <vector>

#include "tree.hpp"

using namespace std;

int main(int argc, char * argv[])
{
	vector <string> file_names;
	Tree * t = new Tree(argc, argv, file_names);
	vector <FILE *> files(file_names.size());
	for (size_t i = 0; i < files.size(); i++) {
		files[i] = fopen(file_names[i].c_str(), "r");
		if (files[i] == nullptr) {
			fprintf(stderr, "%s: ", file_names[i].c_str());
			error("No such file");
		}
	}

	vector <int> file_values(files.size());
	vector <int> changed;
	vector <bool> file_end(files.size(), false);
	vector <int> file_lines(files.size(), 0);
	int open_files = files.size();

	int mini;
	for (size_t i = 0; i < files.size(); i++) {
		if (fscanf(files[i], "%d", &file_values[i]) <= 0) {
			fprintf(stderr, "%s line %d : ", file_names[i].c_str(), file_lines[i]);
			error("Error reading file");
		} else {
// 			printf("plik %s\n",  file_names[i].c_str());
			file_lines[i]++;
			changed.push_back(i);
		}
	}
	mini = file_values[0];
	for (size_t i = 0; i < files.size(); i++) {
		mini = file_values[i] < mini ? file_values[i] : mini;
	}

	t->update(mini, changed, file_values);
	while (open_files > 0) {
		for (size_t i = 0; i < files.size(); i++) {
			if (!file_end[i]) {
				mini = file_values[i];
				break;
			}
		}
		for (size_t i = 0; i < files.size(); i++) {
			if (!file_end[i])
				mini = file_values[i] < mini ? file_values[i] : mini;
		}
		vector <int> minima;
		for (size_t i = 0; i < files.size(); i++) {
			if (file_values[i] == mini)
				minima.push_back(i);
		}
		t->update(mini, minima, file_values);

		t->read(mini);
		changed.clear();
		for (size_t i = 0; i < minima.size(); i++) {
			if (file_end[minima[i]])
				continue;
			int res = fscanf(files[minima[i]], "%d", &file_values[minima[i]]);
			if (res == EOF) {
				file_end[minima[i]] = true;
				open_files--;
			} else if (res < 0) {
				fprintf(stderr, "file %s line %d: ", file_names[i].c_str(), file_lines[i]);
				error("Error reading file");
			} else {
				changed.push_back(minima[i]);
				file_lines[i]++;
			}
		}
		t->update(mini, changed, file_values);
	}

	for (size_t i = 0; i < files.size(); i++) {
		fclose(files[i]);
	}
	delete t;
}
