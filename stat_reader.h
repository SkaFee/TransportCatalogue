#pragma once

#include <string>
#include <vector>

std::vector<std::string> InputWithdrawalRequests();
void OutputWithdrawalRequestBus(
	bool existing, 
	const std::string& name, 
	const int stops_on_route, 
	const int unique_stops, 
	const double route_length
);
void OutputWithdrawalRequestStop(
	bool existing,
	const std::string& name,
	const std::vector<std::string>& passing_buses
);