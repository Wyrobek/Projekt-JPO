#include <gtest/gtest.h>
#include "../Lang/Translator.h"

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

// Sprawdź czy zmiana języka działa poprawnie
TEST(TranslatorTest, SwitchLanguageWorks) {
    Translator t(Translator::Language::PL);
    EXPECT_EQ(t["search_btn"], "Szukaj");
    t.setLanguage(Translator::Language::EN);
    EXPECT_EQ(t["search_btn"], "Search");
}

// Sprawdź czy powrót do polskiego działa
TEST(TranslatorTest, SwitchBackToPL) {
    Translator t(Translator::Language::EN);
    t.setLanguage(Translator::Language::PL);
    EXPECT_EQ(t["search_btn"], "Szukaj");
}

// Sprawdź czy getLanguage zwraca aktualny język
TEST(TranslatorTest, GetLanguageReturnsCorrect) {
    Translator t(Translator::Language::PL);
    EXPECT_EQ(t.getLanguage(), Translator::Language::PL);
    t.setLanguage(Translator::Language::EN);
    EXPECT_EQ(t.getLanguage(), Translator::Language::EN);
}