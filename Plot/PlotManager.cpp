#include "PlotManager.h"
#include <fstream>
#include <sstream>
#include <thread>
#include <algorithm>
#include <cstdlib>
#include <windows.h>

using namespace std;

// Ścieżki do plików tymczasowych używanych przez gnuplot
const string PlotManager::TEMP_DATA_FILE   = "Plot/tmp_plot.dat";
const string PlotManager::TEMP_SCRIPT_FILE = "Plot/tmp_plot.gp";

/*
 * Przetwarza dane pomiarowe z JSON i zapisuje je do pliku tymczasowego
 * Pomija rekordy z wartością null oraz odwraca kolejność
 * żeby najstarsze pomiary były po lewej stronie wykresu
 * @param data obiekt JSON zawierający tablicę "pomiary"
 * @return liczba zapisanych pomiarów (0 jeśli brak danych)
 */
int PlotManager::writeTempData(const json& data) {
    vector<pair<string, double>> measures;

    // Zbierz prawidłowe pomiary pomijając wartości null
    for (const auto& measure : data["pomiary"]) {
        if (!measure.contains("Wartość") || measure["Wartość"].is_null()) continue;

        string data  = measure.value("Data", "");
        // Wytnij tylko godzinę z pełnej daty np. "2026-03-22 14:00:00" → "14:00"
        string time  = data.size() >= 16 ? data.substr(11, 5) : to_string(measures.size());
        double value = measure["Wartość"].get<double>();

        measures.push_back({time, value});
    }

    // Odwróć kolejność — API zwraca od najnowszych, wykres pokazuje od najstarszych
    reverse(measures.begin(), measures.end());

    // Zapisz do pliku w formacie: indeks wartość "godzina"
    // np. "0 45.2 "08:00""
    ofstream tmp(TEMP_DATA_FILE);
    for (int i = 0; i < (int)measures.size(); i++) {
        tmp << i << " " << measures[i].second << " \"" << measures[i].first << "\"\n";
    }
    tmp.close();

    return (int)measures.size();
}

/*
 * Generuje etykiety osi X dla skryptu gnuplot
 * Wyświetla co N-ty punkt żeby etykiety się nie nakładały
 * Krok N jest dobierany automatycznie na podstawie liczby pomiarów
 * parametr script otwarty strumień skryptu gnuplot
 * parametr total  całkowita liczba pomiarów
 */
void PlotManager::writeXTics(ofstream& script, int total) {
    script << "set xtics (";

    ifstream reread(TEMP_DATA_FILE);
    string line;
    int idx  = 0;
    int step = max(1, total / 5);
    bool first = true;

    while (getline(reread, line)) {
        istringstream iss(line);
        int x; double y; string label;
        iss >> x >> y >> label;

        if (idx % step == 0) {
            if (!first) script << ", ";
            // Usuń cudzysłowy z etykiety przed wstawieniem do skryptu
            label.erase(remove(label.begin(), label.end(), '"'), label.end());
            script << "'" << label << "' " << x;
            first = false;
        }
        idx++;
    }

    reread.close();
    script << ")\n";
}

/*
 * Uruchamia gnuplot w tle bez pokazywania okna konsoli
 * Używa WinAPI CreateProcess zamiast system() żeby uniknąć
 * migającego okna CMD podczas uruchamiania
 * parametr flags dodatkowe flagi przekazywane do gnuplot np. "-persist"
 */
void PlotManager::runGnuplot(const string& flags) {
    string cmd = "gnuplot " + flags + " " + TEMP_SCRIPT_FILE;

    // DEBUG — sprawdź czy pliki tymczasowe istnieją
    ifstream testData(TEMP_DATA_FILE);
    ifstream testScript(TEMP_SCRIPT_FILE);
    
    if (!testData.is_open()) {
        MessageBoxA(nullptr, "Brak pliku tmp_plot.dat!", "Błąd wykresu", MB_OK);
        return;
    }
    if (!testScript.is_open()) {
        MessageBoxA(nullptr, "Brak pliku tmp_plot.gp!", "Błąd wykresu", MB_OK);
        return;
    }

    // DEBUG — sprawdź czy gnuplot jest dostępny
    STARTUPINFOA si_test = {};
    PROCESS_INFORMATION pi_test = {};
    si_test.cb = sizeof(si_test);
    string test_cmd = "gnuplot --version";

    BOOL gnuplot_found = CreateProcessA(
        nullptr,
        const_cast<char*>(test_cmd.c_str()),
        nullptr, nullptr,
        FALSE,
        CREATE_NO_WINDOW,
        nullptr, nullptr,
        &si_test, &pi_test
    );

    if (!gnuplot_found) {
        MessageBoxA(nullptr, 
            "gnuplot nie został znaleziony!\n"
            "Zainstaluj gnuplot i dodaj do PATH\n"
            "lub umieść gnuplot.exe w folderze aplikacji.",
            "Błąd wykresu", MB_OK);
        return;
    }

    CloseHandle(pi_test.hProcess);
    CloseHandle(pi_test.hThread);

    // Uruchom właściwy wykres
    STARTUPINFOA si = {};
    PROCESS_INFORMATION pi = {};
    si.cb = sizeof(si);

    // Pokaż okno konsoli żeby zobaczyć błędy gnuplot
    BOOL result = CreateProcessA(
        nullptr,
        const_cast<char*>(cmd.c_str()),
        nullptr, nullptr,
        FALSE,
        CREATE_NEW_CONSOLE, // zmienione z CREATE_NO_WINDOW na CREATE_NEW_CONSOLE
        nullptr, nullptr,
        &si, &pi
    );

    if (!result) {
        MessageBoxA(nullptr, "Nie udało się uruchomić gnuplot!", "Błąd", MB_OK);
        return;
    }

    thread([pi]() mutable {
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }).detach();
}

/*
 * Generuje wykres liniowy pomiarów i otwiera go w gnuplot
 * Wykres pokazuje wartości pomiarów w czasie z niebieską linią
 * parametr data obiekt JSON z danymi pomiarowymi
 * return true jeśli wykres został wygenerowany, false jeśli brak danych
 */
bool PlotManager::generateChart(const json& data) {
    int count = writeTempData(data);
    if (count == 0) return false;

    ofstream script(TEMP_SCRIPT_FILE);

    // Ustawienia ogólne wykresu
    script << "set title 'Pomiary czujnika'\n";
    script << "set xlabel 'Godzina'\n";
    script << "set ylabel 'Wartosc (ug/m3)'\n";
    script << "set grid\n";
    script << "set style line 1 lc rgb '#1E90FF' lw 2 pt 7 ps 0.8\n"; // niebieska linia z punktami
    script << "set xtics rotate by -45\n"; // etykiety pod kątem żeby się nie nakładały
    script << "set xtics font ',9'\n";
    script << "set key off\n"; // ukryj legendę
    script << "set xrange [0:" << (count - 1) << "]\n"; // zakres osi X od 0 do ostatniego pomiaru

    writeXTics(script, count);

    // Rysuj dane jako linię z punktami
    script << "plot '" << TEMP_DATA_FILE << "' using 1:2 with linespoints ls 1\n";
    script << "pause mouse close\n"; // okno zostaje otwarte do kliknięcia
    script.close();

    runGnuplot("-persist");
    return true;
}

/*
 * Generuje wykres pomiarów z nałożoną regresją liniową
 * Regresja jest obliczana przez gnuplot metodą najmniejszych kwadratów
 * Czerwona linia trendu pokazuje ogólny kierunek zmian
 * parametr data obiekt JSON z danymi pomiarowymi
 * return true jeśli wykres został wygenerowany, false jeśli brak danych
 */
bool PlotManager::generateRegressionChart(const json& data) {
    int count = writeTempData(data);
    if (count == 0) return false;

    ofstream script(TEMP_SCRIPT_FILE);

    // Ustawienia ogólne wykresu
    script << "set title 'Pomiary czujnika'\n";
    script << "set xlabel 'Godzina'\n";
    script << "set ylabel 'Wartosc (ug/m3)'\n";
    script << "set grid\n";
    script << "set style line 1 lc rgb '#1E90FF' lw 2 pt 7 ps 0.8\n"; // niebieska linia — dane
    script << "set style line 2 lc rgb '#FF0000' lw 2\n";              // czerwona linia — regresja
    script << "set xtics rotate by -45\n";
    script << "set xtics font ',9'\n";
    script << "set key box top left\n"; // legenda w lewym górnym rogu
    script << "set xrange [0:" << (count - 1) << "]\n";

    writeXTics(script, count);

    // Definicja funkcji liniowej y = a*x + b
    script << "f(x) = a * x + b\n";
    script << "set fit quiet\n"; // ukryj komunikaty fitowania
    // Dopasuj parametry a i b do danych metodą najmniejszych kwadratów
    script << "fit f(x) '" << TEMP_DATA_FILE << "' using 1:2 via a, b\n";

    // Rysuj dane i regresję na jednym wykresie
    script << "plot '" << TEMP_DATA_FILE << "' using 1:2 with linespoints ls 1 title 'Dane z czujnika', \\\n";
    script << "     f(x) with lines ls 2 title 'Regresja liniowa'\n";
    script << "pause mouse close\n";
    script.close();

    runGnuplot("-persist");
    return true;
}