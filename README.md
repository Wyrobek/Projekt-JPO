
 # SPRAWDŹ JAKOŚĆ POWIETRZA
 # Aplikacja desktopowa C++ / Windows
 
## OPIS
----
Aplikacja pozwala sprawdzić jakość powietrza w Polsce na podstawie danych
z API Głównego Inspektoratu Ochrony Środowiska (GIOŚ).
Umożliwia wyszukiwanie stacji pomiarowych, przeglądanie sensorów,
odczyt bieżących pomiarów oraz generowanie wykresów z regresją liniową.
Aplikacja działa również w trybie offline, korzystając z danych
zapisanych podczas ostatniego połączenia z internetem.
 
 
## FUNKCJE
-------
- Wyszukiwanie stacji pomiarowych po nazwie miasta
- Przeglądanie listy sensorów dla wybranej stacji
- Wyświetlanie indeksu jakości powietrza (SO2, NO2, PM10, PM2.5, O3)
- Tabela bieżących pomiarów
- Wykres liniowy pomiarów (gnuplot)
- Wykres z regresją liniową (gnuplot)
- Tryb offline — dane zapisywane lokalnie w plikach JSON
- Obsługa dwóch języków: polski i angielski
 
 
## WYMAGANIA
---------
System operacyjny:  Windows 10/11 (64-bit)
Środowisko:         MSYS2 MinGW64
Kompilator:         g++ 15.x (mingw-w64-x86_64-gcc)
 
Wymagane biblioteki (instalacja przez MSYS2):
  pacman -S mingw-w64-x86_64-gcc
  pacman -S mingw-w64-x86_64-curl
  pacman -S mingw-w64-x86_64-cpr
  pacman -S mingw-w64-x86_64-nlohmann-json
  pacman -S mingw-w64-x86_64-nana
  pacman -S mingw-w64-x86_64-libpng
  pacman -S mingw-w64-x86_64-libjpeg-turbo
  pacman -S mingw-w64-x86_64-gtest
  pacman -S mingw-w64-x86_64-gnuplot
 
 
## STRUKTURA PROJEKTU
------------------
Projekt/<br/>
  main.cpp                    -- punkt wejścia aplikacji
<br/>
 
  API/<br/>
    ApiClient.h               -- nagłówek klienta API
    <br/>
    ApiClient.cpp             -- pobieranie danych z API GIOŚ
    <br/>
 
  MainWindow/<br/>
    MainWindow.h              -- nagłówek głównego okna
    <br/>
    MainWindow.cpp            -- interfejs użytkownika (Nana GUI)
    <br/>
    
  Plot/<br/>
    PlotManager.h             -- nagłówek managera wykresów
    <br/>
    PlotManager.cpp           -- generowanie wykresów (gnuplot)
    <br/>
    
  Lang/<br/>
    Translator.h              -- nagłówek klasy tłumaczeń
    <br/>
    Translator.cpp            -- obsługa wielojęzyczności
    <br/>
    pl.json                   -- tłumaczenia polskie
    <br/>
    en.json                   -- tłumaczenia angielskie
    <br/>
 
  API/                        -- pliki JSON z danymi (generowane automatycznie)
    <br/>
    dataBase.json             -- lista stacji pogrupowana po miastach
    <br/>
    sensors.json              -- sensory dla ostatnio wybranej stacji
    <br/>
    measurments.json          -- pomiary dla ostatnio wybranego sensora
    <br/>
    index.json                -- indeks jakości powietrza
    <br/>
 
  Plot/<br/>
    tmp_plot.dat              -- tymczasowe dane dla gnuplot
    <br/>
    tmp_plot.gp               -- tymczasowy skrypt gnuplot
    <br/>
 
  tests/<br/>
    main_test.cpp             -- punkt wejścia testów
    <br/>
    translator_test.cpp       -- testy klasy Translator
    <br/>
    apiclient_test.cpp        -- testy klasy ApiClient
    <br/>
    plotmanager_test.cpp      -- testy klasy PlotManager
   
 
## BUDOWANIE APLIKACJI
-------------------
Komenda w terminalu MSYS2 MinGW64:
 
  g++ main.cpp API/ApiClient.cpp MainWindow/MainWindow.cpp \
      Plot/PlotManager.cpp Lang/Translator.cpp \
      -IAPI -IMainWindow -IPlot -ILang \
      -o Powietrze.exe \
      -lcurl -std=c++17 -lnana -lgdi32 -lcomdlg32 \
      -lole32 -luuid -lcpr -lssl -lcrypto -lpng -ljpeg \
      -mwindows
 
Lub przez VS Code: Ctrl+Shift+B (task "build")
 
 
## BUDOWANIE I URUCHAMIANIE TESTÓW
--------------------------------
Komenda w terminalu MSYS2 MinGW64:
 
  g++ tests/main_test.cpp tests/translator_test.cpp \
      tests/apiclient_test.cpp tests/plotmanager_test.cpp \
      Lang/Translator.cpp API/ApiClient.cpp Plot/PlotManager.cpp \
      -ILang -IAPI -IPlot \
      -o tests/tests.exe \
      -lgtest -lcpr -lcurl -lssl -lcrypto -std=c++17
 
Uruchomienie testów:
  ./tests/tests.exe
 
Oczekiwany wynik:
  [==========] Running X tests from 3 test suites.
  [  PASSED  ] X tests.
 
 
## UŻYTKOWANIE
-----------
1. Uruchom Powietrze.exe
2. Wpisz nazwę miasta w polu tekstowym (np. "Poznań")
3. Kliknij "Szukaj" — pojawi się lista stacji
4. Kliknij na stację — pojawi się lista sensorów
5. Użyj przycisków po prawej stronie:
     - "Indeks jakości powietrza" — ogólna ocena powietrza
     - "Bieżące pomiary"         — tabela ostatnich pomiarów
     - "Wykres"                  — wykres liniowy w gnuplot
     - "Regresja liniowa"        — wykres z linią trendu
6. Przycisk języka (lewy dolny róg) — przełącza między PL i EN
 
## TRYB OFFLINE
------------
Jeśli brak połączenia z internetem, aplikacja automatycznie
korzysta z danych zapisanych podczas ostatniej sesji online.
Komunikat o braku internetu pojawi się przy wyborze stacji.
 
 
## API
---
Źródło danych: Główny Inspektorat Ochrony Środowiska (GIOŚ)
URL:           https://api.gios.gov.pl/pjp-api/v1/rest
Dokumentacja:  https://api.gios.gov.pl/pjp-api/swagger-ui/
 
 
## AUTOR
-------
Filip Wyrobek
Teleinformatyka | Semestr 4
 
================================================================================
