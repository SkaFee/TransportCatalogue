#pragma once

#include <string>
#include <vector>

namespace withdrawal_requests {

	std::vector<std::string> Input();

	void OutputBusInformation(
		bool existing,
		const std::string& name,
		const int stops_on_route,
		const int unique_stops,
		const int route_actual_length,
		const double curvature
	);

	void OutputStopInformation(
		bool existing,
		const std::string& name,
		const std::vector<std::string>& passing_buses
	);
}