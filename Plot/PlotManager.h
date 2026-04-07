// PlotManager.h
#pragma once
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::ordered_json;

/**
 * Klasa odpowiedzialna za generowanie wykresów przez gnuplot
 * Zapisuje dane do plików tymczasowych i uruchamia gnuplot w tle
 * Wykresy otwierają się w osobnym oknie gnuplot bez blokowania GUI
 */
class PlotManager {
public:
    /**
     * Generuje prosty wykres liniowy pomiarów
     * @param data      obiekt JSON z tablicą "pomiary"
     * @param indicator nazwa mierzonego wskaźnika np. "PM10" — pojawia się w tytule
     * @return true jeśli wykres został wygenerowany, false jeśli brak danych
     */
    bool generateChart(const json& data, const std::string& indicator = "");

    /**
     * Generuje wykres liniowy z nałożoną regresją liniową
     * Regresja obliczana jest przez gnuplot metodą najmniejszych kwadratów
     * @param data      obiekt JSON z tablicą "pomiary"
     * @param indicator nazwa mierzonego wskaźnika np. "PM10"
     * @return true jeśli wykres został wygenerowany, false jeśli brak danych
     */
    bool generateRegressionChart(const json& data, const std::string& indicator = "");

private:
    /**
     * Przetwarza dane JSON i zapisuje je do pliku tymczasowego tmp_plot.dat
     * Format pliku: indeks wartość "etykieta"
     * Dla 1 dnia etykieta to godzina, dla 2-3 dni to data i godzina
     * @param data dane pomiarowe
     * @param days liczba dni danych — wpływa na format etykiet osi X
     * @return liczba zapisanych punktów (0 jeśli brak prawidłowych danych)
     */
    int writeTempData(const json& data, int days);

    /**
     * Generuje linię "set xtics" w skrypcie gnuplot
     * Dobiera automatycznie co ile punktów pokazać etykietę
     * żeby napisy nie nakładały się na wykresie
     * @param script otwarty strumień skryptu gnuplot
     * @param total  całkowita liczba punktów na wykresie
     * @param days   liczba dni — wpływa na gęstość etykiet
     */
    void writeXTics(std::ofstream& script, int total, int days);

    /**
     * Uruchamia gnuplot jako osobny proces bez okna konsoli
     * Używa WinAPI CreateProcess zamiast system() żeby uniknąć migającego CMD
     * Czeka na zakończenie procesu w osobnym wątku — nie blokuje GUI
     * @param flags dodatkowe flagi dla gnuplot np. "-persist"
     */
    void runGnuplot(const std::string& flags = "");

    /// Ścieżka do pliku z danymi pomiarowymi dla gnuplot
    static const std::string TEMP_DATA_FILE;

    /// Ścieżka do pliku skryptu gnuplot z ustawieniami wykresu
    static const std::string TEMP_SCRIPT_FILE;
};