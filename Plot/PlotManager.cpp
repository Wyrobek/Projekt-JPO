#include "PlotManager.h"
#include <fstream>
#include <sstream>
#include <thread>
#include <algorithm>
#include <windows.h>

using namespace std;

const string PlotManager::TEMP_DATA_FILE   = "Plot/tmp_plot.dat";
const string PlotManager::TEMP_SCRIPT_FILE = "Plot/tmp_plot.gp";

/**
 * Przetwarza dane pomiarowe z JSON i zapisuje do pliku tymczasowego
 * Dla wielu dni zapisuje pełną datę i godzinę, dla jednego dnia tylko godzinę
 * @param data obiekt JSON z pomiarami
 * @param days liczba dni — wpływa na format etykiet osi X
 */
int PlotManager::writeTempData(const json& data, int days) {
    vector<pair<string, double>> measures;

    for (const auto& measure : data["pomiary"]) {
        if (!measure.contains("Wartość") || measure["Wartość"].is_null()) continue;

        string fullDate = measure.value("Data", "");
        string label;

        if (days == 1) {
            // Tylko godzina np. "14:00"
            label = fullDate.size() >= 16 ? fullDate.substr(11, 5) : to_string(measures.size());
        } else {
            // Data i godzina np. "22.03 14:00"
            if (fullDate.size() >= 16) {
                string day   = fullDate.substr(8,  2); // dd
                string month = fullDate.substr(5,  2); // MM
                string hour  = fullDate.substr(11, 5); // HH:MM
                label = day + "." + month + " " + hour;
            } else {
                label = to_string(measures.size());
            }
        }

        double value = measure["Wartość"].get<double>();
        measures.push_back({label, value});
    }

    // Odwróć — API zwraca od najnowszych, wykres od najstarszych
    reverse(measures.begin(), measures.end());

    ofstream tmp(TEMP_DATA_FILE);
    for (int i = 0; i < (int)measures.size(); i++) {
        tmp << i << " " << measures[i].second << " \"" << measures[i].first << "\"\n";
    }
    tmp.close();

    return (int)measures.size();
}

/**
 * Generuje etykiety osi X
 * Dla 1 dnia pokazuje co ~4 godziny (co 4 punkty)
 * Dla 2-3 dni pokazuje więcej przedziałów z datą i godziną
 */
void PlotManager::writeXTics(ofstream& script, int total, int days) {
    script << "set xtics (";

    ifstream reread(TEMP_DATA_FILE);
    string line;
    int idx  = 0;

    // Więcej przedziałów dla wielu dni
    int step;
    if      (days == 1) step = max(1, total / 6);  //  ~6 etykiet dla 1 dnia
    else if (days == 2) step = max(1, total / 8);  //  ~8 etykiet dla 2 dni
    else                step = max(1, total / 10); // ~10 etykiet dla 3 dni

    bool first = true;

    while (getline(reread, line)) {
        istringstream iss(line);
        int x; double y; string label;
        iss >> x >> y >> label;

        if (idx % step == 0) {
            if (!first) script << ", ";
            label.erase(remove(label.begin(), label.end(), '"'), label.end());
            script << "'" << label << "' " << x;
            first = false;
        }
        idx++;
    }

    reread.close();
    script << ")\n";
}

/**
 * Uruchamia gnuplot bez okna konsoli
 */
void PlotManager::runGnuplot(const string& flags) {
    string cmd = "gnuplot " + flags + " " + TEMP_SCRIPT_FILE;

    // Sprawdź czy pliki tymczasowe istnieją
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
    testData.close();
    testScript.close();

    // Sprawdź czy gnuplot jest dostępny
    STARTUPINFOA si_test = {};
    PROCESS_INFORMATION pi_test = {};
    si_test.cb = sizeof(si_test);
    string test_cmd = "gnuplot --version";

    BOOL gnuplot_found = CreateProcessA(
        nullptr,
        const_cast<char*>(test_cmd.c_str()),
        nullptr, nullptr, FALSE,
        CREATE_NO_WINDOW, // ← bez konsoli
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

    WaitForSingleObject(pi_test.hProcess, 3000);
    CloseHandle(pi_test.hProcess);
    CloseHandle(pi_test.hThread);

    // Uruchom wykres — BEZ konsoli
    STARTUPINFOA si = {};
    PROCESS_INFORMATION pi = {};
    si.cb = sizeof(si);

    BOOL result = CreateProcessA(
        nullptr,
        const_cast<char*>(cmd.c_str()),
        nullptr, nullptr, FALSE,
        CREATE_NO_WINDOW, // ← kluczowa zmiana: brak okna konsoli
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

/**
 * Generuje wykres liniowy z nazwą wskaźnika w tytule
 * @param data      dane pomiarowe
 * @param indicator nazwa wskaźnika np. "PM10" — wyświetlana w tytule
 */
bool PlotManager::generateChart(const json& data, const string& indicator) {
    // Oblicz liczbę dni na podstawie liczby pomiarów
    int totalPomiary = 0;
    for (const auto& p : data["pomiary"])
        if (!p["Wartość"].is_null()) totalPomiary++;
    int days = totalPomiary <= 24 ? 1 : (totalPomiary <= 48 ? 2 : 3);

    int count = writeTempData(data, days);
    if (count == 0) return false;

    // Tytuł z nazwą wskaźnika jeśli podana
    string title = indicator.empty()
        ? "Pomiary czujnika"
        : "Pomiary: " + indicator;

    // Etykieta osi Y z nazwą wskaźnika
    string ylabel = indicator.empty()
        ? "Wartosc (ug/m3)"
        : indicator + " (ug/m3)";

    ofstream script(TEMP_SCRIPT_FILE);
    script << "set title '" << title << "'\n";
    script << "set xlabel 'Czas'\n";
    script << "set ylabel '" << ylabel << "'\n";
    script << "set grid\n";
    script << "set style line 1 lc rgb '#1E90FF' lw 2 pt 7 ps 0.8\n";
    script << "set xtics rotate by -45\n";
    script << "set xtics font ',9'\n";
    script << "set key off\n";
    script << "set xrange [0:" << (count - 1) << "]\n";
    script << "set yrange [0:*]\n"; // oś Y od 0

    writeXTics(script, count, days);

    script << "plot '" << TEMP_DATA_FILE << "' using 1:2 with linespoints ls 1\n";
    script << "pause mouse close\n";
    script.close();

    runGnuplot("-persist");
    return true;
}

/**
 * Generuje wykres z regresją liniową i nazwą wskaźnika w tytule
 * @param data      dane pomiarowe
 * @param indicator nazwa wskaźnika np. "PM10"
 */
bool PlotManager::generateRegressionChart(const json& data, const string& indicator) {
    int totalPomiary = 0;
    for (const auto& p : data["pomiary"])
        if (!p["Wartość"].is_null()) totalPomiary++;
    int days = totalPomiary <= 24 ? 1 : (totalPomiary <= 48 ? 2 : 3);

    int count = writeTempData(data, days);
    if (count == 0) return false;

    string title = indicator.empty()
        ? "Pomiary czujnika z regresją"
        : "Pomiary: " + indicator + " (z regresją)";

    string ylabel = indicator.empty()
        ? "Wartosc (ug/m3)"
        : indicator + " (ug/m3)";

    ofstream script(TEMP_SCRIPT_FILE);
    script << "set title '" << title << "'\n";
    script << "set xlabel 'Czas'\n";
    script << "set ylabel '" << ylabel << "'\n";
    script << "set grid\n";
    script << "set style line 1 lc rgb '#1E90FF' lw 2 pt 7 ps 0.8\n";
    script << "set style line 2 lc rgb '#FF0000' lw 2\n";
    script << "set xtics rotate by -45\n";
    script << "set xtics font ',9'\n";
    script << "set key box top left\n";
    script << "set xrange [0:" << (count - 1) << "]\n";
    script << "set yrange [0:*]\n";

    writeXTics(script, count, days);

    script << "f(x) = a * x + b\n";
    script << "set fit quiet\n";
    script << "fit f(x) '" << TEMP_DATA_FILE << "' using 1:2 via a, b\n";

    // Tytuł legendy z nazwą wskaźnika
    string dataTitle = indicator.empty() ? "Dane z czujnika" : indicator;
    script << "plot '" << TEMP_DATA_FILE << "' using 1:2 with linespoints ls 1 title '" << dataTitle << "', \\\n";
    script << "     f(x) with lines ls 2 title 'Regresja liniowa'\n";
    script << "pause mouse close\n";
    script.close();

    runGnuplot("-persist");
    return true;
}