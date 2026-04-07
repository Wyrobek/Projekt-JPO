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
    nana::button     btnStats;          // nowy przycisk statystyk
    nana::button     btnDay1;           // filtr: ostatni dzień
    nana::button     btnDay2;           // filtr: ostatnie 2 dni  
    nana::button     btnDay3;           // filtr: ostatnie 3 dni
    nana::button     btnToggleRegression; // włącz/wyłącz regresję


    // ─── Zależności ───────────────────────────────────────────────
    ApiClient   api;    // Klient API GIOŚ — pobiera i zapisuje dane
    PlotManager plot;   // Manager wykresów — generuje wykresy przez gnuplot
    Translator translator;      ///< Obsługa tłumaczeń interfejsu
    nana::button btnLang;       ///< Przycisk przełączania języka
    
    // Dodaj zmienne stanu:
    int  currentSensorId  = -1;  // ID aktualnie wybranego sensora
    int  currentDayFilter = 1;   // aktualny filtr dni (1/2/3)
    bool showRegression   = false; // czy pokazywać regresję
    string currentIndicator = ""; // nazwa wskaźnika aktualnie wybranego sensora

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

    void onSensorSelected(const nana::arg_listbox& arg);

    void onStatsClick();
    
    void onDayFilterClick(int days);

    void onToggleRegression();

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