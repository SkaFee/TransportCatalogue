#include "transport_catalogue.h"
#include "input_reader.h"
#include "stat_reader.h"

#include <algorithm>
#include <cstdlib>

using namespace std;

int main() {
	transport::TransportCatalogue transport_catalogue;
	auto filling_requests = filling_requests::Input();
	sort( filling_requests.begin()
		, filling_requests.end()
		, [](const auto& lhs, const auto& rhs) {
			return lhs[0] > rhs[0];
		}
	);

	for (auto& request : filling_requests) {
		if (request[0] == 'S') {
			transport_catalogue.AddStop(request);
		} else {
			break;
		}
	}
	for (auto& request : filling_requests) {
		if (request[0] == 'S') {
			transport_catalogue.SetDistanceBetweenStops(move(request));
		} else {
			transport_catalogue.AddBus(move(request));
		}
	}

	auto withdrawal_requests = withdrawal_requests::Input();
	for (const auto& request : withdrawal_requests) {
		if (request[0] == 'S') {
			const auto information_stop = transport_catalogue.GetInformationStop(request);
			withdrawal_requests::OutputStopInformation(
				bool(information_stop.stop_existing), 
				information_stop.name, 
				information_stop.passing_buses
			);
		} else {
			const auto information_bus = transport_catalogue.GetInformationBus(request);
			withdrawal_requests::OutputBusInformation(
				bool(information_bus.bus_existing), 
				information_bus.name, 
				information_bus.stops_on_route, 
				information_bus.unique_stops, 
				information_bus.routh_actual_length, 
				information_bus.curvature
			);
		}
	}
}

/*
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
13
Stop Tolstopaltsevo: 55.611087, 37.20829, 3900m to Marushkino
Stop Marushkino: 55.595884, 37.209755, 9900m to Rasskazovka, 100m to Marushkino
Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye
Bus 750: Tolstopaltsevo - Marushkino - Marushkino - Rasskazovka
Stop Rasskazovka: 55.632761, 37.333324, 9500m to Marushkino
Stop Biryulyovo Zapadnoye: 55.574371, 37.6517, 7500m to Rossoshanskaya ulitsa, 1800m to Biryusinka, 2400m to Universam
Stop Biryusinka: 55.581065, 37.64839, 750m to Universam
Stop Universam: 55.587655, 37.645687, 5600m to Rossoshanskaya ulitsa, 900m to Biryulyovo Tovarnaya
Stop Biryulyovo Tovarnaya: 55.592028, 37.653656, 1300m to Biryulyovo Passazhirskaya
Stop Biryulyovo Passazhirskaya: 55.580999, 37.659164, 1200m to Biryulyovo Zapadnoye
Bus 828: Biryulyovo Zapadnoye > Universam > Rossoshanskaya ulitsa > Biryulyovo Zapadnoye
Stop Rossoshanskaya ulitsa: 55.595579, 37.605757
Stop Prazhskaya: 55.611678, 37.603831
6
Bus 256
Bus 750
Bus 751
Stop Samara
Stop Prazhskaya
Stop Biryulyovo Zapadnoye
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
4
Stop A: -55.611087, 37.20829, 200000m to B
Stop B: 55.611087, 37.20829, 100000m to A
Bus 1: A - B
Bus 2: A > B
4
Stop A
Stop B
Bus 1
Bus 2
*/