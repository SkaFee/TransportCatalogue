#include "map_renderer.h"

#include <algorithm>

namespace renderer {
    using namespace std::literals;

	svg::Point SphereProjector::operator()(geo::Coordinates coords) const {
        return {
            (coords.lng - min_lon_) * zoom_coeff_ + padding_,
            (max_lat_ - coords.lat) * zoom_coeff_ + padding_
        };
    }

    MapRenderer::MapRenderer(RenderingSettings&& settings) 
        : settings_(std::move(settings))
    {}
    
    void MapRenderer::SetSettings(RenderingSettings&& settings) {
        settings_ = std::move(settings);
    }

    svg::Document MapRenderer::MakeDocument(std::vector<BusPtr>&& buses, std::vector<std::pair<StopPtr, StopStat>>&& stops) const {
        svg::Document result;

        std::sort(buses.begin(), buses.end(),
            [](const BusPtr& lhs, const BusPtr& rhs) {
                return std::lexicographical_compare(
                    lhs.get()->name.get()->begin(), lhs.get()->name.get()->end(),
                    rhs.get()->name.get()->begin(), rhs.get()->name.get()->end());
            }
        );
        std::sort(stops.begin(), stops.end(),
            [](const std::pair<StopPtr, StopStat>& lhs, const std::pair<StopPtr, StopStat>& rhs) {
                return std::lexicographical_compare(
                    lhs.first.get()->name.get()->begin(), lhs.first.get()->name.get()->end(),
                    rhs.first.get()->name.get()->begin(), rhs.first.get()->name.get()->end());
            }
        );

        const auto coordinates = StopsToCoordinates(stops.begin(), stops.end());
        SphereProjector projector(coordinates.begin(), coordinates.end(), settings_.width, settings_.height, settings_.padding);
        
        AddBusesLines(result, projector, buses);
        AddBusesNames(result, projector, buses);
        AddStopsCircles(result, projector, stops);
        AddStopsNames(result, projector, stops);

        return result;
    }

    void MapRenderer::AddBusesLines(svg::Document& doc, SphereProjector& proj, const std::vector<BusPtr>& buses) const {
        int cnt = 0;
        size_t sz_palette = settings_.color_palette.size();
        for (const BusPtr& bus : buses) {
            if (bus.get()->route.empty()) {
                continue;
            }
            svg::Polyline polyline;
            polyline
                .SetStrokeColor(settings_.color_palette[cnt++ % sz_palette])
                .SetFillColor(svg::Color())
                .SetStrokeWidth(settings_.line_width)
                .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

            cnt = cnt == sz_palette ? 0 : cnt;

            for (const auto& stop_ptr : bus.get()->route) {
                const auto& stop = *stop_ptr.get();
                polyline.AddPoint(proj({ stop.latitude, stop.longitude }));
            }
            doc.Add(std::move(polyline));
        }
    }

    void MapRenderer::AddBusesNames(svg::Document& doc, SphereProjector& proj, const std::vector<BusPtr>& buses) const {
        int cnt = 0;
        size_t sz_palette = settings_.color_palette.size();
        for (const BusPtr& busptr : buses) {
            const auto& bus = *busptr.get();
            if (bus.route.empty()) {
                continue;
            }
            svg::Text text;
            text
                .SetPosition(proj({ bus.route[0].get()->latitude, bus.route[0].get()->longitude }))
                .SetOffset(settings_.bus_label_offset)
                .SetFontSize(settings_.bus_label_font_size)
                .SetFontFamily("Verdana"s)
                .SetFontWeight("bold"s)
                .SetData(*bus.name.get());

            svg::Text text_substrate = text;
            text_substrate
                .SetFillColor(settings_.underlayer_color)
                .SetStrokeColor(settings_.underlayer_color)
                .SetStrokeWidth(settings_.underlayer_width)
                .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

            text
                .SetFillColor(settings_.color_palette[cnt++ % sz_palette]);

            cnt = cnt == sz_palette ? 0 : cnt;

            svg::Text text_substrate_last_stop = text_substrate;
            svg::Text text_last_stop = text;

            doc.Add(std::move(text_substrate));
            doc.Add(std::move(text));

            if (bus.last_stop_name != nullptr) {
                svg::Point p = proj({ bus.last_stop_name.get()->latitude, bus.last_stop_name.get()->longitude });

                text_substrate_last_stop
                    .SetPosition(p);
                doc.Add(std::move(text_substrate_last_stop));

                text_last_stop
                    .SetPosition(p);
                doc.Add(std::move(text_last_stop));
            }
        }
    }

    void MapRenderer::AddStopsCircles(svg::Document& doc, SphereProjector& proj, const std::vector<std::pair<StopPtr, StopStat>>& stops) const {
        for (const auto& [stop_ptr, stop_stat] : stops) {
            const auto& stop = *stop_ptr.get();
            if (stop_stat.passing_buses == nullptr) {
                continue;
            } else if (stop_stat.passing_buses->empty()) {
                continue;
            }
            svg::Circle circle;
            circle
                .SetCenter(proj({ stop.latitude, stop.longitude }))
                .SetRadius(settings_.stop_radius)
                .SetFillColor("white"s);
            doc.Add(std::move(circle));
        }
    }

    void MapRenderer::AddStopsNames(svg::Document& doc, SphereProjector& proj, const std::vector<std::pair<StopPtr, StopStat>>& stops) const {
        for (const auto& [stop_ptr, stop_stat] : stops) {
            const auto& stop = *stop_ptr.get();
            if (stop_stat.passing_buses == nullptr) {
                continue;
            } else if (stop_stat.passing_buses->empty()) {
                continue;
            }

            svg::Text text;
            text
                .SetPosition(proj({ stop.latitude, stop.longitude }))
                .SetOffset(settings_.stop_label_offset)
                .SetFontSize(settings_.stop_label_font_size)
                .SetFontFamily("Verdana"s)
                .SetData(*stop.name.get());

            svg::Text text_substrate = text;
            text_substrate
                .SetFillColor(settings_.underlayer_color)
                .SetStrokeColor(settings_.underlayer_color)
                .SetStrokeWidth(settings_.underlayer_width)
                .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
            doc.Add(std::move(text_substrate));

            text
                .SetFillColor("black"s);
            doc.Add(std::move(text));
        }
    }

}