#include "geo.h"
#include "transport_catalogue.h"

#include <utility>
#include <set>
#include <cmath>

namespace transport {

	using namespace domain;

	size_t TransportCatalogue::StopsPairHasher::operator()(const StopsPair& stops_pair) const {
		return {
			hash_(stops_pair.first.get()) +
			hash_(stops_pair.second.get()) * 37 * 37
		};
	}

	void TransportCatalogue::AddBus(Bus&& bus) {
		buses_.push_back(std::make_shared<Bus>(std::move(bus)));
		const auto bus_ptr = buses_.back().get();
		name_to_bus_[*bus_ptr->name.get()] = buses_.back();

		AddToStopPassingBuses(bus_ptr->route, *bus_ptr->name.get());
	}

	void TransportCatalogue::AddStop(Stop&& stop) {
		if (name_to_stop_.count(*stop.name.get())) {
			return;
		}
		stops_.push_back(std::make_shared<Stop>(std::move(stop)));
		const auto stop_ptr = stops_.back().get();
		name_to_stop_[*stop_ptr->name.get()] = stops_.back();

		stops_pair_to_distance_[{ stops_.back(), stops_.back() }] = 0;
	}

	void TransportCatalogue::SetDistanceBetweenStops(const std::string_view first, const std::string_view second, int distance) {
		StopPtr stop_X = SearchStop(first);
		StopPtr stop_To = SearchStop(second);

		stops_pair_to_distance_[{stop_X, stop_To}] = distance;
		StopsPair tmp_pair = { stop_To, stop_X };

		if (stops_pair_to_distance_.count(tmp_pair) == 0) {
			stops_pair_to_distance_[move(tmp_pair)] = distance;
		}
	}

	BusPtr TransportCatalogue::SearchBus(const std::string_view name) const {
		return (name_to_bus_.count(name) ? name_to_bus_.at(name) : nullptr);
	}

	StopPtr TransportCatalogue::SearchStop(const std::string_view name) const {
		return (name_to_stop_.count(name) ? name_to_stop_.at(name) : nullptr);
	}

	std::optional<int> TransportCatalogue::GetActualDistanceBetweenStops(const std::string_view stop1_name, const std::string_view stop2_name) const {
		StopPtr first_stop	= SearchStop(stop1_name);
		StopPtr second_stop = SearchStop(stop2_name);
		if (first_stop.get() == nullptr || second_stop.get() == nullptr) {
			return {};
		}
		const StopsPair tmp_pair = { first_stop, second_stop };

		return (stops_pair_to_distance_.count(tmp_pair) ? stops_pair_to_distance_.at(tmp_pair) : std::optional<int>{});
	}

	std::optional<double> TransportCatalogue::GetGeographicDistanceBetweenStops(const std::string_view stop1_name, const std::string_view stop2_name) const {
		StopPtr first_stop = SearchStop(stop1_name);
		StopPtr second_stop = SearchStop(stop2_name);
		if (first_stop == nullptr || second_stop == nullptr) {
			return {};
		}

		return first_stop->GetGeographicDistanceTo(second_stop);
	}

	const std::unordered_set<BusPtr>* TransportCatalogue::GetPassingBusesByStop(StopPtr stop) const {
		return ((stop_to_passing_buses_.count(stop)) ? &stop_to_passing_buses_.at(stop) : nullptr);
	}

	const std::vector<BusPtr> TransportCatalogue::GetBusesInVector() const {
		return std::vector<BusPtr>(buses_.begin(), buses_.end());
	}

	const std::vector<StopPtr> TransportCatalogue::GetStopsInVector() const {
		return std::vector<StopPtr>(stops_.begin(), stops_.end());
	}

	void TransportCatalogue::AddToStopPassingBuses(const std::vector<StopPtr>& stops, const std::string_view bus_name) {
		BusPtr bus = SearchBus(bus_name);
		for (size_t i = 0; i < stops.size(); ++i) {
			stop_to_passing_buses_[stops[i]].insert(bus);
		}
	}
}