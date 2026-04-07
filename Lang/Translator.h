// Translator.h
#pragma once
#include <string>
#include <map>
#include <nlohmann/json.hpp>

using json = nlohmann::ordered_json;

/**
 * Klasa odpowiedzialna za obsługę tłumaczeń interfejsu
 * Wczytuje pary klucz→wartość z pliku JSON (pl.json lub en.json)
 * i udostępnia je przez operator[] np. translator["search_btn"]
 */
class Translator {
public:
    /// Dostępne języki aplikacji
    enum class Language { PL, EN };

    /// Konstruktor — wczytuje tłumaczenia dla podanego języka
    /// Domyślnie używa języka polskiego
    Translator(Language lang = Language::PL);

    /// Zmienia język i natychmiast przeładowuje tłumaczenia z pliku
    void setLanguage(Language lang);

    /// Zwraca tłumaczenie dla podanego klucza
    /// Jeśli klucz nie istnieje w pliku — zwraca sam klucz jako fallback
    std::string get(const std::string& key) const;

    /// Skrócony zapis zamiast get() — pozwala pisać translator["klucz"]
    std::string operator[](const std::string& key) const;

    /// Zwraca aktualnie ustawiony język
    Language getLanguage() const { return currentLanguage; }

private:
    Language currentLanguage;

    /// Mapa przechowująca tłumaczenia w pamięci: "klucz" → "przetłumaczony tekst"
    std::map<std::string, std::string> translations;

    /// Wczytuje plik językowy do mapy translations
    /// Wywołuje się automatycznie w konstruktorze i po zmianie języka
    void load();
};