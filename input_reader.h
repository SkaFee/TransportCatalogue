#pragma once

#include <string>
#include <vector>
#include <tuple>

enum class RequestType {
	BUS,
	STOP
};

enum class SeparatorType {
	DASH,
	GREATER_THAN,
	NO_HAVE
};

struct Request {
	std::vector<std::string> words;
	std::string name;
	RequestType query_type;
	SeparatorType separator_type = SeparatorType::NO_HAVE;
};

struct FillingRequests {
	std::vector<Request> requests;
};

size_t SplitGeneralPart(const std::string& str, std::vector<std::string>& words);
std::tuple<std::vector<std::string>, SeparatorType> SplitToWords(const std::string& str);
Request WordsToRequest(std::vector<std::string>&& words, SeparatorType sep);
FillingRequests InputFillingRequests();