#include "geo.h"
#include "transport_catalogue.h"

#include <utility>
#include <set>
#include <cmath>

using namespace std;

namespace transport {
	namespace detail {
		Bus::Bus(const string& bus_name, const vector<string_view>& bus_route,
			int bus_unique_stops, double bus_route_geographic_length, int bus_route_actual_length)
			: name(bus_name)
			, route(bus_route)
			, stops_on_route(bus_route.size())
			, unique_stops(bus_unique_stops)
			, route_geographic_length(bus_route_geographic_length)
			, route_actual_length(bus_route_actual_length) {
			curvature = bus_route_actual_length / bus_route_geographic_length;
		}
		Stop::Stop(const string& stop_name, double stop_latitude, double stop_longitude)
			: name(stop_name)
			, latitude(stop_latitude)
			, longitude(stop_longitude) {
		}
	}
	using namespace detail;

	size_t TransportCatalogue::StopsPairHasher::operator()(const pair<const Stop*, const Stop*> stops_pair) const {
		return {
			hash_(stops_pair.first) +
			hash_(stops_pair.second) * 37 * 37
		};
	}

	void TransportCatalogue::AddBus(string raw_query) {
		const auto [words, separator] = SplitIntoWordsBySeparator(move(raw_query));
		auto [route, unique_stops] = WordsToRoute(words, separator);
		auto [geographic, actual] = ComputeRouteLengths(route);
		Bus new_bus(words[0], route, move(unique_stops), move(geographic), move(actual));

		buses_.push_back(move(new_bus));
		name_to_bus_[words[0]] = &buses_.back();

		for (const auto& stop : route) {
			name_to_buses_by_stop_[stop].push_back(&buses_.back());
		}
	}

	void TransportCatalogue::AddStop(string raw_query) {
		auto [words, _] = SplitIntoWordsBySeparator(move(raw_query));
		Stop new_stop(words[0], stod(words[1]), stod(words[2]));

		stops_.push_back(move(new_stop));
		auto const stop = &stops_.back();
		name_to_stop_[words[0]] = stop;

		stops_pair_to_distance_[{ stop, stop }] = 0;
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
			bus->route_actual_length,
			bus->curvature
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
		const string_view name_sv = SearchStop(move(name))->name;
		if (name_to_buses_by_stop_.count(name_sv)) {
			for (auto const bus : name_to_buses_by_stop_.at(name_sv)) {
				tmp.insert(bus->name);
			}
		}

		return { tmp.begin(), tmp.end() };
	}

	void TransportCatalogue::SetDistanceBetweenStops(string raw_query) {
		auto [parts, _] = SplitIntoWordsBySeparator(move(raw_query));
		const Stop* const stop_X = SearchStop(move(parts[0]));
		for (size_t i = 3; i < parts.size(); ++i) {
			auto [raw_distance, name_stop] = SplitIntoLengthStop(move(parts[i]));
			int distance = stoi(raw_distance.substr(0, raw_distance.size() - 1));
			const Stop* const stop_To = SearchStop(move(name_stop));

			stops_pair_to_distance_[{stop_X, stop_To}] = distance;
			pair<const Stop*, const Stop*> tmp_pair = { stop_To, stop_X };

			if (stops_pair_to_distance_.count(tmp_pair) == 0) {
				stops_pair_to_distance_[move(tmp_pair)] = distance;
			}
		}
	}

	int TransportCatalogue::GetActualDistanceBetweenStops(std::string stop1_name, std::string stop2_name) const {
		const Stop* const first_stop = SearchStop(move(stop1_name));
		const Stop* const second_stop = SearchStop(move(stop2_name));
		if (first_stop == nullptr || second_stop == nullptr) {
			return -1;
		}
		const pair<const Stop*, const Stop*> tmp_pair = { first_stop, second_stop };
		return (stops_pair_to_distance_.count(tmp_pair) ? stops_pair_to_distance_.at(tmp_pair) : -1);
	}

	int TransportCatalogue::GetGeographicDistanceBetweenStops(std::string stop1_name, std::string stop2_name) const {
		const Stop* const first_stop = SearchStop(move(stop1_name));
		const Stop* const second_stop = SearchStop(move(stop2_name));
		if (first_stop == nullptr || second_stop == nullptr) {
			return -1;
		}
		const pair<const Stop*, const Stop*> tmp_pair = { first_stop, second_stop };
		return geo::ComputeDistance(
			{ first_stop->latitude, first_stop->longitude },
			{ second_stop->latitude, second_stop->longitude }
		);
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

	tuple<vector<string>, TransportCatalogue::SeparatorType> TransportCatalogue::SplitIntoWordsBySeparator(string&& str) const {
		vector<string> words;
		SeparatorType sep_type;

		auto [name, pos_start] = QueryGetName(str);
		words.push_back(move(name));

		const size_t str_sz = str.size();
		string word;
		for (size_t i = pos_start; i < str_sz; ++i) {
			if (str[0] == 'B' && (str[i] == '>' || str[i] == '-')) {
				sep_type = (str[i] == '>') ? SeparatorType::GREATER_THAN : SeparatorType::DASH;
				words.push_back(word.substr(1, word.size() - 2));
				word.clear();
			} else if (str[0] == 'S' && str[i] == ',') {
				words.push_back(word.substr(1, word.size() - 1));
				word.clear();
			} else {
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

	tuple<double, int> TransportCatalogue::ComputeRouteLengths(const vector<string_view>& route) const {
		double geographic = 0;
		int actual = 0;

		auto prev_stop = SearchStop(static_cast<string>(route[0]));
		size_t route_sz = route.size();
		for (size_t i = 1; i < route_sz; ++i) {
			auto const stop = SearchStop(static_cast<string>(route[i]));
			geographic +=
				geo::ComputeDistance(
					{ prev_stop->latitude	, prev_stop->longitude },
					{ stop->latitude		, stop->longitude }
			);

			pair<const Stop*, const Stop*> tmp_pair = { prev_stop, stop };
			actual += stops_pair_to_distance_.at(move(tmp_pair));

			prev_stop = stop;
		}

		return tuple<double, int>(geographic, actual);
	}

	tuple<string, string> TransportCatalogue::SplitIntoLengthStop(string&& str) const {
		const auto pos = str.find_first_of(' ');
		string length = str.substr(0, pos);
		string name_stop = str.substr(pos + 4);

		return { length,name_stop };
	}
}