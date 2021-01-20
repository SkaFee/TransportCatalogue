#include "transport_catalogue.h"
#include "input_reader.h"
#include "stat_reader.h"

#include <algorithm>
#include <cstdlib>

using namespace std;

int main() {
	TransportCatalogue transport_catalogue;
	auto filling_requests = InputFillingRequests();
	sort( filling_requests.begin()
		, filling_requests.end()
		, [](const auto& lhs, const auto& rhs) {
			return lhs[0] > rhs[0];
		}
	);

	for (const auto& request : filling_requests) {
		if (request[0] == 'S') {
			transport_catalogue.AddStop(request);
		} else {
			transport_catalogue.AddBus(request);
		}
	}

	auto withdrawal_requests = InputWithdrawalRequests();
	for (const auto& request : withdrawal_requests) {
		auto information_bus = transport_catalogue.GetInformationBus(request);
		OutputWithdrawalRequest(static_cast<int>(information_bus.bus_existing), information_bus.name, information_bus.stops_on_route, information_bus.unique_stops, information_bus.routh_length);
	}
}

/*
11
Stop Rasskazovka: 55.632761, 37.333324
Stop Biryulyovo Zapadnoye: 55.574371, 37.651700
Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye
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