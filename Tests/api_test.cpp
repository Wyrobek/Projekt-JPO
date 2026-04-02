#include "../API/ApiClient.h"
#include <gtest/gtest.h>
#include <fstream>

// Sprawdź czy plik istnieje zwraca false dla nieistniejącego pliku
TEST(ApiClientTest, FileNotExists) {
    EXPECT_FALSE(ApiClient::fileExists("nieistniejacy_plik.json"));
}

// Sprawdź czy plik istnieje zwraca true dla istniejącego pliku
TEST(ApiClientTest, FileExists) {
    // Stwórz tymczasowy plik
    std::ofstream tmp("test_tmp.json");
    tmp << "{}";
    tmp.close();

    EXPECT_TRUE(ApiClient::fileExists("test_tmp.json"));

    // Usuń po teście
    std::remove("test_tmp.json");
}