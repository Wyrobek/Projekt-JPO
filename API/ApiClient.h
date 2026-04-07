// ApiClient.h
#pragma once
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::ordered_json;

/**
 * Klasa odpowiedzialna za komunikację z API GIOŚ
 * (Główny Inspektorat Ochrony Środowiska)
 *
 * Wszystkie metody fetch() pobierają dane z internetu i zapisują je
 * do plików JSON w folderze API/. W przypadku braku połączenia
 * używają danych zapisanych podczas ostatniej sesji online.
 *
 * Przykład użycia:
 *   ApiClient api;
 *   api.fetchStations();
 *   api.fetchSensors(52);
 *   api.fetchMeasurements(295);
 */
class ApiClient {
public:

    /**
     * Sprawdza czy jest aktywne połączenie z internetem
     * Wysyła testowe zapytanie do API z timeoutem TIMEOUT_MS
     * @return true jeśli API odpowiedziało kodem 200 OK
     */
    static bool isInternetAvailable();

    /**
     * Sprawdza czy plik istnieje na dysku i nie jest pusty
     * Używane przed próbą odczytu danych w trybie offline
     * @param filename ścieżka do pliku
     * @return true jeśli plik istnieje i zawiera dane
     */
    static bool fileExists(const std::string& filename);

    /**
     * Pobiera listę wszystkich stacji pomiarowych z API GIOŚ
     * Grupuje stacje według miast i zapisuje do API/dataBase.json
     * Format: { "Poznań": [{stacja1}, {stacja2}], "Warszawa": [...] }
     * Przy braku internetu używa istniejącego pliku jeśli dostępny
     */
    void fetchStations();

    /**
     * Pobiera listę sensorów (stanowisk pomiarowych) dla podanej stacji
     * Każdy sensor mierzy inny wskaźnik np. PM10, NO2, O3, SO2
     * Zapisuje wynik do API/sensors.json
     * @param stationId ID stacji pomiarowej (z API GIOŚ)
     */
    void fetchSensors(int stationId);

    /**
     * Pobiera ostatnie MAX_MEASUREMENTS godzinnych pomiarów
     * dla podanego stanowiska pomiarowego
     * Obsługuje wartości null — niektóre godziny mogą nie mieć danych
     * Zapisuje wynik do API/measurments.json
     * @param sensorId ID stanowiska pomiarowego (nie stacji!)
     */
    void fetchMeasurements(int sensorId);

    /**
     * Pobiera indeks jakości powietrza dla podanej stacji
     * Indeks zawiera ogólną ocenę (0-5) oraz szczegółowe wartości
     * dla każdego wskaźnika: SO2, NO2, PM10, PM2.5, O3
     * Zapisuje wynik do API/index.json
     * @param stationId ID stacji pomiarowej
     */
    void fetchAirQualityIndex(int stationId);

private:
    /// Timeout zapytań HTTP w milisekundach
    /// Zbyt wysoki timeout blokuje GUI — 3s to rozsądny kompromis
    static constexpr int TIMEOUT_MS       = 3000;

    /// Maksymalna liczba pobieranych stacji w jednym zapytaniu
    static constexpr int MAX_STATIONS     = 500;

    /// Maksymalna liczba pobieranych pomiarów — 72h = 3 doby
    static constexpr int MAX_MEASUREMENTS = 72;

    /// Bazowy adres URL API GIOŚ — wersja v1
    static const std::string BASE_URL;
};