#include "json_reader.h"

#include <utility>
#include <unordered_set>
#include <set>
#include <algorithm>
#include <sstream>
#include <cassert>

using namespace std;

namespace json_reader {

	JsonReader::JsonReader()
		: db_(transport::TransportCatalogue())
		, rh_(db_)
	{}

	JsonReader::JsonReader(const transport::TransportCatalogue& db, const request_handler::RequestHandler& req_handler)
		: db_(db)
		, rh_(req_handler)
	{}

	void JsonReader::Start(istream& input, ostream& out) {
		const json::Document doc = json::Load(input);
		const json::Node& node = doc.GetRoot();
		const json::Dict& dict = node.AsMap();
		
		if (dict.count("base_requests"s)) {
			FillTransportCatalogue(dict);
		}

		renderer::RenderingSettings settings;
		if (dict.count("render_settings"s)) {
			settings = ReadRenderingSettings(dict);
		}
		if (dict.count("stat_requests"s)) {
			AnswerStatRequests(dict, out, move(settings));
		}
	}

	void JsonReader::FillTransportCatalogue(const json::Dict& dict) {
		const json::Array& base_requests = dict.at("base_requests"s).AsArray();
		vector<pair<const string&, const json::Dict&>> stops_road_distances;

		for (const auto& req_node : base_requests) {
			const json::Dict& req = req_node.AsMap();
			if (req.at("type"s).AsString() == "Stop"s) {
				stops_road_distances.push_back({ req.at("name"s).AsString(), FillStop(req) });
			} 
		}

		for (const auto& [stop_name, dict] : stops_road_distances) {
			for (const auto& [stop_name_to, distance] : dict) {
				db_.SetDistanceBetweenStops(stop_name, stop_name_to, distance.AsInt());
			}
		}

		for (const auto& req_node : base_requests) {
			const json::Dict& req = req_node.AsMap();
			if (req.at("type"s).AsString() == "Bus"s) {
				FillBus(req);
			}
		}
	}

	const json::Dict& JsonReader::FillStop(const json::Dict& stop_req) {
		const auto& node_latitude = stop_req.at("latitude"s);
		double latitude = node_latitude.IsPureDouble() ? node_latitude.AsDouble() : node_latitude.AsInt();
		const auto& node_longitude = stop_req.at("longitude"s);
		double longitude = node_longitude.IsPureDouble() ? node_longitude.AsDouble() : node_longitude.AsInt();
		Stop stop(move(string(stop_req.at("name"s).AsString())), latitude, longitude);
		db_.AddStop(move(stop));
		return stop_req.at("road_distances"s).AsMap();
	}

	void JsonReader::FillBus(const json::Dict& bus_req) {
		auto [route, unique_stops_num, last_stop ] = WordsToRoute(bus_req.at("stops"s).AsArray(), bus_req.at("is_roundtrip"s).AsBool());
		const auto [geographic, actual] = rh_.ComputeRouteLengths(route);
		if (last_stop.get() == db_.SearchStop(route.front()).get()) {
			Bus bus(move(string(bus_req.at("name"s).AsString())), rh_.StopsToStopPtr(move(route)), unique_stops_num, actual, geographic);
			db_.AddBus(move(bus));
		} else {
			Bus bus(move(string(bus_req.at("name"s).AsString())), rh_.StopsToStopPtr(move(route)), unique_stops_num, actual, geographic, last_stop);
			db_.AddBus(move(bus));
		}
	}

	renderer::RenderingSettings JsonReader::ReadRenderingSettings(const json::Dict& dict) {
		renderer::RenderingSettings settings;

		const json::Dict& dict_deeper = dict.at("render_settings"s).AsMap();

		const json::Node& node_width = dict_deeper.at("width"s);
		settings.width = GetDoubleFromNode(node_width);

		const json::Node& node_height = dict_deeper.at("height"s);
		settings.height = GetDoubleFromNode(node_height);

		const json::Node& node_padding = dict_deeper.at("padding"s);
		settings.padding = GetDoubleFromNode(node_padding);

		const json::Node& node_stop_radius = dict_deeper.at("stop_radius"s);
		settings.stop_radius = GetDoubleFromNode(node_stop_radius);

		const json::Node& node_line_width = dict_deeper.at("line_width"s);
		settings.line_width = GetDoubleFromNode(node_line_width);

		settings.bus_label_font_size = dict_deeper.at("bus_label_font_size"s).AsInt();

		const json::Array& arr_bus_label_offset = dict_deeper.at("bus_label_offset"s).AsArray();
		settings.bus_label_offset.x = GetDoubleFromNode(arr_bus_label_offset[0]);
		settings.bus_label_offset.y = GetDoubleFromNode(arr_bus_label_offset[1]);

		settings.stop_label_font_size = dict_deeper.at("stop_label_font_size"s).AsInt();

		const json::Array& arr_stop_label_offset = dict_deeper.at("stop_label_offset"s).AsArray();
		settings.stop_label_offset.x = GetDoubleFromNode(arr_stop_label_offset[0]);
		settings.stop_label_offset.y = GetDoubleFromNode(arr_stop_label_offset[1]);

		const json::Array& arr_underlayer_color = dict_deeper.at("underlayer_color"s).AsArray();
		settings.underlayer_color = GetColor(arr_underlayer_color);

		const json::Node& node_underlayer_width = dict_deeper.at("underlayer_width"s);
		settings.underlayer_width = GetDoubleFromNode(node_underlayer_width);

		const json::Array& node_color_palette = dict_deeper.at("color_palette"s).AsArray();
		settings.color_palette = GetColorsFromArray(node_color_palette);

		return settings;
	}

	double JsonReader::GetDoubleFromNode(const json::Node& node) const {
		return (node.IsPureDouble() ? node.AsDouble() : node.AsInt());
	}

	std::vector<svg::Color> JsonReader::GetColorsFromArray(const json::Array& arr) const {
		std::vector<svg::Color> result;
		result.reserve(arr.size());

		for (size_t i = 0; i < arr.size(); ++i) {
			const json::Node& node = arr[i];
			result.emplace_back(GetColor(node));
		}

		return result;
	}

	svg::Color JsonReader::GetColor(const json::Node& node) const {
		if (node.IsString()) {
			return node.AsString();
		}
		else if (node.IsArray()) {
			const json::Array& arr = node.AsArray();
			if (arr.size() == 3) {
				return
					svg::Rgb{
						static_cast<uint8_t>(arr[0].AsInt()),
						static_cast<uint8_t>(arr[1].AsInt()),
						static_cast<uint8_t>(arr[2].AsInt())
					};
			}
			else {
				return
					svg::Rgba{
						static_cast<uint8_t>(arr[0].AsInt()),
						static_cast<uint8_t>(arr[1].AsInt()),
						static_cast<uint8_t>(arr[2].AsInt()),
						GetDoubleFromNode(arr[3])
					};
			}
		}

		return {};
	}

	void JsonReader::AnswerStatRequests(const json::Dict& dict, ostream& out, renderer::RenderingSettings&& settings) const {
		json::Array result;
		const auto mr = renderer::MapRenderer(move(settings));
		const json::Array& stat_requests = dict.at("stat_requests"s).AsArray();
		for (const auto& req_node : stat_requests) {
			const json::Dict& req = req_node.AsMap();
			const string& type = req.at("type"s).AsString();
			json::Node node;
			if (type == "Stop"s) {
				node = OutStopStat(rh_.GetStopStat(req.at("name"s).AsString()), req.at("id"s).AsInt());
			} else if (type == "Bus"s) {
				node = OutBusStat(rh_.GetBusStat(req.at("name"s).AsString()), req.at("id"s).AsInt());
			} else {
				node = OutMapReq(mr, req.at("id"s).AsInt());
			}
			result.push_back(move(node));
		}

		json::Print(json::Document(json::Node(result)), out);
	}

	json::Node JsonReader::OutStopStat(const std::optional<StopStat> stop_stat, int id) const {
		if (stop_stat.has_value()) {
			json::Array arr;
			if (stop_stat->passing_buses == nullptr) {
				json::Dict dict = {
					{ "buses"s, json::Node(move(arr)) },
					{ "request_id"s, json::Node(id) }
				};
				return json::Node(move(dict));
			}
			const auto buses = *stop_stat->passing_buses;
			arr.reserve(buses.size());
			vector<BusPtr> tmp(buses.begin(), buses.end());
			sort(tmp.begin(), tmp.end(), 
				[](const BusPtr& lhs, const BusPtr& rhs) {
					return lexicographical_compare(
						lhs.get()->name.get()->begin(), lhs.get()->name.get()->end(),
						rhs.get()->name.get()->begin(), rhs.get()->name.get()->end());
				}
			);
			for (const BusPtr& bus : move(tmp)) {
				arr.push_back(json::Node(*bus.get()->name.get()));
			}
			json::Dict dict = {
				{ "buses"s, json::Node(move(arr)) },
				{ "request_id"s, json::Node(id) }
			};
			return json::Node(move(dict));
		} else {
			json::Dict dict = {
				{ "request_id"s, json::Node(id) },
				{ "error_message"s, json::Node("not found"s) }
			};
			return json::Node(move(dict));
		}
	}

	json::Node JsonReader::OutBusStat(const std::optional<BusStat> bus_stat, int id) const {
		if (bus_stat.has_value()) {
			json::Dict dict = {
				{ "request_id"s, json::Node(id) },
				{ "curvature"s, json::Node(bus_stat->curvature) },
				{ "unique_stop_count"s, json::Node(bus_stat->unique_stops) },
				{ "stop_count"s, json::Node(bus_stat->stops_on_route) },
				{ "route_length"s, json::Node(bus_stat->routh_actual_length) }
			};
			return json::Node(move(dict));
		}
		else {
			json::Dict dict = {
				{ "request_id"s, json::Node(id) },
				{ "error_message"s, json::Node("not found"s) }
			};
			return json::Node(move(dict));
		}
	}

	json::Node JsonReader::OutMapReq(const renderer::MapRenderer& mr, int id) const {
		ostringstream out;
		vector<BusPtr> buses = db_.GetBusesInVector();

		vector<pair<StopPtr, StopStat>> stops;
		for (StopPtr stop : db_.GetStopsInVector()) {
			stops.emplace_back(pair<StopPtr, StopStat>{ stop, *rh_.GetStopStat(*stop.get()->name.get()) });
		}

		svg::Document doc = mr.MakeDocument(move(buses), move(stops));
		doc.Render(out);
		//cout << out.str() << endl;////////////////////////////////////////////////////////////////////////////////////////////////////

		//std::ostringstream svg_string;
		//doc.Render(svg_string);
		//std::string s;
		//s = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"s +
		//	"<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\n"s +
		//	"  <polyline points=\"99.2283,329.5 50,232.18 99.2283,329.5\" fill=\"none\" stroke=\"green\" stroke-width=\"14\" stroke-linecap=\"round\" stroke-linejoin=\"round\"/>\n"s +
		//	"  <polyline points=\"550,190.051 279.22,50 333.61,269.08 550,190.051\" fill=\"none\" stroke=\"rgb(255,160,0)\" stroke-width=\"14\" stroke-linecap=\"round\" stroke-linejoin=\"round\"/>\n"s +
		//	"  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"99.2283\" y=\"329.5\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">114</text>\n"s +
		//	"  <text fill=\"green\" x=\"99.2283\" y=\"329.5\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">114</text>\n"s +
		//	"  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"50\" y=\"232.18\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">114</text>\n"s +
		//	"  <text fill=\"green\" x=\"50\" y=\"232.18\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">114</text>\n"s +
		//	"  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"550\" y=\"190.051\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">14</text>\n"s +
		//	"  <text fill=\"rgb(255,160,0)\" x=\"550\" y=\"190.051\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">14</text>\n"s +
		//	"  <circle cx=\"99.2283\" cy=\"329.5\" r=\"5\" fill=\"white\"/>\n"s +
		//	"  <circle cx=\"50\" cy=\"232.18\" r=\"5\" fill=\"white\"/>\n"s +
		//	"  <circle cx=\"333.61\" cy=\"269.08\" r=\"5\" fill=\"white\"/>\n"s +
		//	"  <circle cx=\"550\" cy=\"190.051\" r=\"5\" fill=\"white\"/>\n"s +
		//	"  <circle cx=\"279.22\" cy=\"50\" r=\"5\" fill=\"white\"/>\n"s +
		//	"  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"99.2283\" y=\"329.5\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\">Морской вокзал</text>\n"s +
		//	"  <text fill=\"black\" x=\"99.2283\" y=\"329.5\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\">Морской вокзал</text>\n"s +
		//	"  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"50\" y=\"232.18\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\">Ривьерский мост</text>\n"s +
		//	"  <text fill=\"black\" x=\"50\" y=\"232.18\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\">Ривьерский мост</text>\n"s +
		//	"  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"333.61\" y=\"269.08\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\">Улица Докучаева</text>\n"s +
		//	"  <text fill=\"black\" x=\"333.61\" y=\"269.08\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\">Улица Докучаева</text>\n"s +
		//	"  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"550\" y=\"190.051\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\">Улица Лизы Чайкиной</text>\n"s +
		//	"  <text fill=\"black\" x=\"550\" y=\"190.051\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\">Улица Лизы Чайкиной</text>\n"s +
		//	"  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"279.22\" y=\"50\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\">Электросети</text>\n"s +
		//	"  <text fill=\"black\" x=\"279.22\" y=\"50\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\">Электросети</text>\n"s +
		//	"</svg>"s;
		//std::istringstream got(svg_string.str());
		//std::istringstream need(s);
		//std::string lhs, rhs;
		//int str_num = 263;
		//while (getline(got, lhs) && getline(need, rhs)) {
		//	std::cerr << str_num++ << std::endl;
		//	std::cout << lhs << std::endl;
		//	std::cout << rhs << std::endl;
		//	assert(lhs == rhs);
		//}
		//assert(svg_string.str() == s);



		json::Dict dict = {
			{ "request_id"s, json::Node(id) },
			{ "map"s, json::Node(out.str())}
		};

		return json::Node(move(dict));
	}

	tuple<vector<string_view>, int, StopPtr> JsonReader::WordsToRoute(const json::Array& words, bool is_roundtrip) const {
		vector<string_view> result;
		unordered_set<string_view, hash<string_view>> stops_unique_names;
		result.reserve(words.size());

		for (size_t i = 0; i < words.size(); ++i) {
			StopPtr stop = db_.SearchStop(words[i].AsString());
			result.push_back(*stop.get()->name.get());
			stops_unique_names.insert(words[i].AsString());
		}
		const StopPtr last_stop = db_.SearchStop(result.back());

		if (!is_roundtrip && words.size() > 1) {
			result.reserve(words.size() * 2);
			for (int i = (int)words.size() - 2; i >= 0; --i) {
				StopPtr stop = db_.SearchStop(words[i].AsString());
				result.push_back(*stop.get()->name.get());
			}
		}

		return {
			move(result),
			(int)stops_unique_names.size(),
			last_stop
		};
	}
}
