#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <unordered_set>
#include <memory>
#include <optional>

struct Bus;
using BusPtr = std::shared_ptr<Bus>;

struct Stop;
using StopPtr = std::shared_ptr<Stop>;

struct Bus final {
	Bus(std::string&& f_name, std::vector<StopPtr>&& f_route, int f_unique, int f_actual, double f_geogr, StopPtr last_stop = nullptr);

	Bus& operator=(const Bus& bus) = default;

	std::shared_ptr<std::string> name;
	std::vector<StopPtr> route;
	int	unique_stops = 0;
	int	route_actual_length	= 0;
	double route_geographic_length = 0;
	StopPtr last_stop_name;
};

struct Stop final {
	Stop(std::string&& f_name, double f_lat, double f_long);

	double GetGeographicDistanceTo(StopPtr stop_to) const;

	std::shared_ptr<std::string> name;
	double latitude	= 0;
	double longitude = 0;
};

struct BusStat final {
	std::string_view name;
	int	stops_on_route = 0;
	int	unique_stops = 0;
	int	routh_actual_length = 0;
	double curvature = 0.0;
};

struct StopStat final {
	std::string_view name;
	const std::unordered_set<BusPtr>* passing_buses;
};