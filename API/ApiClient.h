#pragma once
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::ordered_json;

class ApiClient {
public:
    static bool isInternetAvailable();
    static bool fileExists(const std::string& filename);

    void fetchStations();
    void fetchSensors(int stationId);
    void fetchMeasurements(int stationId);
    void fetchAirQualityIndex(int stationId);

private:
    static constexpr int TIMEOUT_MS       = 3000;
    static constexpr int MAX_STATIONS     = 500;
    static constexpr int MAX_MEASUREMENTS = 72;

    static const std::string BASE_URL;
};
