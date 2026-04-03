#include <gtest/gtest.h>
#include "../Lang/Translator.h"
#include "../API/ApiClient.h"
#include <fstream>
#include <windows.h>

int main(int argc, char **argv) {
 
    SetDllDirectoryA("libs");
    
    #ifdef _WIN32
        SetCurrentDirectoryA("..");
    #endif

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

//TEST TRANSLATOR

// Sprawdź czy zwraca poprawne tłumaczenie po polsku
TEST(TranslatorTest, ReturnPolishTranslation) {
    Translator t(Translator::Language::PL);
    EXPECT_EQ(t["search_btn"], "Szukaj");
    EXPECT_EQ(t["btn_chart"],  "Wykres");
    EXPECT_EQ(t["col_street"], "Ulica");
}

// Sprawdź czy zwraca poprawne tłumaczenie po angielsku
TEST(TranslatorTest, ReturnEnglishTranslation) {
    Translator t(Translator::Language::EN);
    EXPECT_EQ(t["search_btn"], "Search");
    EXPECT_EQ(t["btn_chart"],  "Chart");
    EXPECT_EQ(t["col_street"], "Street");
}

// Sprawdź czy zwraca klucz jeśli tłumaczenie nie istnieje
TEST(TranslatorTest, ReturnKeyIfMissing) {
    Translator t(Translator::Language::PL);
    EXPECT_EQ(t["nieistniejacy_klucz"], "nieistniejacy_klucz");
}

// Sprawdź czy getLanguage zwraca aktualny język
TEST(TranslatorTest, GetLanguageReturnsCorrect) {
    Translator t(Translator::Language::PL);
    EXPECT_EQ(t.getLanguage(), Translator::Language::PL);
    t.setLanguage(Translator::Language::EN);
    EXPECT_EQ(t.getLanguage(), Translator::Language::EN);
}


//TESTY API

// Sprawdź czy zwraca false dla nieistniejącego pliku
TEST(ApiClientTest, FileNotExists) {
    EXPECT_FALSE(ApiClient::fileExists("nieistniejacy_plik.json"));
}

// Sprawdź czy zwraca true dla istniejącego pliku
TEST(ApiClientTest, FileExists) {
    std::ofstream tmp("tmp_test.json");
    tmp << "{\"test\": 1}";
    tmp.close();

    EXPECT_TRUE(ApiClient::fileExists("tmp_test.json"));
    std::remove("tmp_test.json");
}

// Sprawdź czy zwraca false dla pustego pliku
TEST(ApiClientTest, EmptyFileReturnsFalse) {
    std::ofstream tmp("tmp_empty.json"); // bez "tests/"
    tmp.close();

    EXPECT_FALSE(ApiClient::fileExists("tmp_empty.json"));
    std::remove("tmp_empty.json");
}