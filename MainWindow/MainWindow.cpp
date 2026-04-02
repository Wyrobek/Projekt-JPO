#include "MainWindow.h"
#include <fstream>
#include <sstream>

using namespace std;

MainWindow::MainWindow()
    : form          {}
    , labelInput    {form, "Wpisz szukane miasto:"}
    , inputCity     {form}
    , btnSearch     {form, "Szukaj"}
    , listStations  {form}
    , listSensors   {form}
    , labelStats    {form, "Statystyki danej stacji:"}
    , labelStatsTitle{form, "Statystyki:"}
    , btnIndex      {form, "Indeks jakości powietrza"}
    , btnMeasurements{form, "Bieżące pomiary"}
    , btnChart      {form, "Wykres"}
    , btnRegression {form, "Regresja liniowa"}
{
    form.caption("Sprawdź jakość powietrza");
    form.size({590, 580});

    setupLayout();
    bindEvents();
}

void MainWindow::show() {
    form.show();
    nana::exec();
}

void MainWindow::setupLayout() {
    labelInput.move    ({20,  15, 150,  25});
    inputCity.move     ({160, 10, 250,  30});
    btnSearch.move     ({420, 10,  70,  30});

    listStations.move({10, 60, 500, 220});
    listStations.append_header("ID",      60);
    listStations.append_header("Ulica", 200);
    listStations.append_header("Gmina",  150);
    listStations.append_header("Powiat", 150);

    labelStats.move    ({10,  295, 150,  25});

    listSensors.move   ({10,  320, 380, 250});
    listSensors.append_header("ID stanowiska",  120);
    listSensors.append_header("Wskaźnik",  250);

    labelStatsTitle.move({460, 330, 150,  30});
    btnIndex.move       ({410, 360, 150,  30});
    btnMeasurements.move({410, 410, 150,  30});
    btnChart.move       ({410, 460, 150,  30});
    btnRegression.move  ({410, 510, 150,  30});
}

void MainWindow::bindEvents() {
    btnSearch.events().click([&]() { onSearch(); });

    listStations.events().selected([&](const nana::arg_listbox& arg) {
        onStationSelected(arg);
    });

    btnIndex.events().click      ([&]() { onIndexClick();        });
    btnMeasurements.events().click([&]() { onMeasurementsClick(); });
    btnChart.events().click      ([&]() { onChartClick();        });
    btnRegression.events().click ([&]() { onRegressionClick();   });
}

void MainWindow::onSearch() {
    string city = inputCity.text();
    listStations.clear();

    json data = loadJson("API/dataBase.json");
    if (data.is_null()) return;

    if (!data.contains(city)) {
        showError("Brak wyników", "Nie znaleziono miasta: " + city);
        return;
    }

    auto category = listStations.at(0);
    for (const auto& station : data[city]) {
        category.append({
            to_string(station.value("Id", 0)),
            station.value("Ulica",  "brak"),
            station.value("Gmina",  "brak"),
            station.value("Powiat", "brak")
        });
    }
}

void MainWindow::onStationSelected(const nana::arg_listbox& arg) {
    if (!arg.item.selected()) return;

    int stationId = stoi(arg.item.text(0));

    if (!ApiClient::isInternetAvailable()) {
        showError("Brak połączenia", "Brak połączenia z internetem. Dane dostępne z ostatnio przeglądanej stacji.");

        json sensorData = loadJson("API/sensors.json");
        if (sensorData.is_null()) return;

        listSensors.clear();
        auto categorySensors = listSensors.at(0);

        for (const auto& sensor : sensorData["sensory"]) {
            if (sensor["Id stacji"].is_null() || sensor["Id stanowiska"].is_null()) continue;

            if (sensor["Id stacji"].get<int>() == stationId) {
                categorySensors.append({
                    to_string(sensor["Id stanowiska"].get<int>()),
                    sensor.value("Wskaźnik", "brak")
                });
            }
        }
        return;
    }

    api.fetchSensors(stationId);

    json sensorData = loadJson("API/sensors.json");
    if (sensorData.is_null()) return;

    api.fetchAirQualityIndex(stationId);
    api.fetchMeasurements(stationId);

    listSensors.clear();
    auto categorySensors = listSensors.at(0);

    for (const auto& sensor : sensorData["sensory"]) {
        if (sensor["Id stacji"].is_null() || sensor["Id stanowiska"].is_null()) continue;

        if (sensor["Id stacji"].get<int>() == stationId) {
            categorySensors.append({
                to_string(sensor["Id stanowiska"].get<int>()),
                sensor.value("Wskaźnik", "N/A")
            });
        }
    }

    if (listSensors.at(0).size() == 0) {
        showError("Brak danych", "Brak czujników dla stacji o ID: " + to_string(stationId));
    }
}

void MainWindow::onIndexClick() {
    json data = loadJson("API/index.json");
    if (data.is_null()) return;

    nana::form window(form, {500, 380});
    window.caption("Indeks jakości powietrza");

    string content = "";
    content += "Data obliczen:"     + safeGetStr(data, "Data wykonania obliczeń indeksu") + "\n";
    content += "\nIndeks ogolny\n";
    content += "  Wartosc:   "           + safeGetStr(data, "Wartość indeksu") + "\n";
    content += "  Kategoria: "           + safeGetStr(data, "Nazwa kategorii indeksu") + "\n";
    content += "\nWskazniki szczegolowe\n";
    content += "  SO2:   "              + safeGetStr(data, "Nazwa kategorii indeksu dla wskażnika SO2")   + "\n";
    content += "  NO2:   "              + safeGetStr(data, "Nazwa kategorii indeksu dla wskażnika NO2")   + "\n";
    content += "  PM10:  "              + safeGetStr(data, "Nazwa kategorii indeksu dla wskażnika PM10")  + "\n";
    content += "  PM2.5: "              + safeGetStr(data, "Nazwa kategorii indeksu dla wskażnika PM2.5") + "\n";
    content += "  O3:    "              + safeGetStr(data, "Nazwa kategorii indeksu dla wskażnika O3")    + "\n";
    content += "\nZanieczyszczenie krytyczne: " + safeGetStr(data, "Kod zanieczyszczenia krytycznego");

    nana::label lbl(window, content);
    lbl.move({10, 10, 470, 350});
    lbl.text_align(nana::align::left);

    window.show();
    nana::exec();
}

void MainWindow::onMeasurementsClick() {
    json data = loadJson("API/measurments.json");
    if (data.is_null()) return;

    if (!data.contains("pomiary") || data["pomiary"].empty()) {
        showError("Brak danych", "Brak dostępnych danych pomiarowych");
        return;
    }

    nana::form window(form, {500, 500});
    window.caption("Bieżące pomiary");

    nana::listbox listMeasurements(window);
    listMeasurements.move({10, 10, 470, 470});
    listMeasurements.append_header("Date",           250);
    listMeasurements.append_header("Value (μg/m³)",  195);

    auto category = listMeasurements.at(0);
    for (const auto& measurement : data["pomiary"]) {
        if (!measurement.contains("Wartość") || measurement["Wartość"].is_null()) continue;

        category.append({
            measurement.value("Data", "N/A"),
            to_string(measurement["Wartość"].get<double>())
        });
    }

    window.show();
    nana::exec();
}

void MainWindow::onChartClick() {
    json data = loadJson("API/measurments.json");
    if (data.is_null()) return;

    if (!data.contains("pomiary") || data["pomiary"].empty()) {
        showError("Brak danych", "Brak dostępnych danych pomiarowych");
        return;
    }

    if (!plot.generateChart(data)) {
        showError("Brak danych", "Brak prawidłowych pomiarów");
    }
}

void MainWindow::onRegressionClick() {
    json data = loadJson("API/measurments.json");
    if (data.is_null()) return;

    if (!data.contains("pomiary") || data["pomiary"].empty()) {
        showError("Brak danych", "Brak dostępnych danych pomiarowych");
        return;
    }

    if (!plot.generateRegressionChart(data)) {
        showError("Brak danych", "Brak prawidłowych pomiarów");
    }
}

void MainWindow::showError(const string& title, const string& message) {
    nana::msgbox mb(form, title);
    mb << message;
    mb.show();
}

json MainWindow::loadJson(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        showError("Błąd", "Nie można otworzyć pliku: " + filename);
        return nullptr;
    }
    json data;
    file >> data;
    return data;
}

string MainWindow::safeGetStr(const json& data, const string& key) {
    if (!data.contains(key) || data[key].is_null()) return "brak";
    if (data[key].is_string()) return data[key].get<string>();
    return data[key].dump();
}