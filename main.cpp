#include <windows.h>
#include <thread>
#include "API/ApiClient.h"
#include "MainWindow/MainWindow.h"
#include <windows.h>

int main() {

SetDllDirectoryA("libs");

#ifdef _WIN32
    system("chcp 65001 > nul");
    SetConsoleOutputCP(65001);
#endif

    ApiClient api;

    // Pobieranie stacji
    std::thread download([&api]() {
        api.fetchStations();
    });

    // Okno uruchamia się od razu
    MainWindow window;
    window.show();

    // Poczekaj na zakończenie wątku przed zamknięciem programu
    download.join();

    return 0;
}