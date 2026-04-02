#pragma once
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::ordered_json;

// Klasa do generowania wykresów przez gnuplot
class PlotManager {
public:
    // Generowanie wykresu z pomiarami
    bool generateChart(const json& data);

    // Generuje wykres z regresją liniową
    bool generateRegressionChart(const json& data);

private:
    // Zapisanie danych do pliku tymczasowego, zwraca liczbę zapisanych punktów
    int writeTempData(const json& data);

    // Zapisanie etykiety osi X do skryptu gnuplot
    void writeXTics(std::ofstream& script, int total);

    // Uruchamianie gnuplot w osobnym wątku
    void runGnuplot(const std::string& flags = "");

    static const std::string TEMP_DATA_FILE;
    static const std::string TEMP_SCRIPT_FILE;
};
