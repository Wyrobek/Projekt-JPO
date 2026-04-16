// ApiClient.cpp
#include "ApiClient.h"
#include <fstream>
#include <iostream>
#include <cpr/cpr.h>
#include <stdexcept>

using namespace std;

class OfflineModeFallback {};
class CriticalDataError : public runtime_error {
public:
    CriticalDataError() : runtime_error("Brak internetu oraz brak pliku ") {}
};

// Bazowy adres API GIOŚ — wszystkie endpointy zaczynają się od tego URL
const std::string ApiClient::BASE_URL = "https://api.gios.gov.pl/pjp-api/v1/rest";

/**
 * Sprawdza połączenie z internetem wysyłając testowe zapytanie do API
 * Używa krótkiego timeoutu żeby nie blokować GUI przy braku sieci
 * cpr::ssl::CaInfo wskazuje plik certyfikatów CA dla weryfikacji HTTPS
 * @return true jeśli odpowiedź HTTP to 200 OK
 */
bool ApiClient::isInternetAvailable() {
    cpr::Response response = cpr::Get(
        cpr::Url{BASE_URL + "/station/findAll?size=1"},
        cpr::Timeout{TIMEOUT_MS},
        cpr::Ssl(cpr::ssl::CaInfo{"curl-ca-bundle.crt"})
    );
    return response.status_code == 200;
}

/**
 * Sprawdza czy plik istnieje na dysku i zawiera jakieś dane
 * file.peek() != eof() odróżnia pusty plik od nieistniejącego
 * Pusty plik JSON powodowałby crash przy parsowaniu — dlatego oba warunki
 */
bool ApiClient::fileExists(const std::string& filename) {
    ifstream file(filename);
    return file.good() && file.peek() != ifstream::traits_type::eof();
}

/**
 * Pobiera listę wszystkich stacji pomiarowych z API GIOŚ
 * Grupuje stacje według miast — klucz to nazwa miasta, wartość to tablica stacji
 * Zapisuje do API/dataBase.json
 *
 * Tryb offline: jeśli brak internetu ale plik istnieje — używa zapisanych danych
 * Tryb online:  pobiera świeże dane i nadpisuje plik
 */
void ApiClient::fetchStations() {
    try {
        if (!isInternetAvailable()) {
            if (fileExists("API/dataBase.json")) {
                throw OfflineModeFallback(); 
            }
            throw CriticalDataError();
        }

    } catch (const OfflineModeFallback&) {
        cout << "Brak internetu - używasz zapisanych danych\n";
        return;
        
    } catch (const CriticalDataError& e) {
        cout << e.what() << "\n";
        return;
        
    } catch (const exception& e) {
        cout << "Nieznany błąd: " << e.what() << "dataBase.json" << "\n";
        return;
    }

    // if (!isInternetAvailable()) {
    //     if (fileExists("API/dataBase.json")) {
    //         cout << "Brak internetu - używasz zapisanych danych\n";
    //         return;
    //     }
    //     cout << "Brak internetu oraz brak pliku dataBase.json!\n";
    //     return;
    // }

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

        // Jeśli miasto pojawia się po raz pierwszy — utwórz pustą tablicę
        if (!dataBase.contains(city)) dataBase[city] = json::array();

        // Zapisz tylko potrzebne pola — pomijamy zbędne metadane z API
        json newStation;
        newStation["Id"]          = station["Identyfikator stacji"];
        newStation["Miasto"]      = station["Nazwa miasta"];
        newStation["Ulica"]       = station["Ulica"].is_null() ? "brak" : station["Ulica"];
        newStation["Gmina"]       = station["Gmina"];
        newStation["Powiat"]      = station["Powiat"];
        newStation["Województwo"] = station["Województwo"];

        dataBase[station["Nazwa miasta"]].push_back(newStation);
    }

    // Zapisz pogrupowane dane do pliku z wcięciami dla czytelności
    ofstream file("API/dataBase.json");
    file << dataBase.dump(4);
}

/**
 * Pobiera listę sensorów dla podanej stacji
 * Jeden sensor = jedno stanowisko pomiarowe mierzące konkretny wskaźnik
 * np. stacja może mieć sensory dla PM10, PM2.5, NO2, O3 jednocześnie
 * Zapisuje do API/sensors.json
 */
void ApiClient::fetchSensors(int stationId) {
    try {
        if (!isInternetAvailable()) {
            if (fileExists("API/sensors.json")) {
                throw OfflineModeFallback(); 
            }
            throw CriticalDataError();
        }

    } catch (const OfflineModeFallback&) {
        cout << "Brak internetu - używasz zapisanych danych\n";
        return;
        
    } catch (const CriticalDataError& e) {
        cout << e.what() << "\n";
        return;
        
    } catch (const exception& e) {
        cout << "Nieznany błąd: " << e.what() << "sensors.json" << "\n";
        return;
    }

    // if (!isInternetAvailable()) {
    //     if (fileExists("API/sensors.json")) {
    //         cout << "Brak internetu - używasz zapisanych danych\n";
    //         return;
    //     }
    //     cout << "Brak internetu oraz brak pliku sensors.json!\n";
    //     return;
    // }

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

    // Zapisz tylko trzy kluczowe pola każdego sensora
    for (const auto& sensor : data["Lista stanowisk pomiarowych dla podanej stacji"]) {
        json newSensor;
        newSensor["Id stanowiska"] = sensor["Identyfikator stanowiska"];
        newSensor["Id stacji"]     = sensor["Identyfikator stacji"];
        newSensor["Wskaźnik"]      = sensor["Wskaźnik"]; // np. "pył zawieszony PM10"
        sensors["sensory"].push_back(newSensor);
    }

    ofstream file("API/sensors.json");
    file << sensors.dump(4);
}

/**
 * Pobiera pomiary dla konkretnego stanowiska (sensora)
 * API zwraca dane od najnowszych — PlotManager odwraca kolejność przy rysowaniu
 * Wartość null oznacza brak pomiaru w danej godzinie — zapisywana jako nullptr
 * Zapisuje do API/measurments.json
 */
void ApiClient::fetchMeasurements(int sensorId) {
    try {
        if (!isInternetAvailable()) {
            if (fileExists("API/dataBase.json")) {
                throw OfflineModeFallback(); 
            }
            throw CriticalDataError();
        }

    } catch (const OfflineModeFallback&) {
        cout << "Brak internetu - używasz zapisanych danych\n";
        return;
        
    } catch (const CriticalDataError& e) {
        cout << e.what() << "\n";
        return;
        
    } catch (const exception& e) {
        cout << "Nieznany błąd: " << e.what() << "measurements.json" << "\n";
        return;
    }

    // if (!isInternetAvailable()) {
    //     if (fileExists("API/measurments.json")) {
    //         cout << "Brak internetu\n";
    //         return;
    //     }
    //     cout << "Brak internetu oraz brak pliku measurments.json!\n";
    //     return;
    // }

    // size=72 oznacza ostatnie 72 godziny (3 doby)
    string url = BASE_URL + "/data/getData/" + to_string(sensorId) + "?size=" + to_string(MAX_MEASUREMENTS);
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

        // Wartość null = brak pomiaru w tej godzinie — zapisz jako nullptr
        // żeby PlotManager mógł je pominąć bez crashowania
        if (measurement.contains("Wartość") && !measurement["Wartość"].is_null())
            newMeasurement["Wartość"] = measurement["Wartość"].get<double>();
        else
            newMeasurement["Wartość"] = nullptr;

        measurements["pomiary"].push_back(newMeasurement);
    }

    ofstream file("API/measurments.json");
    file << measurements.dump(4);
}

/**
 * Pobiera indeks jakości powietrza dla podanej stacji
 * Indeks to ocena 0-5 gdzie: 0=bardzo dobry, 5=bardzo zły
 * Zawiera też oceny cząstkowe dla każdego wskaźnika osobno
 * Zapisuje tylko sekcję "AqIndex" z odpowiedzi — reszta to metadane
 */
void ApiClient::fetchAirQualityIndex(int stationId) {
    try {
        if (!isInternetAvailable()) {
            if (fileExists("API/dataBase.json")) {
                throw OfflineModeFallback(); 
            }
            throw CriticalDataError();
        }

    } catch (const OfflineModeFallback&) {
        cout << "Brak internetu - używasz zapisanych danych\n";
        return;
        
    } catch (const CriticalDataError& e) {
        cout << e.what() << "\n";
        return;
        
    } catch (const exception& e) {
        cout << "Nieznany błąd: " << e.what() << "index.json" << "\n";
        return;
    }

    // if (!isInternetAvailable()) {
    //     if (fileExists("API/index.json")) {
    //         cout << "Brak internetu - używasz zapisanych danych\n";
    //         return;
    //     }
    //     cout << "Brak internetu oraz brak pliku index.json!\n";
    //     return;
    // }

    string url = BASE_URL + "/aqindex/getIndex/" + to_string(stationId);
    cpr::Response response = cpr::Get(
        cpr::Url{url},
        cpr::Ssl(cpr::ssl::CaInfo{"curl-ca-bundle.crt"})
    );

    if (response.status_code != 200) {
        cout << "Błąd HTTP: " << response.status_code << endl;
        return;
    }

    // Zapisz tylko sekcję AqIndex — pomijamy @context i inne metadane JSON-LD
    json data = json::parse(response.text);
    ofstream file("API/index.json");
    file << data["AqIndex"].dump(4);
}