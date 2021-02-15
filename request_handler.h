#pragma once

/*
 * ����� ����� ���� �� ���������� ��� ����������� �������� � ����, ����������� ������, ������� ��
 * �������� �� �������� �� � transport_catalogue, �� � json reader.
 *
 * � �������� ��������� ��� ���� ���������� ��������� �� ���� ������ ����������� ��������.
 * �� ������ ����������� ��������� �������� ��������, ������� ������� ���.
 *
 * ���� �� ������������� �������, ��� ����� ���� �� ��������� � ���� ����,
 * ������ �������� ��� ������.
 */

 // ����� RequestHandler ������ ���� ������, ����������� �������������� JSON reader-�
 // � ������� ������������ ����������.
 // ��. ������� �������������� �����: https://ru.wikipedia.org/wiki/�����_(������_��������������)�
 
#include "transport_catalogue.h"
#include "map_renderer.h"

#include <optional>
#include <unordered_set>
#include <tuple>
#include <string>
#include <string_view>
#include <memory>

namespace request_handler {

    class RequestHandler {
    private:
        enum class SeparatorType {
            DASH,
            GREATER_THAN,
            NO_HAVE
        };

    public:
        RequestHandler(transport::TransportCatalogue& db);

        void AddBus(const std::string_view raw_query);
        void AddStop(const std::string_view raw_query);
        void SetDistanceBetweenStops(const std::string_view raw_query);

        std::optional<BusStat>              GetBusStat(const std::string_view bus_name)      const;
        std::optional<StopStat>             GetStopStat(const std::string_view stop_name)    const;
        const std::unordered_set<BusPtr>*   GetBusesByStop(const std::string_view stop_name) const;
        std::tuple<double, int>             ComputeRouteLengths(const std::vector<std::string_view>& routh) const;
        std::vector<StopPtr>                StopsToStopPtr(const std::vector<std::string_view>& stops) const;

    private:
        transport::TransportCatalogue& db_;

        std::tuple<std::string, std::size_t>                QueryGetName(const std::string_view str)                                     const;
        std::tuple<std::string, std::string>                SplitIntoLengthStop(std::string&& str)                                       const;
        std::tuple<std::vector<std::string>, SeparatorType> SplitIntoWordsBySeparator(const std::string_view str)                        const;
        std::tuple<std::vector<std::string_view>, int>      WordsToRoute(const std::vector<std::string>& words, SeparatorType separator) const;
    };
}
 