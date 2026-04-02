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

/**
 * Główne okno aplikacji "Sprawdź jakość powietrza"
 * Odpowiada za wyświetlanie interfejsu użytkownika oraz obsługę zdarzeń
 * Komunikuje się z API GIOŚ przez ApiClient i rysuje wykresy przez PlotManager
 */
class MainWindow {
public:
    // Konstruktor — tworzy okno, ustawia layout i podpina zdarzenia
    MainWindow();

    // Wyświetla okno i uruchamia pętlę zdarzeń Nana
    void show();

private:
    // ─── Widgety interfejsu ───────────────────────────────────────
    nana::form       form;              // Główne okno aplikacji
    nana::label      labelInput;        // Etykieta "Wpisz szukane miasto"
    nana::textbox    inputCity;         // Pole tekstowe do wpisania miasta
    nana::button     btnSearch;         // Przycisk "Szukaj"
    nana::listbox    listStations;      // Lista stacji pomiarowych dla wybranego miasta
    nana::listbox    listSensors;       // Lista sensorów dla wybranej stacji
    nana::label      labelStats;        // Etykieta "Statystyki danej stacji"
    nana::label      labelStatsTitle;   // Etykieta "Statystyki"
    nana::button     btnIndex;          // Przycisk otwierający indeks jakości powietrza
    nana::button     btnMeasurements;   // Przycisk otwierający bieżące pomiary
    nana::button     btnChart;          // Przycisk rysujący wykres pomiarów
    nana::button     btnRegression;     // Przycisk rysujący wykres z regresją liniową

    // ─── Zależności ───────────────────────────────────────────────
    ApiClient   api;    // Klient API GIOŚ — pobiera i zapisuje dane
    PlotManager plot;   // Manager wykresów — generuje wykresy przez gnuplot
    Translator translator;      ///< Obsługa tłumaczeń interfejsu
    nana::button btnLang;       ///< Przycisk przełączania języka

    // ─── Konfiguracja ─────────────────────────────────────────────
    // Ustawia pozycje i rozmiary wszystkich widgetów w oknie
    void setupLayout();

    // Podpina handlery zdarzeń do przycisków i list
    void bindEvents();

    // ─── Handlery zdarzeń ─────────────────────────────────────────
    // Obsługuje kliknięcie przycisku "Szukaj" — wyszukuje stacje w podanym mieście
    void onSearch();

    // Obsługuje zaznaczenie stacji na liście — pobiera sensory i dane
    // parametr arg argument zdarzenia zawierający zaznaczony element listy
    void onStationSelected(const nana::arg_listbox& arg);

    // Obsługuje kliknięcie przycisku "Indeks jakości powietrza"
    void onIndexClick();

    // Obsługuje kliknięcie przycisku "Bieżące pomiary"
    void onMeasurementsClick();

    // Obsługuje kliknięcie przycisku "Wykres"
    void onChartClick();

    // Obsługuje kliknięcie przycisku "Regresja liniowa"
    void onRegressionClick();

    // ─── Pomocnicze ───────────────────────────────────────────────
    // Wyświetla okienko z komunikatem błędu
    // parametr title  tytuł okienka
    // parametr message treść komunikatu
    void showError(const std::string& title, const std::string& message);

    // Wczytuje plik JSON z dysku
    // parametr filename ścieżka do pliku
    // return sparsowany obiekt JSON lub nullptr jeśli plik nie istnieje
    json loadJson(const std::string& filename);

    // Bezpiecznie odczytuje wartość stringa z obiektu JSON
    // Zwraca "brak" jeśli klucz nie istnieje lub ma wartość null
    // parametr data obiekt JSON
    // parametr key  nazwa klucza
    std::string safeGetStr(const json& data, const std::string& key);

    // Przełącza język i odświeża interfejs
    void switchLanguage();

    // Odświeża teksty wszystkich widgetów
    void refreshLabels();      

    // Odświeża nagłówki tabel po zmianie języka
    void refreshHeaders();
};