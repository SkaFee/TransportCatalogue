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

void TransportCatalogue::AddBus(string raw_query) {
	const auto [words, separator] = SplitIntoWords(move(raw_query));
	auto [route, unique_stops] = WordsToRoute(words, separator);
	Bus new_bus(words[0], route, route.size(), unique_stops, ComputeRouteLength(route));

	buses_.push_back(move(new_bus));
	name_to_bus_[words[0]] = &buses_.back();

	for (const auto& stop : route) {
		name_to_buses_by_stop_[stop].push_back(&buses_.back());
	}
}

void TransportCatalogue::AddStop(string raw_query) {
	auto [words, _] = SplitIntoWords(move(raw_query));
	Stop new_stop(words[0], stod(words[1]), stod(words[2]));

	stops_.push_back(move(new_stop));
	name_to_stop_[words[0]] = &stops_.back();
}

const Bus* TransportCatalogue::SearchBus(const string& name) const {
	return (name_to_bus_.count(name) ? name_to_bus_.at(name) : nullptr);
}

const Stop* TransportCatalogue::SearchStop(const string& name) const {
	return (name_to_stop_.count(name) ? name_to_stop_.at(name) : nullptr);
}

InformationBus TransportCatalogue::GetInformationBus(string raw_query) const {
	InformationBus result;

	const auto [name, _] = QueryGetName(move(raw_query));
	auto const bus = SearchBus(name);
	if (bus == nullptr) {
		result.name = name;
		return result;
	}
	
	return {
		Existing::YES,
		name,
		bus->stops_on_route,
		bus->unique_stops,
		bus->route_length
	};
}

InformationStop TransportCatalogue::GetInformationStop(string raw_query) const {
	InformationStop result;

	auto [name, _] = QueryGetName(move(raw_query));
	auto const stop = SearchStop(name);
	if (stop == nullptr) {
		result.name = name;
		return result;
	}

	return {
		Existing::YES,
		name,
		TakeBusesByStop(move(name))
	};
}

vector<string> TransportCatalogue::TakeBusesByStop(string name) const {
	set<string> tmp;
	const string_view name_sv = SearchStop(name)->name;
	if (name_to_buses_by_stop_.count(name_sv)) {
		for (auto const bus : name_to_buses_by_stop_.at(name_sv)) {
			tmp.insert(bus->name);
		}
	}

	return { tmp.begin(), tmp.end() };
}

tuple<string, size_t> TransportCatalogue::QueryGetName(const string_view str) const {
	auto pos = str.find_first_of(' ', 0) + 1;
	auto new_pos = str.find_first_of(':', pos);
	if (new_pos == str.npos) {
		return tuple<string, size_t>(str.substr(pos), new_pos);
	}
	auto name = str.substr(pos, new_pos - pos);
	return tuple<string, size_t>(name, ++new_pos);
}

tuple<vector<string>, TransportCatalogue::SeparatorType> TransportCatalogue::SplitIntoWords(string&& str) const {
	vector<string> words;
	SeparatorType sep_type;

	auto [name, pos_start] = QueryGetName(str);
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

tuple<vector<string_view>, int> TransportCatalogue::WordsToRoute(const vector<string>& words, SeparatorType separator) const {
	vector<string_view> result;
	set<string_view> stops_unique_names;
	result.reserve(words.size() - 1);

	for (size_t i = 1; i < words.size(); ++i) {
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

double TransportCatalogue::ComputeRouteLength(const std::vector<std::string_view>& route) const {
	double result = 0;

	auto prev_stop = SearchStop(static_cast<string>(route[0]));
	size_t route_sz = route.size();
	for (size_t i = 1; i < route_sz; ++i) {
		const auto stop = SearchStop(static_cast<string>(route[i]));
		result +=
			ComputeDistance(
				{ prev_stop->latitude	, prev_stop->longitude	},
				{ stop->latitude		, stop->longitude		}
		);
		prev_stop = stop;
	}

	return result;
}