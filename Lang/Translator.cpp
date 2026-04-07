// Translator.cpp
#include "Translator.h"
#include <fstream>
#include <iostream>

using namespace std;

/// Konstruktor — ustawia język i od razu wczytuje tłumaczenia
Translator::Translator(Language lang) : currentLanguage(lang) {
    load();
}

/// Zmienia język i przeładowuje tłumaczenia z odpowiedniego pliku
void Translator::setLanguage(Language lang) {
    currentLanguage = lang;
    load();
}

/**
 * Wczytuje tłumaczenia z pliku JSON do mapy translations
 * Wybiera plik na podstawie aktualnego języka:
 *   PL → Lang/pl.json
 *   EN → Lang/en.json
 * Jeśli plik nie istnieje — mapa pozostaje pusta,
 * a get() będzie zwracać same klucze zamiast tłumaczeń
 */
void Translator::load() {
    // Wyczyść stare tłumaczenia przed wczytaniem nowych
    translations.clear();

    string filename = (currentLanguage == Language::PL)
        ? "Lang/pl.json"
        : "Lang/en.json";

    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Brak pliku tłumaczeń: " << filename << "\n";
        return;
    }

    json data;
    file >> data;

    // Przepisz wszystkie pary klucz-wartość z JSON do mapy
    // Pomijamy wartości które nie są stringami (liczby, tablice itp.)
    for (const auto& [key, value] : data.items()) {
        if (value.is_string())
            translations[key] = value.get<string>();
    }
}

/**
 * Szuka tłumaczenia w mapie i zwraca je
 * Jeśli klucz nie istnieje — zwraca sam klucz jako tekst awaryjny
 * Dzięki temu widać które tłumaczenie brakuje zamiast pustego przycisku
 */
string Translator::get(const std::string& key) const {
    auto it = translations.find(key);
    if (it != translations.end()) return it->second;
    return key;
}

/// Skrócony zapis — pozwala używać translator["klucz"] zamiast translator.get("klucz")
string Translator::operator[](const std::string& key) const {
    return get(key);
}