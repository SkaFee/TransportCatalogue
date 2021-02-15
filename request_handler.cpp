#include "request_handler.h"

#include <unordered_set>
#include <vector>
#include <utility>
#include <functional>

using namespace std;

namespace request_handler {

	RequestHandler::RequestHandler(transport::TransportCatalogue& db)
		: db_(db)
	{}

	void RequestHandler::AddBus(const string_view raw_query) {
		auto [words, separator] = SplitIntoWordsBySeparator(raw_query);
		auto [route, unique_stops] = WordsToRoute(words, separator);
		const auto [geographic, actual] = ComputeRouteLengths(route);
		Bus new_bus(move(words[0]), StopsToStopPtr(move(route)), unique_stops, actual, geographic);

		db_.AddBus(move(new_bus));
	}

	void RequestHandler::AddStop(const string_view raw_query) {
		auto [words, _] = SplitIntoWordsBySeparator(raw_query);
		Stop new_stop(move(words[0]), stod(words[1]), stod(words[2]));

		db_.AddStop(move(new_stop));
	}

	void RequestHandler::SetDistanceBetweenStops(const string_view raw_query) {
		auto [parts, _] = SplitIntoWordsBySeparator(raw_query);
		const auto& stop_X = parts[0];
		for (size_t i = 3; i < parts.size(); ++i) {
			auto [raw_distance, stop_To] = SplitIntoLengthStop(move(parts[i]));
			int distance = stoi(move(raw_distance.substr(0, raw_distance.size() - 1)));

			db_.SetDistanceBetweenStops(stop_X, stop_To, distance);
		}
	}

	optional<BusStat> RequestHandler::GetBusStat(const string_view bus_name) const {
		BusPtr bus = db_.SearchBus(bus_name);
		if (bus == nullptr) {
			return {};
		}

		return optional<BusStat>({
			bus_name,
			static_cast<int>(bus.get()->route.size()),
			bus->unique_stops,
			bus->route_actual_length,
			bus->route_actual_length / bus->route_geographic_length
		});
	}

	optional<StopStat> RequestHandler::GetStopStat(const string_view stop_name) const {
		StopPtr stop = db_.SearchStop(stop_name);
		if (stop == nullptr) {
			return {};
		}

		return optional<StopStat>({
			stop_name,
			GetBusesByStop(stop_name)
		});
	}

	const unordered_set<BusPtr>* RequestHandler::GetBusesByStop(const std::string_view stop_name) const {
		StopPtr stop = db_.SearchStop(stop_name);
		return db_.GetPassingBusesByStop(stop);
	}

	tuple<double, int> RequestHandler::ComputeRouteLengths(const vector<string_view>& route) const {
		double geographic = 0;
		int actual = 0;

		auto prev_stop = &route[0];
		size_t route_sz = route.size();
		for (size_t i = 1; i < route_sz; ++i) {
			const auto cur_stop = &route[i];
			const auto res_geogr = db_.GetGeographicDistanceBetweenStops(*prev_stop, *cur_stop);
			geographic += (res_geogr.has_value()) ? *res_geogr : 0;

			const auto res_actual = db_.GetActualDistanceBetweenStops(*prev_stop, *cur_stop);
			actual += (res_actual.has_value()) ? *res_actual : 0;

			prev_stop = cur_stop;
		}

		return tuple<double, int>(geographic, actual);
	}

	std::vector<StopPtr> RequestHandler::StopsToStopPtr(const std::vector<std::string_view>& stops) const {
		vector<StopPtr> result;
		result.reserve(stops.size());
		for (const auto& stop : stops) {
			result.push_back(db_.SearchStop(stop));
		}
		return result;
	}

	tuple<string, size_t> RequestHandler::QueryGetName(const string_view str) const {
		auto pos = str.find_first_of(' ', 0) + 1;
		auto new_pos = str.find_first_of(':', pos);
		if (new_pos == str.npos) {
			return tuple<string, size_t>(move(str.substr(pos)), new_pos);
		}
		auto name = str.substr(pos, new_pos - pos);

		return tuple<string, size_t>(move(name), ++new_pos);
	}

	tuple<string, string> RequestHandler::SplitIntoLengthStop(string&& str) const {
		const auto pos = str.find_first_of(' ');
		string length = str.substr(0, pos);
		string name_stop = str.substr(pos + 4);

		return { move(length), move(name_stop) };
	}

	tuple<vector<string>, RequestHandler::SeparatorType> RequestHandler::SplitIntoWordsBySeparator(const string_view str) const {
		vector<string> words;
		SeparatorType sep_type;

		auto [name, pos_start] = QueryGetName(str);
		words.push_back(move(name));

		const size_t str_sz = str.size();
		string word;
		for (size_t i = pos_start; i < str_sz; ++i) {
			if (str[0] == 'B' && (str[i] == '>' || str[i] == '-')) {
				sep_type = (str[i] == '>') ? SeparatorType::GREATER_THAN : SeparatorType::DASH;
				words.push_back(move(word.substr(1, word.size() - 2)));
				word.clear();
			} else if (str[0] == 'S' && str[i] == ',') {
				words.push_back(move(word.substr(1, word.size() - 1)));
				word.clear();
			} else {
				word += str[i];
			}
		}
		words.push_back(move(word.substr(1, word.size() - 1)));

		return tuple<vector<string>, SeparatorType>(move(words), sep_type);
	}

	tuple<vector<string_view>, int> RequestHandler::WordsToRoute(const vector<string>& words, SeparatorType separator) const {
		vector<string_view> result;
		unordered_set<string_view, hash<string_view>> stops_unique_names;
		result.reserve(words.size() - 1);

		for (size_t i = 1; i < words.size(); ++i) {
			StopPtr stop = db_.SearchStop(words[i]);
			result.push_back(*stop->name.get());
			stops_unique_names.insert(words[i]);
		}

		if (separator == SeparatorType::DASH) {
			result.reserve(words.size() * 2 - 1);
			for (int i = words.size() - 2; i >= 1; --i) {
				StopPtr stop = db_.SearchStop(words[i]);
				result.push_back(*stop->name.get());
			}
		}

		return {
			move(result),
			(int)stops_unique_names.size()
		};
	}

}
