#pragma once

#include <string>
#include <vector>
#include <string_view>
#include <deque>
#include <unordered_map>
#include <tuple>

enum class Existing {
	NO,
	YES
};

struct InformationBus {
	Existing bus_existing = Existing::NO;
	std::string name;
	int stops_on_route;
	int unique_stops;
	double routh_length;
};

struct InformationStop {
	Existing stop_existing = Existing::NO;
	std::string name;
	std::vector<std::string> passing_buses;
};

struct Bus {
	Bus() = default;
	Bus(const std::string& bus_name, const std::vector<std::string_view>& bus_route, 
		int bus_stops_on_route, int bus_unique_stops, double bus_route_length)
		: name(bus_name)
		, route(bus_route)
		, stops_on_route(bus_stops_on_route)
		, unique_stops(bus_unique_stops)
		, route_length(bus_route_length) {
	}

	std::string name;
	std::vector<std::string_view> route;
	int stops_on_route = 0;
	int unique_stops = 0;
	double route_length = 0;
};

struct Stop {
	Stop() = default;
	Stop(const std::string& stop_name, double stop_latitude, double stop_longitude) 
		: name(stop_name)
		, latitude(stop_latitude)
		, longitude(stop_longitude) {
	}

	std::string name;
	double latitude = 0;
	double longitude = 0;
};

class TransportCatalogue {
public:
	void AddBus(std::string raw_query);
	void AddStop(std::string raw_query);
	const Bus* SearchBus(const std::string& name) const;
	const Stop* SearchStop(const std::string& name) const;
	InformationBus GetInformationBus(std::string raw_query) const;
	InformationStop GetInformationStop(std::string raw_query) const;
	std::vector<std::string> TakeBusesByStop(std::string name) const;

private:
	std::deque<Stop> stops_;
	std::deque<Bus> buses_;
	std::unordered_map<std::string, const Bus*, std::hash<std::string>> name_to_bus_;
	std::unordered_map<std::string, const Stop*, std::hash<std::string>> name_to_stop_;
	std::unordered_map<std::string_view, std::vector<const Bus*>, std::hash<std::string_view>> name_to_buses_by_stop_;

	enum class SeparatorType {
		DASH,
		GREATER_THAN,
		NO_HAVE
	};

	std::tuple<std::string, size_t> QueryGetName(const std::string_view str) const;
	std::tuple<std::vector<std::string>, SeparatorType> SplitIntoWords(std::string&& str) const;
	std::tuple<std::vector<std::string_view>, int> WordsToRoute(const std::vector<std::string>& words, SeparatorType separator) const;
	double ComputeRouteLength(const std::vector<std::string_view>& routh) const;
};