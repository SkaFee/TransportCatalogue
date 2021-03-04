#include "stat_reader.h"

#include <iomanip>
#include <iostream>

namespace withdrawal_requests {

	std::vector<std::string> Input() {
		using namespace std;
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

	void OutputBusInformation(
		bool existing,
		const std::string& name,
		const int stops_on_route,
		const int unique_stops,
		const int routh_length,
		const double curvature) 
	{
		using namespace std;
		if (existing) {
			cout
				<< "Bus "s << name << ": "s
				<< stops_on_route << " stops on route, "s
				<< unique_stops << " unique stops, "s
				<< routh_length << " route length, "s
				<< setprecision(6)
				<< curvature << " curvature"s
				<< endl;
		} else {
			cout << "Bus "s << name << ": not found"s << endl;
		}
	}

	void OutputStopInformation(
		bool existing,
		const std::string& name,
		const std::vector<std::string>& passing_buses)
	{
		using namespace std;
		if (existing) {
			if (passing_buses.size()) {
				cout << "Stop "s << name << ": buses"s;
				for (const auto& bus : passing_buses) {
					cout << " "s << bus;
				}
				cout << endl;
			} else {
				cout << "Stop "s << name << ": no buses"s << endl;
			}
		} else {
			cout << "Stop "s << name << ": not found"s << endl;
		}
	}
}
