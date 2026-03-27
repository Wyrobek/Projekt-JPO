#include "PlotManager.h"
#include <fstream>
#include <sstream>
#include <thread>
#include <algorithm>
#include <cstdlib>
#include <windows.h>

using namespace std;

const string PlotManager::TEMP_DATA_FILE   = "Plot/tmp_plot.dat";
const string PlotManager::TEMP_SCRIPT_FILE = "Plot/tmp_plot.gp";

int PlotManager::writeTempData(const json& data) {
    // Zapisanie danych do wektora
    vector<pair<string, double>> measures;

    for (const auto& measure : data["pomiary"]) {
        if (!measure.contains("Wartość") || measure["Wartość"].is_null()) continue;

        string data       = measure.value("Data", "");
        string time = data.size() >= 16 ? data.substr(11, 5) : to_string(measures.size());
        double value     = measure["Wartość"].get<double>();

        measures.push_back({time, value});
    }

    // Odwrócenie kolejności pomiarów
    reverse(measures.begin(), measures.end());

    // Zapissanie do pliku
    ofstream tmp(TEMP_DATA_FILE);
    for (int i = 0; i < (int)measures.size(); i++) {
        tmp << i << " " << measures[i].second << " \"" << measures[i].first << "\"\n";
    }
    tmp.close();

    return (int)measures.size();
}

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
            label.erase(remove(label.begin(), label.end(), '"'), label.end());
            script << "'" << label << "' " << x;
            first = false;
        }
        idx++;
    }

    reread.close();
    script << ")\n";
}

void PlotManager::runGnuplot(const string& flags) {
    string cmd = "gnuplot " + flags + " " + TEMP_SCRIPT_FILE;

    STARTUPINFOA si = {};
    PROCESS_INFORMATION pi = {};
    si.cb = sizeof(si);

    // Uruchom gnuplot bez okna konsoli
    CreateProcessA(
        nullptr,
        const_cast<char*>(cmd.c_str()),
        nullptr, nullptr,
        FALSE,
        CREATE_NO_WINDOW,
        nullptr, nullptr,
        &si, &pi
    );

    // Zamknij uchwyty w osobnym wątku żeby nie blokować GUI
    thread([pi]() mutable {
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }).detach();
}

bool PlotManager::generateChart(const json& data) {
    int count = writeTempData(data);
    if (count == 0) return false;

    ofstream script(TEMP_SCRIPT_FILE);
    script << "set title 'Pomiary czujnika'\n";
    script << "set xlabel 'Godzina'\n";
    script << "set ylabel 'Wartosc (ug/m3)'\n";
    script << "set grid\n";
    script << "set style line 1 lc rgb '#1E90FF' lw 2 pt 7 ps 0.8\n";
    script << "set xtics rotate by -45\n";
    script << "set xtics font ',9'\n";
    script << "set key off\n";
    script << "set xrange [0:" << (count - 1) << "]\n"; // ← lewo→prawo

    writeXTics(script, count);

    script << "plot '" << TEMP_DATA_FILE << "' using 1:2 with linespoints ls 1\n";
    script << "pause mouse close\n";
    script.close();

    runGnuplot("-persist");
    return true;
}

bool PlotManager::generateRegressionChart(const json& data) {
    int count = writeTempData(data);
    if (count == 0) return false;

    ofstream script(TEMP_SCRIPT_FILE);
    script << "set title 'Pomiary czujnika'\n";
    script << "set xlabel 'Godzina'\n";
    script << "set ylabel 'Wartosc (ug/m3)'\n";
    script << "set grid\n";
    script << "set style line 1 lc rgb '#1E90FF' lw 2 pt 7 ps 0.8\n";
    script << "set style line 2 lc rgb '#FF0000' lw 2\n";
    script << "set xtics rotate by -45\n";
    script << "set xtics font ',9'\n";
    script << "set key box top left\n";
    script << "set xrange [0:" << (count - 1) << "]\n"; // ← lewo→prawo

    writeXTics(script, count);

    script << "f(x) = a * x + b\n";
    script << "set fit quiet\n";
    script << "fit f(x) '" << TEMP_DATA_FILE << "' using 1:2 via a, b\n";

    script << "plot '" << TEMP_DATA_FILE << "' using 1:2 with linespoints ls 1 title 'Dane z czujnika', \\\n";
    script << "     f(x) with lines ls 2 title 'Regresja liniowa'\n";
    script << "pause mouse close\n";
    script.close();

    runGnuplot("-persist");
    return true;
}
