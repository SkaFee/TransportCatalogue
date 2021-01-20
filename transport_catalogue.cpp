#include "geo.h"
#include "transport_catalogue.h"

#include <utility>
#include <string>
#include <vector>
#include <string_view>
#include <deque>
#include <map>
#include <set>

using namespace std;

void TransportCatalogue::AddBus(const string& name, const vector<string>& route) {
	auto [it, _] = unique_buses_names_.insert(name);
	Bus new_bus(*it);
	for (const auto& elem : route) {
		const Stop* const stop = SearchStop(elem);
		new_bus.route.push_back(stop->name);
	}
	buses_.push_back(move(new_bus));
	name_to_bus_[*it] = &buses_.back();
}

void TransportCatalogue::AddStop(const string& name, double latitude, double longitude) {
	auto [it, _] = unique_stops_names_.insert(name);
	Stop new_stop(*it, latitude, longitude);
	stops_.push_back(move(new_stop));
	name_to_stop_[*it] = &stops_.back();
}

const Bus* TransportCatalogue::SearchBus(const string& name) {
	auto it = unique_buses_names_.find(name);
	if (it == unique_buses_names_.end()) {
		return nullptr;
	}

	return name_to_bus_[*it];
}

const Stop* TransportCatalogue::SearchStop(const string& name) {
	auto it = unique_stops_names_.find(name);
	if (it == unique_stops_names_.end()) {
		return nullptr;
	}

	return name_to_stop_[*it];
}

InformationBus TransportCatalogue::GetInformationBus(const string& name) {
	InformationBus result;

	auto bus = SearchBus(name);
	if (bus == nullptr) {
		result.name = static_cast<string>(name);
		return result;
	}
	auto route = bus->route;
	int stops_on_route = route.size();

	set<string_view> unique_stops_of_bus;
	double routh_length = 0;
	string last_name = static_cast<string>(route[0]);
	Stop last_stop = *SearchStop(last_name);
	unique_stops_of_bus.insert(last_stop.name);
	size_t route_sz = route.size();
	for (size_t i = 1; i < route_sz; ++i) {
		auto stop = *SearchStop(static_cast<string>(route[i]));
		unique_stops_of_bus.insert(stop.name);
		routh_length +=
			ComputeDistance(
				{ last_stop.latitude	, last_stop.longitude },
				{ stop.latitude		, stop.longitude }
			);
		last_stop = stop;
	}

	int unique_stops = unique_stops_of_bus.size();
	return {
		BusExisting::YES,
		static_cast<string>(name),
		stops_on_route,
		unique_stops,
		routh_length
	};
}