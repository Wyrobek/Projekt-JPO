#include "../Plot/PlotManager.h"
#include <gtest/gtest.h>

// Sprawdź czy generateChart zwraca false dla pustych danych
TEST(PlotManagerTest, EmptyDataReturnsFalse) {
    PlotManager plot;
    json emptyData;
    emptyData["pomiary"] = json::array();
    EXPECT_FALSE(plot.generateChart(emptyData));
}

// Sprawdź czy generateChart zwraca true dla prawidłowych danych
TEST(PlotManagerTest, ValidDataReturnsTrue) {
    PlotManager plot;
    json data;
    data["pomiary"] = json::array();
    data["pomiary"].push_back({
        {"Data", "2026-03-22 14:00:00"},
        {"Wartość", 45.2}
    });
    EXPECT_TRUE(plot.generateChart(data));
}