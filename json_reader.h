#pragma once

#include "json.h"
#include "transport_catalogue.h"
#include "request_handler.h"
#include "map_renderer.h"

#include <iostream>
#include <tuple>
#include <string_view>
#include <vector>
#include <string>
#include <optional>

namespace json_reader {

	class JsonReader final {
	public:
		JsonReader();
		JsonReader(const transport::TransportCatalogue& db, const request_handler::RequestHandler& req_handler);

		void Start(std::istream& input, std::ostream& out);

	private:
		transport::TransportCatalogue db_;
		request_handler::RequestHandler rh_;

		void FillTransportCatalogue(const json::Dict& dict);
		const json::Dict& FillStop(const json::Dict& stop_req);
		void FillBus(const json::Dict& bus_req);
		renderer::RenderingSettings ReadRenderingSettings(const json::Dict& dict);
		double GetDoubleFromNode(const json::Node& node) const;
		std::vector<svg::Color> GetColorsFromArray(const json::Array& arr) const;
		svg::Color GetColor(const json::Node& node) const;
		void AnswerStatRequests(const json::Dict& dict, std::ostream& out, renderer::RenderingSettings&& settings) const;
		json::Node OutStopStat(const std::optional<StopStat> stop_stat, int id) const;
		json::Node OutBusStat(const std::optional<BusStat> bus_stat, int id) const;
		json::Node OutMapReq(const renderer::MapRenderer& mr, int id) const;


		std::tuple<std::vector<std::string_view>, int, StopPtr> WordsToRoute(const json::Array& words, bool is_roundtrip) const;
	};
}