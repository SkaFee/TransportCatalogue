#include "transport_catalogue.h"
#include "input_reader.h"
#include "stat_reader.h"

#include <algorithm>
#include <cstdlib>

using namespace std;

int main() {
	TransportCatalogue transport_catalogue;
	auto filling_requests = InputFillingRequests();
	sort( filling_requests.requests.begin()
		, filling_requests.requests.end()
		, [](const auto& lhs, const auto& rhs) {
			return lhs.query_type > rhs.query_type;
		}
	);
	const auto& requests = filling_requests.requests;
	for (const auto& request : requests) {
		if (request.query_type == RequestType::STOP) {
			transport_catalogue.AddStop(request.name, stod(request.words[0]), stod(request.words[1]));
		} else {
			if (request.separator_type == SeparatorType::DASH) {
				vector<string> tmp(request.words);
				tmp.reserve(request.words.size() * 2 - 1);
				for (int i = request.words.size() - 2; i >= 0; i--) {
					tmp.push_back(request.words[i]);
				}
				transport_catalogue.AddBus(request.name, tmp);
			} else {
				transport_catalogue.AddBus(request.name, request.words);
			}
		}
	}

	auto withdrawal_requests = InputWithdrawalRequests();
	for (const auto& request : withdrawal_requests.requests) {
		auto information_bus = transport_catalogue.GetInformationBus(request);
		OutputWithdrawalRequest(static_cast<int>(information_bus.bus_existing), information_bus.name, information_bus.stops_on_route, information_bus.unique_stops, information_bus.routh_length);
	}
}

/*
10
Stop Rasskazovka: 55.632761, 37.333324
Stop Biryulyovo Zapadnoye: 55.574371, 37.651700
Stop Biryusinka: 55.581065, 37.648390
Stop Universam: 55.587655, 37.645687
Stop Biryulyovo Tovarnaya: 55.592028, 37.653656
Stop Biryulyovo Passazhirskaya: 55.580999, 37.659164
Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka
Stop Tolstopaltsevo: 55.611087, 37.208290
Stop Marushkino: 55.595884, 37.209755
Bus MyNewBus 256 abcDE: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye
4
Bus 256
Bus 750
Bus 751
Bus MyNewBus 256 abcDE
*/