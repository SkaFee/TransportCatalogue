#include "input_reader.h"

#include <vector>
#include <string>
#include <iostream>

using namespace std;

vector<string> InputFillingRequests() {
	vector<string> result;

	int quantity;
	cin >> quantity;
	string tmp;
	getline(cin, tmp);

	result.reserve(quantity);
	string line;
	for (int i = 0; i < quantity; ++i) {
		getline(cin, line);
		result.push_back(line);
	}

	return result;
}