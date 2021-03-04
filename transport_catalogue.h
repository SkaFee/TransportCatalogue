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
		using StopsPair = std::pair<domain::StopPtr, domain::StopPtr>;

		class StopsPairHasher {
		public:
			std::size_t operator()(const StopsPair& stops_pair) const;

		private:
			std::hash<const void*> hash_;
		};

	public:
		void AddBus(domain::Bus&& bus);
		void AddStop(domain::Stop&& stop);
		void SetDistanceBetweenStops(const std::string_view first, const std::string_view second, int distance);

		domain::BusPtr  SearchBus(const std::string_view name)  const;
		domain::StopPtr SearchStop(const std::string_view name) const;

		std::optional<int>                        GetActualDistanceBetweenStops(const std::string_view stop1_name, const std::string_view stop2_name)       const;
		std::optional<double>                     GetGeographicDistanceBetweenStops(const std::string_view stop1_name, const std::string_view stop2_name)   const;
		const std::unordered_set<domain::BusPtr>* GetPassingBusesByStop(domain::StopPtr stop)                                                               const;
		const std::vector<domain::BusPtr>         GetBusesInVector()                                                                                        const;
		const std::vector<domain::StopPtr>        GetStopsInVector()                                                                                        const;

	private:
		std::deque<std::shared_ptr<domain::Stop>> stops_;
		std::deque<std::shared_ptr<domain::Bus>>  buses_;

		std::unordered_map<std::string_view, domain::BusPtr, std::hash<std::string_view>>  name_to_bus_;
		std::unordered_map<std::string_view, domain::StopPtr, std::hash<std::string_view>> name_to_stop_;

		std::unordered_map<StopsPair, int, StopsPairHasher>                                                 stops_pair_to_distance_;
		std::unordered_map<domain::StopPtr, std::unordered_set<domain::BusPtr>, std::hash<domain::StopPtr>> stop_to_passing_buses_;

		void AddToStopPassingBuses(const std::vector<domain::StopPtr>& stops, const std::string_view bus_name);
	};
}