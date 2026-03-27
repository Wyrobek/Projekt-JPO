#pragma once
#include <nana/gui.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/listbox.hpp>
#include <nana/gui/msgbox.hpp>
#include "../API/ApiClient.h"
#include "../Plot/PlotManager.h"

class MainWindow {
public:
    MainWindow();
    void show();

private:
    // Widgets
    nana::form       form;
    nana::label      labelInput;
    nana::textbox    inputCity;
    nana::button     btnSearch;
    nana::listbox    listStations;
    nana::listbox    listSensors;
    nana::label      labelStats;
    nana::label      labelStatsTitle;
    nana::button     btnIndex;
    nana::button     btnMeasurements;
    nana::button     btnChart;
    nana::button     btnRegression;

    // Dependencies
    ApiClient   api;
    PlotManager plot;

    // Layout & events
    void setupLayout();
    void bindEvents();

    // Event handlers
    void onSearch();
    void onStationSelected(const nana::arg_listbox& arg);
    void onIndexClick();
    void onMeasurementsClick();
    void onChartClick();
    void onRegressionClick();

    // Helpers
    void showError(const std::string& title, const std::string& message);
    json loadJson(const std::string& filename);
    std::string safeGetStr(const json& data, const std::string& key);
};
