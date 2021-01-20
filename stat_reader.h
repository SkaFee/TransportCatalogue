#pragma once

#include <string>
#include <vector>

struct WithdrawalRequests {
	std::vector<std::string> requests;
};

std::string SplitToWord(const std::string& str);
WithdrawalRequests InputWithdrawalRequests();
void OutputWithdrawalRequest(
	int existing, 
	const std::string_view& name, 
	const int stops_on_route, 
	const int unique_stops, 
	const double routh_length
);