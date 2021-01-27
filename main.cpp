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