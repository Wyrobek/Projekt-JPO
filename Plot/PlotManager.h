#pragma once
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::ordered_json;

class PlotManager {
public:
    // Generowanie wykresu — przyjmuje nazwę wskaźnika do tytułu
    bool generateChart(const json& data, const std::string& indicator = "");

    // Generuje wykres z regresją liniową
    bool generateRegressionChart(const json& data, const std::string& indicator = "");

private:
    int  writeTempData(const json& data, int days);
    void writeXTics(std::ofstream& script, int total, int days);
    void runGnuplot(const std::string& flags = "");

    static const std::string TEMP_DATA_FILE;
    static const std::string TEMP_SCRIPT_FILE;
};