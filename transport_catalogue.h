#pragma once

#include "domain.h"

#include <string>
#include <vector>
#include <string_view>
#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <tuple>
#include <optional>
#include <memory>

namespace transport {

	class TransportCatalogue {
	private:
		using StopsPair = std::pair<StopPtr, StopPtr>;

		class StopsPairHasher {
		public:
			std::size_t operator()(const StopsPair& stops_pair) const;
		private:
			std::hash<const void*> hash_;
		};

	public:
		void AddBus(Bus&& bus);
		void AddStop(Stop&& stop);
		void SetDistanceBetweenStops(const std::string_view first, const std::string_view second, int distance);

		BusPtr  SearchBus(const std::string_view name)	const;
		StopPtr	SearchStop(const std::string_view name) const;

		std::optional<int>				  GetActualDistanceBetweenStops(const std::string_view stop1_name, const std::string_view stop2_name)		const;
		std::optional<double>			  GetGeographicDistanceBetweenStops(const std::string_view stop1_name, const std::string_view stop2_name)	const;
		const std::unordered_set<BusPtr>* GetPassingBusesByStop(StopPtr stop)																		const;
		const std::vector<BusPtr>		  GetBusesInVector()																						const;
		const std::vector<StopPtr>		  GetStopsInVector()																						const;
	private:
		std::deque<std::shared_ptr<Stop>> stops_;
		std::deque<std::shared_ptr<Bus>>  buses_;

		std::unordered_map<std::string_view, BusPtr, std::hash<std::string_view>>  name_to_bus_;
		std::unordered_map<std::string_view, StopPtr, std::hash<std::string_view>> name_to_stop_;

		std::unordered_map<StopsPair, int, StopsPairHasher> stops_pair_to_distance_;
		std::unordered_map<StopPtr, std::unordered_set<BusPtr>, std::hash<StopPtr>> stop_to_passing_buses_;

		void AddToStopPassingBuses(const std::vector<StopPtr>& stops, const std::string_view bus_name);
	};
}