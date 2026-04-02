#include <gtest/gtest.h>
#include "../Plot/PlotManager.h"

// Sprawdź czy generateChart zwraca false dla pustej tablicy pomiarów
TEST(PlotManagerTest, EmptyDataReturnsFalse) {
    PlotManager plot;
    json data;
    data["pomiary"] = json::array();
    EXPECT_FALSE(plot.generateChart(data));
}

// Sprawdź czy generateChart zwraca false gdy brak klucza pomiary
TEST(PlotManagerTest, MissingKeyReturnsFalse) {
    PlotManager plot;
    json data;
    EXPECT_FALSE(plot.generateChart(data));
}

// Sprawdź czy generateChart zwraca false gdy wszystkie wartości są null
TEST(PlotManagerTest, AllNullValuesReturnsFalse) {
    PlotManager plot;
    json data;
    data["pomiary"] = json::array();
    data["pomiary"].push_back({{"Data", "2026-03-22 14:00:00"}, {"Wartość", nullptr}});
    data["pomiary"].push_back({{"Data", "2026-03-22 15:00:00"}, {"Wartość", nullptr}});
    EXPECT_FALSE(plot.generateChart(data));
}

// Sprawdź czy generateRegressionChart zwraca false dla pustych danych
TEST(PlotManagerTest, RegressionEmptyDataReturnsFalse) {
    PlotManager plot;
    json data;
    data["pomiary"] = json::array();
    EXPECT_FALSE(plot.generateRegressionChart(data));
}