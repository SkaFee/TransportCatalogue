#include "input_reader.h"

#include <vector>
#include <string>
#include <iostream>

using namespace std;

namespace filling_requests {
	vector<string> Input() {
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
}