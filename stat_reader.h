#pragma once

#include <string>
#include <vector>

std::vector<std::string> InputWithdrawalRequests();
void OutputWithdrawalRequest(
	bool existing, 
	const std::string_view& name, 
	const int stops_on_route, 
	const int unique_stops, 
	const double route_length
);