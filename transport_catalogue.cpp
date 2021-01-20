#include "geo.h"
#include "transport_catalogue.h"

#include <utility>
#include <string>
#include <vector>
#include <string_view>
#include <deque>
#include <unordered_map>
#include <set>
#include <tuple>

using namespace std;

void TransportCatalogue::AddBus(const std::string& raw_query) {
	const auto [words, separator] = SplitIntoWords(raw_query);
	const auto [route, unique_stops] = WordsToRoute(words, separator);
	Bus new_bus(words[0], move(route), route.size(), unique_stops, ComputeRouteLength(route));

	buses_.push_back(move(new_bus));
	name_to_bus_[words[0]] = &buses_.back();
}

void TransportCatalogue::AddStop(const std::string& raw_query) {
	const auto [words, _] = SplitIntoWords(raw_query);
	Stop new_stop(words[0], stod(words[1]), stod(words[2]));

	stops_.push_back(move(new_stop));
	name_to_stop_[words[0]] = &stops_.back();
}

const Bus* TransportCatalogue::SearchBus(const string& name) {
	return (name_to_bus_.count(name) ? name_to_bus_[name] : nullptr);
}

const Stop* TransportCatalogue::SearchStop(const string& name) {
	return (name_to_stop_.count(name) ? name_to_stop_[name] : nullptr);
}

InformationBus TransportCatalogue::GetInformationBus(const string& raw_query) {
	InformationBus result;

	auto [name, _] = QueryGetName(raw_query);
	auto const bus = SearchBus(name);
	if (bus == nullptr) {
		result.name = static_cast<string>(name);
		return result;
	}
	
	return {
		BusExisting::YES,
		static_cast<string>(name),
		bus->stops_on_route,
		bus->unique_stops,
		bus->route_length
	};
}

tuple<string, size_t> TransportCatalogue::QueryGetName(const string_view& str) {
	auto pos = str.find_first_of(' ', 0) + 1;
	auto new_pos = str.find_first_of(':', pos);
	if (new_pos == str.npos) {
		return tuple<string, size_t>(str.substr(pos), new_pos);
	}
	auto name = str.substr(pos, new_pos - pos);
	return tuple<string, size_t>(name, ++new_pos);
}

tuple<vector<string>, TransportCatalogue::SeparatorType> TransportCatalogue::SplitIntoWords(const string_view& str) {
	vector<string> words;
	SeparatorType sep_type;

	const auto [name, pos_start] = QueryGetName(str);
	words.push_back(move(name));

	const size_t str_sz = str.size();
	string word;
	for (size_t i = pos_start; i < str_sz; ++i) {
		if (str[i] == '>' || str[i] == '-') {
			sep_type = (str[i] == '>') ? SeparatorType::GREATER_THAN : SeparatorType::DASH;
			words.push_back(word.substr(1, word.size() - 2));
			word.clear();
		}
		else if (str[i] == ',') {
			words.push_back(word.substr(1, word.size() - 1));
			word.clear();
		}
		else {
			word += str[i];
		}
	}
	words.push_back(word.substr(1, word.size() - 1));

	return tuple<vector<string>, SeparatorType>(words, sep_type);
}

tuple<vector<string_view>, int> TransportCatalogue::WordsToRoute(const vector<string>& words, SeparatorType separator) {
	vector<string_view> result;
	set<string_view> stops_unique_names;
	result.reserve(words.size() - 1);

	for (int i = 1; i < words.size(); ++i) {
		auto const stop = SearchStop(words[i]);
		result.push_back(stop->name);
		stops_unique_names.insert(words[i]);
	}

	if (separator == SeparatorType::DASH) {
		result.reserve(words.size() * 2 - 1);
		for (int i = words.size() - 2; i >= 1; --i) {
			auto const stop = SearchStop(words[i]);
			result.push_back(stop->name);
		}
	}

	return { 
		result, 
		stops_unique_names.size() 
	};
}

double TransportCatalogue::ComputeRouteLength(const std::vector<std::string_view>& route) {
	double result = 0;

	auto prev_stop = SearchStop(static_cast<string>(route[0]));
	size_t route_sz = route.size();
	for (size_t i = 1; i < route_sz; ++i) {
		auto stop = SearchStop(static_cast<string>(route[i]));
		result +=
			ComputeDistance(
				{ prev_stop->latitude	, prev_stop->longitude	},
				{ stop->latitude		, stop->longitude		}
		);
		prev_stop = stop;
	}

	return result;
}
