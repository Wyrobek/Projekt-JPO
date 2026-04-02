// ApiClient.cpp
#include "ApiClient.h"
#include <fstream>
#include <iostream>
#include <cpr/cpr.h>

using namespace std;

const std::string ApiClient::BASE_URL = "https://api.gios.gov.pl/pjp-api/v1/rest";

/*
 * Sprawdza połączenie z internetem wysyłając testowe zapytanie do API
 * Zwraca true jeśli odpowiedź HTTP to 200 OK
 */
bool ApiClient::isInternetAvailable() {
    cpr::Response response = cpr::Get(
    cpr::Url{BASE_URL + "/station/findAll?size=1"},
    cpr::Timeout{TIMEOUT_MS},
    cpr::Ssl(cpr::ssl::CaInfo{"curl-ca-bundle.crt"})
);
    return response.status_code == 200;
}

/*
 * Sprawdza czy plik istnieje na dysku i zawiera jakieś dane
 * Używane przed próbą odczytu danych offline
 */
bool ApiClient::fileExists(const std::string& filename) {
    ifstream file(filename);
    return file.good() && file.peek() != ifstream::traits_type::eof();
}

/*
 * Pobiera listę wszystkich stacji pomiarowych z API GIOŚ
 * Grupuje stacje według miast i zapisuje do API/dataBase.json
 * Przy braku internetu używa istniejącego pliku jeśli jest dostępny
 */
void ApiClient::fetchStations() {
    if (!isInternetAvailable()) {
        if (fileExists("API/dataBase.json")) {
            cout << "Brak internetu - używasz zapisanych danych\n";
            return;
        }
        cout << "Brak internetu oraz brak pliku dataBase.json file!\n";
        return;
    }

    string url = BASE_URL + "/station/findAll?size=" + to_string(MAX_STATIONS);
    cpr::Response response = cpr::Get(
    cpr::Url{url},
    cpr::Ssl(cpr::ssl::CaInfo{"curl-ca-bundle.crt"})
);

    if (response.status_code != 200) {
        cout << "Błąd HTTP: " << response.status_code << endl;
        return;
    }

    json data = json::parse(response.text);
    json dataBase;

    // Iteruj po liście stacji i grupuj je według nazwy miasta
    for (const auto& station : data["Lista stacji pomiarowych"]) {
        string city = station["Nazwa miasta"];

        // Jeśli miasto nie istnieje jeszcze w bazie, utwórz pustą tablicę
        if (!dataBase.contains(city)) dataBase[city] = json::array();

        // Zapisz tylko potrzebne pola stacji
        json newStation;
        newStation["Id"]          = station["Identyfikator stacji"];
        newStation["Miasto"]      = station["Nazwa miasta"];
        newStation["Ulica"]       = station["Ulica"].is_null() ? "brak" : station["Ulica"];
        newStation["Gmina"]       = station["Gmina"];
        newStation["Powiat"]      = station["Powiat"];
        newStation["Województwo"] = station["Województwo"];

        dataBase[station["Nazwa miasta"]].push_back(newStation);
    }

    // Zapisz pogrupowane dane do pliku
    ofstream file("API/dataBase.json");
    file << dataBase.dump(4);
}

/*
 * Pobiera listę sensorów (stanowisk pomiarowych) dla podanej stacji
 * Każdy sensor mierzy inny wskaźnik np. PM10, NO2, O3
 * Zapisuje wynik do API/sensors.json
 */
void ApiClient::fetchSensors(int stationId) {
    if (!isInternetAvailable()) {
        if (fileExists("API/sensors.json")) {
            cout << "Brak internetu - używasz zapisanych danych\n";
            return;
        }
        cout << "Brak internetu oraz brak pliku sensors.json file!\n";
        return;
    }

    string url = BASE_URL + "/station/sensors/" + to_string(stationId) + "?size=500";
    cpr::Response response = cpr::Get(
    cpr::Url{url},
    cpr::Ssl(cpr::ssl::CaInfo{"curl-ca-bundle.crt"})
);

    if (response.status_code != 200) {
        cout << "Błąd HTTP: " << response.status_code << endl;
        return;
    }

    json data = json::parse(response.text);
    json sensors;
    sensors["sensory"] = json::array();

    // Zapisz każdy sensor z ID stanowiska, ID stacji i nazwą wskaźnika
    for (const auto& sensor : data["Lista stanowisk pomiarowych dla podanej stacji"]) {
        json newSensor;
        newSensor["Id stanowiska"] = sensor["Identyfikator stanowiska"];
        newSensor["Id stacji"]     = sensor["Identyfikator stacji"];
        newSensor["Wskaźnik"]      = sensor["Wskaźnik"];
        sensors["sensory"].push_back(newSensor);
    }

    ofstream file("API/sensors.json");
    file << sensors.dump(4);
}

/*
 * Pobiera pomiary dla podanego stanowiska pomiarowego
 * Pobiera ostatnie MAX_MEASUREMENTS (72) godzinne pomiary
 * Obsługuje wartości null — niektóre pomiary mogą być niedostępne
 * Zapisuje wynik do API/measurments.json
 */
void ApiClient::fetchMeasurements(int stationId) {
    if (!isInternetAvailable()) {
        if (fileExists("API/measurments.json")) {
            cout << "Brak internetu\n";
            return;
        }
        cout << "Brak internetu oraz brak pliku measurments.json file!\n";
        return;
    }

    string url = BASE_URL + "/data/getData/" + to_string(stationId) + "?size=" + to_string(MAX_MEASUREMENTS);
    cpr::Response response = cpr::Get(
    cpr::Url{url},
    cpr::Ssl(cpr::ssl::CaInfo{"curl-ca-bundle.crt"})
);

    if (response.status_code != 200) {
        cout << "Błąd HTTP: " << response.status_code << endl;
        return;
    }

    json data = json::parse(response.text);
    if (!data.contains("Lista danych pomiarowych")) {
        cout << "Brak danych pomiarowych\n";
        return;
    }

    json measurements;
    measurements["pomiary"] = json::array();

    for (const auto& measurement : data["Lista danych pomiarowych"]) {
        json newMeasurement;
        newMeasurement["Kod stanowiska"] = measurement.value("Kod stanowiska", "brak");
        newMeasurement["Data"]           = measurement.value("Data", "brak");

        // Niektóre pomiary mogą mieć wartość null — zapisz nullptr zamiast crashować
        if (measurement.contains("Wartość") && !measurement["Wartość"].is_null())
            newMeasurement["Wartość"] = measurement["Wartość"].get<double>();
        else
            newMeasurement["Wartość"] = nullptr;

        measurements["pomiary"].push_back(newMeasurement);
    }

    ofstream file("API/measurments.json");
    file << measurements.dump(4);
}

/*
 * Pobiera indeks jakości powietrza dla podanej stacji
 * Indeks zawiera ogólną ocenę oraz szczegółowe wartości dla SO2, NO2, PM10, PM2.5, O3
 * Zapisuje wynik do API/index.json
 */
void ApiClient::fetchAirQualityIndex(int stationId) {
    if (!isInternetAvailable()) {
        if (fileExists("API/index.json")) {
            cout << "Brak internetu - używasz zapisanych danych\n";
            return;
        }
        cout << "Brak internetu oraz brak pliku index.json file!\n";
        return;
    }

    string url = BASE_URL + "/aqindex/getIndex/" + to_string(stationId);
    cpr::Response response = cpr::Get(
    cpr::Url{url},
    cpr::Ssl(cpr::ssl::CaInfo{"curl-ca-bundle.crt"})
);

    if (response.status_code != 200) {
        cout << "Błąd HTTP: " << response.status_code << endl;
        return;
    }

    // Zapisz tylko sekcję AqIndex z odpowiedzi API
    json data = json::parse(response.text);
    ofstream file("API/index.json");
    file << data["AqIndex"].dump(4);
}