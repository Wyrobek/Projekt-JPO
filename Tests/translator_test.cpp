#include <gtest/gtest.h>
#include "../Lang/Translator.h"

// Sprawdź czy zwraca poprawne tłumaczenie po polsku
TEST(TranslatorTest, ReturnPolishTranslation) {
    Translator t(Translator::Language::PL);
    EXPECT_EQ(t["search_btn"], "Szukaj");
}

// Sprawdź czy zwraca poprawne tłumaczenie po angielsku
TEST(TranslatorTest, ReturnEnglishTranslation) {
    Translator t(Translator::Language::EN);
    EXPECT_EQ(t["search_btn"], "Search");
}

// Sprawdź czy zwraca klucz jeśli tłumaczenie nie istnieje
TEST(TranslatorTest, ReturnKeyIfMissing) {
    Translator t(Translator::Language::PL);
    EXPECT_EQ(t["nieistniejacy_klucz"], "nieistniejacy_klucz");
}

// Sprawdź czy zmiana języka działa
TEST(TranslatorTest, SwitchLanguage) {
    Translator t(Translator::Language::PL);
    EXPECT_EQ(t["search_btn"], "Szukaj");
    t.setLanguage(Translator::Language::EN);
    EXPECT_EQ(t["search_btn"], "Search");
}