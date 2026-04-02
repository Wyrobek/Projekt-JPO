#pragma once
#include <string>
#include <map>
#include <nlohmann/json.hpp>

using json = nlohmann::ordered_json;

/**
 * Klasa odpowiedzialna za obsługę tłumaczeń interfejsu
 * Wczytuje tłumaczenia z pliku JSON i udostępnia je przez operator []
 */
class Translator {
public:
    enum class Language { PL, EN };

    /// Wczytuje tłumaczenia z pliku i ustawia język
    Translator(Language lang = Language::PL);

    /// Zmienia język i przeładowuje tłumaczenia
    void setLanguage(Language lang);

    /// Zwraca aktualne tłumaczenie dla podanego klucza
    /// Jeśli klucz nie istnieje zwraca sam klucz
    std::string get(const std::string& key) const;

    /// Skrócony zapis — odpowiednik get()
    std::string operator[](const std::string& key) const;

    Language getLanguage() const { return currentLanguage; }

private:
    Language currentLanguage;
    std::map<std::string, std::string> translations;

    void load();
};