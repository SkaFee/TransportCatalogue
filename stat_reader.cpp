#include "stat_reader.h"

#include <string>
#include <iomanip>
#include <iostream>

using namespace std;

vector<string> InputWithdrawalRequests() {
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

void OutputWithdrawalRequest(
	bool existing, 
	const std::string_view& name,
	const int stops_on_route, 
	const int unique_stops, 
	const double routh_length
) {
	if (existing) {
		cout
			<< "Bus "s << name << ": "s
			<< stops_on_route << " stops on route, "s
			<< unique_stops << " unique stops, "s
			<< setprecision(6)
			<< routh_length << " route length"s
			<< endl;
	} else {
		cout << "Bus "s << name << ": not found"s << endl;
	}
}