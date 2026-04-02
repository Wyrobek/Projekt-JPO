#include "Translator.h"
#include <fstream>
#include <iostream>

using namespace std;

Translator::Translator(Language lang) : currentLanguage(lang) {
    load();
}

void Translator::setLanguage(Language lang) {
    currentLanguage = lang;
    load();
}

/// Wczytuje odpowiedni plik tłumaczeń na podstawie aktualnego języka
void Translator::load() {
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

    // Wczytaj wszystkie klucz-wartość z pliku do mapy
    for (const auto& [key, value] : data.items()) {
        if (value.is_string())
            translations[key] = value.get<string>();
    }
}

string Translator::get(const std::string& key) const {
    auto it = translations.find(key);
    if (it != translations.end()) return it->second;
    return key; // zwróć klucz jeśli brak tłumaczenia
}

string Translator::operator[](const std::string& key) const {
    return get(key);
}