#pragma once

#include <string>
#include <vector>
#include <string_view>
#include <deque>
#include <unordered_map>
#include <tuple>

namespace transport {
	namespace detail {
		enum class Existing {
			NO,
			YES
		};

		struct InformationBus {
			Existing bus_existing = Existing::NO;
			std::string name;
			int stops_on_route;
			int unique_stops;
			int routh_actual_length;
			double curvature;
		};

		struct InformationStop {
			Existing stop_existing = Existing::NO;
			std::string name;
			std::vector<std::string> passing_buses;
		};

		struct Bus {
			Bus() = default;
			Bus(const std::string& bus_name, const std::vector<std::string_view>& bus_route,
				int bus_unique_stops, double bus_route_geographic_length, int bus_route_actual_length);

			std::string name;
			std::vector<std::string_view> route;
			int stops_on_route = 0;
			int unique_stops = 0;
			double route_geographic_length = 0;
			int route_actual_length = 0;
			double curvature = 0;
		};

		struct Stop {
			Stop() = default;
			Stop(const std::string& stop_name, double stop_latitude, double stop_longitude);

			std::string name;
			double latitude = 0;
			double longitude = 0;
		};
	}

	class TransportCatalogue {
	private:
		enum class SeparatorType {
			DASH,
			GREATER_THAN,
			NO_HAVE
		};
		class StopsPairHasher {
		public:
			std::size_t operator()(const std::pair<const detail::Stop*, const detail::Stop*> stops_pair) const;
		private:
			std::hash<const void*> hash_;
		};
	public:
		void AddBus(std::string raw_query);
		void AddStop(std::string raw_query);
		const detail::Bus* SearchBus(const std::string& name) const;
		const detail::Stop* SearchStop(const std::string& name) const;
		detail::InformationBus GetInformationBus(std::string raw_query) const;
		detail::InformationStop GetInformationStop(std::string raw_query) const;
		std::vector<std::string> TakeBusesByStop(std::string name) const;
		void SetDistanceBetweenStops(std::string raw_query);
		int GetActualDistanceBetweenStops(std::string stop1_name, std::string stop2_name) const;
		int GetGeographicDistanceBetweenStops(std::string stop1_name, std::string stop2_name) const;
	private:
		std::deque<detail::Stop> stops_;
		std::deque<detail::Bus> buses_;
		std::unordered_map<std::string, const detail::Bus*, std::hash<std::string>> name_to_bus_;
		std::unordered_map<std::string, const detail::Stop*, std::hash<std::string>> name_to_stop_;
		std::unordered_map<std::string_view, std::vector<const detail::Bus*>, std::hash<std::string_view>> name_to_buses_by_stop_;
		std::unordered_map<std::pair<const detail::Stop*, const detail::Stop*>, int, StopsPairHasher> stops_pair_to_distance_;

		std::tuple<std::string, std::size_t> QueryGetName(const std::string_view str) const;
		std::tuple<std::vector<std::string>, SeparatorType> SplitIntoWordsBySeparator(std::string&& str) const;
		std::tuple<std::vector<std::string_view>, int> WordsToRoute(const std::vector<std::string>& words, SeparatorType separator) const;
		std::tuple<double, int> ComputeRouteLengths(const std::vector<std::string_view>& routh) const;
		std::tuple<std::string, std::string> SplitIntoLengthStop(std::string&& str) const;
	};
}