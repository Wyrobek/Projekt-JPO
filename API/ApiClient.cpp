#include "ApiClient.h"
#include <fstream>
#include <iostream>
#include <cpr/cpr.h>

using namespace std;

const std::string ApiClient::BASE_URL = "https://api.gios.gov.pl/pjp-api/v1/rest";

bool ApiClient::isInternetAvailable() {
    cpr::Response response = cpr::Get(
        cpr::Url{BASE_URL + "/station/findAll?size=1"},
        cpr::Timeout{TIMEOUT_MS}
    );
    return response.status_code == 200;
}

bool ApiClient::fileExists(const std::string& filename) {
    ifstream file(filename);
    return file.good() && file.peek() != ifstream::traits_type::eof();
}

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
    cpr::Response response = cpr::Get(cpr::Url{url});

    if (response.status_code != 200) {
        cout << "Błąd HTTP: " << response.status_code << endl;
        return;
    }

    json data = json::parse(response.text);
    json dataBase;

    for (const auto& station : data["Lista stacji pomiarowych"]) {
        string city = station["Nazwa miasta"];
        if (!dataBase.contains(city)) dataBase[city] = json::array();

        json newStation;
        newStation["Id"]          = station["Identyfikator stacji"];
        newStation["Miasto"]      = station["Nazwa miasta"];
        newStation["Ulica"]       = station["Ulica"].is_null() ? "brak" : station["Ulica"];
        newStation["Gmina"]       = station["Gmina"];
        newStation["Powiat"]      = station["Powiat"];
        newStation["Województwo"] = station["Województwo"];

        dataBase[station["Nazwa miasta"]].push_back(newStation);
    }

    ofstream file("API/dataBase.json");
    file << dataBase.dump(4);
}

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
    cpr::Response response = cpr::Get(cpr::Url{url});

    if (response.status_code != 200) {
        cout << "Błąd HTTP: " << response.status_code << endl;
        return;
    }

    json data = json::parse(response.text);
    json sensors;
    sensors["sensory"] = json::array();

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
    cpr::Response response = cpr::Get(cpr::Url{url});

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

        if (measurement.contains("Wartość") && !measurement["Wartość"].is_null())
            newMeasurement["Wartość"] = measurement["Wartość"].get<double>();
        else
            newMeasurement["Wartość"] = nullptr;

        measurements["pomiary"].push_back(newMeasurement);
    }

    ofstream file("API/measurments.json");
    file << measurements.dump(4);
}

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
    cpr::Response response = cpr::Get(cpr::Url{url});

    if (response.status_code != 200) {
        cout << "Błąd HTTP: " << response.status_code << endl;
        return;
    }

    json data = json::parse(response.text);
    ofstream file("API/index.json");
    file << data["AqIndex"].dump(4);
}