#include "domain.h"
#include "geo.h"

#include <utility>

using namespace std;

Stop::Stop(std::string&& f_name, double f_lat, double f_long)
	: name(make_shared<string>(move(f_name)))
	, latitude(f_lat)
	, longitude(f_long)
{}

double Stop::GetGeographicDistanceTo(StopPtr stop_to) const {
	return geo::ComputeDistance(
		{ latitude, longitude },
		{ stop_to.get()->latitude, stop_to.get()->longitude }
	);
}

Bus::Bus(string&& f_name, vector<StopPtr>&& f_route, int f_unique, int f_actual, double f_geogr, StopPtr last_stop)
	: name(make_shared<string>(move(f_name)))
	, route(vector<StopPtr>(move(f_route)))
	, unique_stops(f_unique)
	, route_actual_length(f_actual)
	, route_geographic_length(f_geogr)
	, last_stop_name(last_stop)
{}
