// ApiClient.h
#pragma once
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::ordered_json;

/*
 * Klasa odpowiedzialna za komunikację z API GIOŚ
 * Wszystkie metody pobierają dane z internetu i zapisują je do plików JSON
 * W przypadku braku internetu używają danych zapisanych lokalnie
 */
class ApiClient {
public:
    // Sprawdza czy jest aktywne połączenie z internetem
    static bool isInternetAvailable();

    // Sprawdza czy plik istnieje i nie jest pusty
    static bool fileExists(const std::string& filename);

    // Pobiera listę wszystkich stacji pomiarowych i zapisuje do API/dataBase.json
    void fetchStations();

    // Pobiera listę sensorów dla podanej stacji i zapisuje do API/sensors.json
    // paramater stationId ID stacji pomiarowej
    void fetchSensors(int stationId);

    // Pobiera pomiary dla podanego stanowiska i zapisuje do API/measurments.json
    // parametr stationId ID stanowiska pomiarowego
    void fetchMeasurements(int stationId);

    // Pobiera indeks jakości powietrza dla podanej stacji i zapisuje do API/index.json
    // parametr stationId ID stacji pomiarowej
    void fetchAirQualityIndex(int stationId);

private:
    /// Timeout zapytań HTTP w milisekundach
    static constexpr int TIMEOUT_MS       = 3000;

    /// Maksymalna liczba pobieranych stacji
    static constexpr int MAX_STATIONS     = 500;

    /// Maksymalna liczba pobieranych pomiarów
    static constexpr int MAX_MEASUREMENTS = 72;

    /// Bazowy adres URL API GIOŚ
    static const std::string BASE_URL;
};