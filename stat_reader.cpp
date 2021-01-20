#include "stat_reader.h"

#include <string>
#include <iomanip>
#include <iostream>
#include <utility>

using namespace std;

string SplitToWord(const string& str) {
	string words;

	auto pos = str.find_first_of(' ', 0);
	words = str.substr(pos + 1);

	return words;
}

WithdrawalRequests InputWithdrawalRequests() {
	WithdrawalRequests result;

	int quantity;
	cin >> quantity;
	string tmp;
	getline(cin, tmp);
	vector<string> requests;
	requests.reserve(quantity);

	for (int i = 0; i < quantity; ++i) {
		string line;
		getline(cin, line);
		auto word = SplitToWord(line);
		result.requests.push_back(move(word));
	}

	return result;
}

void OutputWithdrawalRequest(int existing, const std::string_view& name, const int stops_on_route, const int unique_stops, const double routh_length) {
	using namespace literals;
	if (existing) {
		cout
			<< "Bus "s << name << ": "s
			<< stops_on_route << " stops on route, "s
			<< unique_stops << " unique stops, "s
			<< setprecision(6)
			<< routh_length << " route length"
			<< endl;
	} else {
		cout << "Bus "s << name << ": not found"s << endl;
	}
}