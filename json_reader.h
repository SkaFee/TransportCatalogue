#pragma once

#include "json.h"
#include "transport_catalogue.h"
#include "request_handler.h"

#include <iostream>
#include <tuple>
#include <string_view>
#include <vector>
#include <string>
#include <optional>

namespace json_reader {

	class JsonReader final {
	public:
		JsonReader(request_handler::RequestHandler& req_handler);

		void Start(std::istream& input, std::ostream& out);

	private:
		request_handler::RequestHandler& rh_;

		void FillTransportCatalogue(const json::Dict& dict);
		const json::Dict&	FillStop(const json::Dict& stop_req);
		void				FillBus(const json::Dict& bus_req);

		renderer::RenderingSettings ReadRenderingSettings(const json::Dict& dict);
		double						GetDoubleFromNode(const json::Node& node)	const;
		std::vector<svg::Color>		GetColorsFromArray(const json::Array& arr)	const;
		svg::Color					GetColor(const json::Node& node)			const;

		void		AnswerStatRequests(const json::Dict& dict, std::ostream& out)			const;
		json::Node	OutStopStat(const std::optional<domain::StopStat> stop_stat, int id)	const;
		json::Node	OutBusStat(const std::optional<domain::BusStat> bus_stat, int id)		const;
		json::Node	OutMapReq(int id)														const;


		std::tuple<std::vector<std::string_view>, int, domain::StopPtr> WordsToRoute(const json::Array& words, bool is_roundtrip) const;
	};
}