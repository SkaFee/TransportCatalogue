#pragma once

#include <string>
#include <vector>
#include <string_view>
#include <deque>
#include <map>
#include <set>

enum class BusExisting {
	NO,
	YES
};

struct InformationBus {
	BusExisting bus_existing = BusExisting::NO;
	std::string name;
	int stops_on_route;
	int unique_stops;
	double routh_length;
};

struct Bus {
	Bus() = default;
	Bus(const std::string_view& name_sv) 
		: name(name_sv) {
	}

	std::string_view name;
	std::vector<std::string_view> route;
};

struct Stop {
	Stop() = default;
	Stop(const std::string_view& name_sv, double latitude, double longitude) 
		: name(name_sv)
		, latitude(latitude)
		, longitude(longitude) {
	}

	std::string_view name;
	double latitude;
	double longitude;
};

class TransportCatalogue {
public:
	void AddBus(const std::string& name, const std::vector<std::string>& route);
	void AddStop(const std::string& name, double latitude, double longitude);
	const Bus* SearchBus(const std::string& name);
	const Stop* SearchStop(const std::string& name);
	InformationBus GetInformationBus(const std::string& name);

private:
	std::deque<Stop> stops_;
	std::deque<Bus> buses_;
	std::map<std::string_view, const Bus*> name_to_bus_;
	std::map<std::string_view, const Stop*> name_to_stop_;
	std::set<std::string, std::less<>> unique_stops_names_;
	std::set<std::string, std::less<>> unique_buses_names_;
};