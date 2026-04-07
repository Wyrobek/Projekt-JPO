// main.cpp
#include <windows.h>
#include <thread>
#include "API/ApiClient.h"
#include "MainWindow/MainWindow.h"

int main() {

    // Wskaż folder z bibliotekami DLL — szukaj w libs/ zamiast w folderze głównym
    // Musi być wywołane przed jakimkolwiek ładowaniem DLL-i
    SetDllDirectoryA("libs");

    // Ustaw kodowanie UTF-8 w konsoli Windows
    // Bez tego polskie znaki wyświetlają się jako krzaczki
    #ifdef _WIN32
        system("chcp 65001 > nul");
        SetConsoleOutputCP(65001);
    #endif

    ApiClient api;

    // Pobierz listę stacji w osobnym wątku żeby nie blokować startu GUI
    // Okno aplikacji otworzy się natychmiast, dane załadują się w tle
    std::thread download([&api]() {
        api.fetchStations();
    });

    // Utwórz i wyświetl główne okno aplikacji
    // Wywołanie show() blokuje się do momentu zamknięcia okna przez użytkownika
    MainWindow window;
    window.show();

    // Poczekaj na zakończenie wątku pobierającego dane
    // przed ostatecznym zamknięciem programu
    download.join();

    return 0;
}