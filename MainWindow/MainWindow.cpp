// MainWindow.cpp
#include "MainWindow.h"
#include <fstream>
#include <sstream>

using namespace std;

/*
 * Konstruktor inicjalizuje wszystkie widgety przekazując im rodzica (form)
 * Kolejność inicjalizacji musi zgadzać się z kolejnością w headerze
 */
MainWindow::MainWindow()
    : form          {}
    , labelInput    {form}
    , inputCity     {form}
    , btnSearch     {form}
    , listStations  {form}
    , listSensors   {form}
    , labelStats    {form}
    , labelStatsTitle{form}
    , btnIndex      {form}
    , btnMeasurements{form}
    , btnChart      {form}
    , btnRegression {form}
    , btnLang       {form}
    , translator    {Translator::Language::PL}
{
    form.caption(translator["app_title"]);
    form.size({590, 610}); // trochę wyższe żeby zmieścił się przycisk języka

    setupLayout();
    refreshLabels();
    bindEvents();
}

// Wyświetla okno i przekazuje sterowanie do pętli zdarzeń Nana
void MainWindow::show() {
    form.show();
    nana::exec();
}

/*
 * Ustawia pozycje i rozmiary wszystkich widgetów
 * Format move({x, y, szerokość, wysokość})
 */
void MainWindow::setupLayout() {
    labelInput.move    ({20,  15, 150,  25});
    inputCity.move     ({160, 10, 250,  30});
    btnSearch.move     ({420, 10,  70,  30});

    listStations.move({10, 60, 500, 220});
    listStations.append_header(translator["col_id"],       60);
    listStations.append_header(translator["col_street"],  200);
    listStations.append_header(translator["col_district"],150);
    listStations.append_header(translator["col_county"],  150);

    labelStats.move    ({10,  295, 150,  25});

    listSensors.move   ({10,  320, 380, 240});
    listSensors.append_header(translator["col_sensor_id"], 120);
    listSensors.append_header(translator["col_indicator"], 250);

    labelStatsTitle.move({460, 330, 150,  30});
    btnIndex.move       ({410, 360, 150,  30});
    btnMeasurements.move({410, 410, 150,  30});
    btnChart.move       ({410, 460, 150,  30});
    btnRegression.move  ({410, 510, 150,  30});
    btnLang.move        ({10, 570, 150,  30}); // przycisk języka na dole
}



/// Przełącza język między PL i EN i odświeża interfejs
void MainWindow::switchLanguage() {
    if (translator.getLanguage() == Translator::Language::PL)
        translator.setLanguage(Translator::Language::EN);
    else
        translator.setLanguage(Translator::Language::PL);

    refreshLabels();
    refreshHeaders(); // odśwież nagłówki tabel
}


// Podpina lambdy z handlerami do zdarzeń wszystkich widgetów
void MainWindow::bindEvents() {
    btnSearch.events().click([&]() { onSearch(); });

    listStations.events().selected([&](const nana::arg_listbox& arg) {
        onStationSelected(arg);
    });

    btnIndex.events().click      ([&]() { onIndexClick();        });
    btnMeasurements.events().click([&]() { onMeasurementsClick(); });
    btnChart.events().click      ([&]() { onChartClick();        });
    btnRegression.events().click ([&]() { onRegressionClick();   });
    btnLang.events().click       ([&]() { switchLanguage();      }); // przełącz język
}

/*
 * Obsługuje wyszukiwanie stacji po nazwie miasta
 * Wczytuje dataBase.json i wyświetla stacje pasujące do wpisanego miasta
 */
void MainWindow::onSearch() {
    string city = inputCity.text();
    listStations.clear();

    json data = loadJson("API/dataBase.json");
    if (data.is_null()) return;

    if (!data.contains(city)) {
        showError(translator["title_no_results"], translator["err_no_city"] + city);
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

/*
 * Obsługuje zaznaczenie stacji na liście
 * Przy braku internetu używa ostatnio zapisanych danych
 * Przy dostępnym internecie pobiera świeże dane z API
 */
void MainWindow::onStationSelected(const nana::arg_listbox& arg) {
    if (!arg.item.selected()) return;

    int stationId = stoi(arg.item.text(0));

    // Tryb offline — użyj zapisanych danych sensorów
    if (!ApiClient::isInternetAvailable()) {
        showError(translator["title_no_internet"], translator["err_no_internet"]);

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

    // Tryb online — pobierz świeże dane z API
    api.fetchSensors(stationId);

    json sensorData = loadJson("API/sensors.json");
    if (sensorData.is_null()) return;

    // Pobierz indeks i pomiary po wczytaniu sensorów
    api.fetchAirQualityIndex(stationId);
    api.fetchMeasurements(stationId);

    // Wypełnij listę sensorów
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
        showError(translator["title_no_data"], translator["err_no_sensors"] + to_string(stationId));
    }
}

/*
 * Wyświetla indeks jakości powietrza w nowym okienku
 * Dane odczytuje z API/index.json
 */
void MainWindow::onIndexClick() {
    json data = loadJson("API/index.json");
    if (data.is_null()) return;

    nana::form window(form, {500, 380});
    window.caption(translator["title_index"]);

    string content = "";
    content += translator["index_date"]     + safeGetStr(data, "Data wykonania obliczeń indeksu") + "\n";
    content += translator["index_general"];
    content += translator["index_value"]    + safeGetStr(data, "Wartość indeksu") + "\n";
    content += translator["index_category"] + safeGetStr(data, "Nazwa kategorii indeksu") + "\n";
    content += translator["index_detailed"];
    content += "  SO2:   "                  + safeGetStr(data, "Nazwa kategorii indeksu dla wskażnika SO2")   + "\n";
    content += "  NO2:   "                  + safeGetStr(data, "Nazwa kategorii indeksu dla wskażnika NO2")   + "\n";
    content += "  PM10:  "                  + safeGetStr(data, "Nazwa kategorii indeksu dla wskażnika PM10")  + "\n";
    content += "  PM2.5: "                  + safeGetStr(data, "Nazwa kategorii indeksu dla wskażnika PM2.5") + "\n";
    content += "  O3:    "                  + safeGetStr(data, "Nazwa kategorii indeksu dla wskażnika O3")    + "\n";
    content += translator["index_critical"] + safeGetStr(data, "Kod zanieczyszczenia krytycznego");

    nana::label lbl(window, content);
    lbl.move({10, 10, 470, 350});
    lbl.text_align(nana::align::left);

    window.show();
    nana::exec();
}

/*
 * Wyświetla tabelę bieżących pomiarów w nowym okienku
 * Pomija rekordy z wartością null
 */
void MainWindow::onMeasurementsClick() {
    json data = loadJson("API/measurments.json");
    if (data.is_null()) return;

    if (!data.contains("pomiary") || data["pomiary"].empty()) {
        showError(translator["title_no_data"], translator["err_no_data"]);
        return;
    }

    nana::form window(form, {500, 500});
    window.caption(translator["title_measurements"]);

    nana::listbox listMeasurements(window);
    listMeasurements.move({10, 10, 470, 470});
    listMeasurements.append_header(translator["col_date"],  250);
    listMeasurements.append_header(translator["col_value"], 195);

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

// Generuje wykres liniowy pomiarów przez PlotManager
void MainWindow::onChartClick() {
    json data = loadJson("API/measurments.json");
    if (data.is_null()) return;

    if (!data.contains("pomiary") || data["pomiary"].empty()) {
        showError(translator["title_no_data"], translator["err_no_data"]);
        return;
    }

    if (!plot.generateChart(data)) {
        showError(translator["title_no_data"], translator["err_no_measures"]);
    }
}

// Generuje wykres z regresją liniową przez PlotManager
void MainWindow::onRegressionClick() {
    json data = loadJson("API/measurments.json");
    if (data.is_null()) return;

    if (!data.contains("pomiary") || data["pomiary"].empty()) {
        showError(translator["title_no_data"], translator["err_no_data"]);
        return;
    }

    if (!plot.generateRegressionChart(data)) {
        showError(translator["title_no_data"], translator["err_no_measures"]);
    }
}

// Wyświetla okienko z komunikatem błędu
void MainWindow::showError(const string& title, const string& message) {
    nana::msgbox mb(form, title);
    mb << message;
    mb.show();
}

/*
 * Wczytuje i parsuje plik JSON z dysku
 * Wyświetla błąd jeśli plik nie istnieje lub nie można go otworzyć
 */
json MainWindow::loadJson(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        showError(translator["title_error"], translator["err_no_file"] + filename);
        return nullptr;
    }
    json data;
    file >> data;
    return data;
}

/*
 * Bezpiecznie odczytuje wartość stringa z obiektu JSON
 * Obsługuje przypadki gdy klucz nie istnieje, jest nullem lub liczbą
 */
string MainWindow::safeGetStr(const json& data, const string& key) {
    if (!data.contains(key) || data[key].is_null())
        return translator["no_data_value"]; // "brak" lub "N/A"
    if (data[key].is_string()) return data[key].get<string>();
    return data[key].dump();
}


// Odświeża teksty wszystkich widgetów po zmianie języka
void MainWindow::refreshLabels() {
    form.caption          (translator["app_title"]);
    labelInput.caption    (translator["search_label"]);
    btnSearch.caption     (translator["search_btn"]);
    labelStats.caption    (translator["stats_label"]);
    labelStatsTitle.caption(translator["stats_title"]);
    btnIndex.caption      (translator["btn_index"]);
    btnMeasurements.caption(translator["btn_measurements"]);
    btnChart.caption      (translator["btn_chart"]);
    btnRegression.caption (translator["btn_regression"]);
    btnLang.caption       (translator["btn_lang"]);
}



/// Usuwa i ponownie tworzy nagłówki tabel z nowymi tłumaczeniami
/// Nana nie pozwala zmieniać tekstu nagłówka po utworzeniu
void MainWindow::refreshHeaders() {
    // Sprawdzamy czy kolumny istnieją, aby uniknąć błędów
    if (listStations.column_size() >= 4) {
        listStations.column_at(0).text(translator["col_id"]);
        listStations.column_at(1).text(translator["col_street"]);
        listStations.column_at(2).text(translator["col_district"]);
        listStations.column_at(3).text(translator["col_county"]);
    }

    if (listSensors.column_size() >= 2) {
        listSensors.column_at(0).text(translator["col_sensor_id"]);
        listSensors.column_at(1).text(translator["col_indicator"]);
    }
}