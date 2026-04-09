// MainWindow.h
#pragma once
#include <nana/gui.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/listbox.hpp>
#include <nana/gui/msgbox.hpp>
#include "../API/ApiClient.h"
#include "../Plot/PlotManager.h"
#include "../Lang/Translator.h"
#include <iostream>
#include <string>
using namespace std;

/**
 * Główne okno aplikacji "Sprawdź jakość powietrza"
 * Odpowiada za wyświetlanie interfejsu użytkownika oraz obsługę zdarzeń
 * Komunikuje się z API GIOŚ przez ApiClient i rysuje wykresy przez PlotManager
 *
 * Przepływ użytkownika:
 *   1. Wpisz miasto → kliknij "Szukaj" → pojawia się lista stacji
 *   2. Kliknij stację → pojawia się lista sensorów
 *   3. Kliknij sensor → pobierają się pomiary
 *   4. Użyj przycisków po prawej żeby zobaczyć dane
 */
class MainWindow {
public:
    /// Konstruktor — tworzy okno, ustawia layout i podpina zdarzenia
    MainWindow();

    /// Wyświetla okno i uruchamia pętlę zdarzeń Nana
    /// Blokuje się do momentu zamknięcia okna przez użytkownika
    void show();

private:
    // ─── Widgety interfejsu ───────────────────────────────────────

    nana::form       form;               ///< Główne okno aplikacji
    nana::label      labelInput;         ///< Etykieta "Wpisz szukane miasto"
    nana::textbox    inputCity;          ///< Pole tekstowe do wpisania nazwy miasta
    nana::button     btnSearch;          ///< Przycisk "Szukaj" — wyszukuje stacje

    nana::listbox    listStations;       ///< Tabela stacji pomiarowych dla wybranego miasta
    nana::listbox    listSensors;        ///< Tabela sensorów dla wybranej stacji

    nana::label      labelStats;         ///< Etykieta "Statystyki danej stacji" nad listą sensorów
    nana::label      labelStatsTitle;    ///< Etykieta "Statystyki:" nad przyciskami po prawej

    nana::button     btnIndex;           ///< Otwiera okno z indeksem jakości powietrza
    nana::button     btnMeasurements;    ///< Otwiera okno z tabelą bieżących pomiarów
    nana::button     btnStats;           ///< Otwiera okno ze statystykami (min/max/średnia)
    nana::button     btnChart;           ///< Generuje wykres dla wybranego sensora

    nana::button     btnRegression;      ///< Nieużywany — zastąpiony przez btnToggleRegression
    nana::button     btnToggleRegression;///< Przełącza czy na wykresie pokazuje się regresja liniowa

    nana::button     btnDay1;            ///< Filtr danych: ostatnie 24 godziny
    nana::button     btnDay2;            ///< Filtr danych: ostatnie 48 godzin
    nana::button     btnDay3;            ///< Filtr danych: ostatnie 72 godziny (3 doby)

    // ─── Zależności ───────────────────────────────────────────────

    ApiClient   api;         ///< Klient API GIOŚ — pobiera dane i zapisuje do plików JSON
    PlotManager plot;        ///< Manager wykresów — generuje wykresy przez gnuplot
    Translator  translator;  ///< Obsługa tłumaczeń — zwraca teksty w aktualnym języku
    nana::button btnLang;    ///< Przycisk przełączania języka PL/EN

    // ─── Stan aplikacji ───────────────────────────────────────────

    int  currentSensorId  = -1;    ///< ID aktualnie wybranego sensora (-1 = brak wyboru)
    int  currentDayFilter = 1;     ///< Aktualny filtr zakresu danych: 1, 2 lub 3 dni
    bool showRegression   = false; ///< Czy na wykresie pokazywać linię regresji liniowej
    string currentIndicator = "";  ///< Nazwa wskaźnika wybranego sensora np. "PM10", "NO2"

    // ─── Konfiguracja ─────────────────────────────────────────────

    /// Ustawia pozycje i rozmiary wszystkich widgetów w oknie
    /// Format: widget.move({x, y, szerokość, wysokość})
    void setupLayout();

    /// Podpina lambdy z handlerami do zdarzeń wszystkich widgetów
    /// Wywoływane raz w konstruktorze po setupLayout()
    void bindEvents();

    // ─── Handlery zdarzeń ─────────────────────────────────────────

    /// Obsługuje kliknięcie "Szukaj" — wczytuje dataBase.json
    /// i wypełnia listStations stacjami z wpisanego miasta
    void onSearch();

    /// Obsługuje zaznaczenie stacji na liście
    /// W trybie online pobiera sensory, indeks i pierwsze pomiary z API
    /// W trybie offline używa ostatnio zapisanych danych z plików JSON
    /// @param arg zdarzenie zawierające zaznaczony element listy
    void onStationSelected(const nana::arg_listbox& arg);

    /// Obsługuje zaznaczenie sensora na liście
    /// Pobiera pomiary dla wybranego sensora i zapisuje jego nazwę wskaźnika
    /// @param arg zdarzenie zawierające zaznaczony element listy
    void onSensorSelected(const nana::arg_listbox& arg);

    /// Otwiera okno z indeksem jakości powietrza odczytanym z API/index.json
    void onIndexClick();

    /// Otwiera okno z tabelą pomiarów odczytanych z API/measurments.json
    /// Uwzględnia aktualny filtr dni (currentDayFilter)
    void onMeasurementsClick();

    /// Otwiera okno ze statystykami: min, max, średnia, odchylenie standardowe
    /// Obliczenia wykonywane na danych z aktualnego zakresu dni
    void onStatsClick();

    /// Generuje wykres dla wybranego sensora
    /// Uwzględnia filtr dni i opcję regresji (showRegression)
    /// Tytuł wykresu zawiera nazwę wskaźnika (currentIndicator)
    void onChartClick();

    /// Zmienia zakres danych wyświetlanych na wykresie i w statystykach
    /// @param days liczba dni: 1 = ostatnia doba, 2 = dwie doby, 3 = trzy doby
    void onDayFilterClick(int days);

    /// Przełącza wyświetlanie regresji liniowej na wykresie (showRegression)
    /// Aktualizuje tekst przycisku btnToggleRegression
    void onToggleRegression();

    // ─── Pomocnicze ───────────────────────────────────────────────

    /// Wyświetla okienko z komunikatem błędu
    /// @param title   tytuł okienka
    /// @param message treść komunikatu
    void showError(const std::string& title, const std::string& message);

    /// Wczytuje i parsuje plik JSON z dysku
    /// Wyświetla błąd jeśli plik nie istnieje lub nie można go otworzyć
    /// @param filename ścieżka do pliku
    /// @return sparsowany obiekt JSON lub nullptr jeśli wystąpił błąd
    json loadJson(const std::string& filename);

    /// Bezpiecznie odczytuje wartość stringa z obiektu JSON
    /// Zwraca translator["no_data_value"] ("brak"/"N/A") jeśli klucz
    /// nie istnieje lub ma wartość null
    /// @param data obiekt JSON
    /// @param key  nazwa klucza do odczytania
    std::string safeGetStr(const json& data, const std::string& key);

    /// Przełącza język między PL i EN
    /// Wywołuje refreshLabels() i refreshHeaders() żeby zaktualizować interfejs
    void switchLanguage();

    /// Odświeża teksty wszystkich przycisków i etykiet po zmianie języka
    /// Wywołuje translator[] dla każdego widgetu
    void refreshLabels();

    /// Odświeża nagłówki kolumn obu tabel po zmianie języka
    /// Nana nie pozwala zmieniać tekstu nagłówka po utworzeniu —
    /// dlatego używamy column_at().text() zamiast usuwania i tworzenia na nowo
    void refreshHeaders();
};