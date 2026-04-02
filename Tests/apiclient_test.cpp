#include <gtest/gtest.h>
#include "../API/ApiClient.h"
#include <fstream>

// Sprawdź czy zwraca false dla nieistniejącego pliku
TEST(ApiClientTest, FileNotExists) {
    EXPECT_FALSE(ApiClient::fileExists("nieistniejacy_plik.json"));
}

// Sprawdź czy zwraca true dla istniejącego pliku
TEST(ApiClientTest, FileExists) {
    std::ofstream tmp("tests/tmp_test.json");
    tmp << "{\"test\": 1}";
    tmp.close();

    EXPECT_TRUE(ApiClient::fileExists("tests/tmp_test.json"));
    std::remove("tests/tmp_test.json");
}

// Sprawdź czy zwraca false dla pustego pliku
TEST(ApiClientTest, EmptyFileReturnsFalse) {
    std::ofstream tmp("tests/tmp_empty.json");
    tmp.close(); // pusty plik

    EXPECT_FALSE(ApiClient::fileExists("tests/tmp_empty.json"));
    std::remove("tests/tmp_empty.json");
}