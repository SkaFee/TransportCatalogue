#pragma once

#include "graph.h"
#include "router.h"

#include <string>
#include <optional>
#include <utility>
#include <unordered_map>
#include <string_view>
#include <vector>
#include <functional>

namespace transport {

	struct EdgeInfo {
		graph::Edge<double> edge;

		std::string_view name;
		int         span_count = -1;
		double      time       = 0.;
	};

	struct RouteItemWait {
		std::string_view stop_name;
		double           time;
	};

	struct RouteItemBus {
		std::string_view bus_name;
		int              span_count;
		double           time;
	};

	struct RouteItem {
		std::optional<RouteItemWait> wait_item;
		std::optional<RouteItemBus>  bus_item;
	};

	struct RouteInfo {
		double total_time = 0.;
		std::vector<RouteItem> items;
	};

	class Router {
	private:
		static constexpr double TO_MINUTES = (3.6 / 60.0);

		using Graph   = graph::DirectedWeightedGraph<double>;
		using RouterG = graph::Router<double>;

		struct Settings {
			double wait_time = 6;
			double velocity  = 40.;
		};

		struct Vertexes {
			size_t start_wait;
			size_t end_wait;
		};

	public:
		Router() = default;
		explicit Router(const size_t graph_size);

		void SetSettings(const double bus_wait_time, const double bus_velocity);
		void AddWaitEdge(const std::string_view stop_name);
		void AddBusEdge(const std::string_view stop_from, const std::string_view stop_to, const std::string_view bus_name, const int span_count, const int dist);
		void AddStop(const std::string_view stop_name);

		void BuildGraph();
		void BuildRouter();

		std::optional<RouteInfo> GetRouteInfo(const std::string_view from, const std::string_view to) const;

	private:
		std::optional<Graph>   graph_  = std::nullopt;
		std::optional<RouterG> router_ = std::nullopt;

		Settings settings_;

		std::unordered_map<std::string_view, Vertexes, std::hash<std::string_view>> stop_to_vertex_id_;
		std::vector<EdgeInfo> edges_;

		void AddEdgesToGraph();
		std::vector<RouteItem> MakeItemsByEdgeIds(const std::vector<graph::EdgeId>& edge_ids) const;
	};
}