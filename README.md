============================================================================
 # SPRAWDŹ JAKOŚĆ POWIETRZA
 # Aplikacja desktopowa C++ / Windows
============================================================================


## OPIS
--------
Aplikacja pozwala sprawdzić jakość powietrza w Polsce na podstawie danych
z API Głównego Inspektoratu Ochrony Środowiska (GIOŚ).
Umożliwia wyszukiwanie stacji pomiarowych po nazwie miasta, przeglądanie
sensorów, odczyt bieżących pomiarów, statystyk oraz generowanie wykresów
z opcjonalną regresją liniową.
Aplikacja działa również w trybie offline, korzystając z danych
zapisanych podczas ostatniego połączenia z internetem.


## FUNKCJE
----------
- Wyszukiwanie stacji pomiarowych po nazwie miasta
- Przeglądanie listy sensorów dla wybranej stacji
- Wyświetlanie indeksu jakości powietrza (SO2, NO2, PM10, PM2.5, O3)
- Tabela bieżących pomiarów z filtrem zakresu (1/2/3 dni)
- Statystyki pomiarów: minimum, maksimum, średnia, odchylenie standardowe
- Wykres liniowy pomiarów (gnuplot) z nazwą mierzonego wskaźnika w tytule
- Opcjonalna regresja liniowa na wykresie (włącz/wyłącz przyciskiem)
- Filtr zakresu danych: ostatnia doba, dwie doby lub trzy doby
- Tryb offline — dane zapisywane lokalnie w plikach JSON
- Obsługa dwóch języków: polski i angielski (przełącznik w lewym dolnym rogu)
- Testy jednostkowe (Google Test) dla klas Translator, ApiClient, PlotManager


## WYMAGANIA
------------
System operacyjny:  Windows 10/11 (64-bit)
Środowisko:         MSYS2 MinGW64
Kompilator:         g++ 15.x (mingw-w64-x86_64-gcc)

Wymagane biblioteki (instalacja przez MSYS2):
  pacman -S mingw-w64-x86_64-gcc<br>
  pacman -S mingw-w64-x86_64-curl<br>
  pacman -S mingw-w64-x86_64-cpr<br>
  pacman -S mingw-w64-x86_64-nlohmann-json<br>
  pacman -S mingw-w64-x86_64-nana<br>
  pacman -S mingw-w64-x86_64-libpng<br>
  pacman -S mingw-w64-x86_64-libjpeg-turbo<br>
  pacman -S mingw-w64-x86_64-gtest<br>
  pacman -S mingw-w64-x86_64-gnuplot<br>


## STRUKTURA PROJEKTU
---------------------
Projekt/<br>
  main.cpp                    -- punkt wejścia aplikacji, inicjalizacja DLL i GUI
  
  API/<br>
    ApiClient.h                 -- nagłówek klienta API GIOŚ
    <br>
    ApiClient.cpp               -- pobieranie danych z API, obsługa trybu offline

  MainWindow/<br>
    MainWindow.h                -- nagłówek głównego okna
    <br>
    MainWindow.cpp              -- interfejs użytkownika (Nana GUI), obsługa zdarzeń

  Plot/<br>
    PlotManager.h               -- nagłówek managera wykresów
    <br>
    PlotManager.cpp             -- generowanie wykresów przez gnuplot
    <br>
    tmp_plot.dat                -- tymczasowe dane dla gnuplot (generowane automatycznie)
    <br>
    tmp_plot.gp                 -- tymczasowy skrypt gnuplot (generowany automatycznie)

  Lang/<br>
    Translator.h                -- nagłówek klasy tłumaczeń
    <br>
    Translator.cpp              -- obsługa wielojęzyczności (PL/EN)
    <br>
    pl.json                     -- tłumaczenia polskie
    <br>
    en.json                     -- tłumaczenia angielskie

  API/                        -- pliki JSON z danymi (generowane automatycznie)
  <br>
    dataBase.json               -- lista stacji pogrupowana po miastach
    <br>
    sensors.json                -- sensory dla ostatnio wybranej stacji
    <br>
    measurments.json            -- pomiary dla ostatnio wybranego sensora
    <br>
    index.json                  -- indeks jakości powietrza

  libs/                         -- biblioteki DLL wymagane przez aplikację
  <br>
    libcpr-1.dll
    <br>
    libcurl-4.dll
    <br>
    libssl-3-x64.dll
    <br>
    ... (pozostałe DLL-e)

  Tests/<br>
    main_test.cpp               -- punkt wejścia testów jednostkowych
    <br>
    translator_test.cpp         -- testy klasy Translator (6 testów)
    <br>
    apiclient_test.cpp          -- testy klasy ApiClient (3 testy)
    <br>
    plotmanager_test.cpp        -- testy klasy PlotManager (4 testy)

  Start.bat                     -- uruchamia aplikację z bibliotekami z folderu libs/
  <br>
  RunTests.bat                  -- uruchamia testy jednostkowe z bibliotekami z libs/


## BUDOWANIE APLIKACJI
----------------------
Komenda w terminalu MSYS2 MinGW64:

  g++ main.cpp API/ApiClient.cpp MainWindow/MainWindow.cpp \ <br>
      Plot/PlotManager.cpp Lang/Translator.cpp \ <br>
      -IAPI -IMainWindow -IPlot -ILang \ <br>
      -o Powietrze.exe \ <br>
      -lcurl -std=c++17 -lnana -lgdi32 -lcomdlg32 \ <br>
      -lole32 -luuid -lcpr -lssl -lcrypto -lpng -ljpeg \ <br>
      -mwindows <br>

Lub przez VS Code: Ctrl+Shift+B (task "build app")


## BUDOWANIE I URUCHAMIANIE TESTÓW
------------------------------------
Komenda w terminalu MSYS2 MinGW64:

  g++ Tests/main_test.cpp Tests/translator_test.cpp \ <br>
      Tests/apiclient_test.cpp Tests/plotmanager_test.cpp \ <br>
      Lang/Translator.cpp API/ApiClient.cpp Plot/PlotManager.cpp \ <br>
      -ILang -IAPI -IPlot \ <br>
      -o Tests/tests.exe \ <br>
      -lgtest -lgtest_main -lpthread \ <br>
      -lcpr -lcurl -lssl -lcrypto -std=c++17 <br>

Lub przez VS Code: Ctrl+Shift+P -> "Tasks: Run Task" -> "build tests" <br>

Uruchomienie testów:
  RunTests.bat                  (zalecane - ustawia PATH do libs/)
  lub: ./Tests/tests.exe        (tylko jesli libs/ jest w PATH)

Oczekiwany wynik:
  [==========] Running 9 tests from 2 test suites.
  [  PASSED  ] 9 tests.


## UŻYTKOWANIE
--------------
1. Uruchom Start.bat (lub Powietrze.exe jeśli DLL-e są w PATH)
2. Wpisz nazwę miasta w polu tekstowym (np. "Poznan")
3. Kliknij "Szukaj" -- pojawi się lista stacji pomiarowych
4. Kliknij na stację -- pojawi się lista sensorów po lewej stronie
5. Kliknij na sensor -- zostaną pobrane pomiary dla tego sensora
6. Wybierz zakres danych przyciskami: [ 1d ] [ 2d ] [ 3d ]
7. Użyj przycisków po prawej stronie:
     - "Indeks jakosci powietrza"  -- ogólna ocena jakości powietrza
     - "Biezace pomiary"           -- tabela ostatnich pomiarów
     - "Statystyki"                -- min, max, srednia, odchylenie std
     - "Regresja: OFF/ON"          -- wlącz/wyłącz linię trendu na wykresie
     - "Wykres"                    -- otwórz wykres w gnuplot
8. Przycisk języka (lewy dolny róg) -- przełącza interfejs między PL i EN


## TRYB OFFLINE
---------------
Jeśli brak połączenia z internetem, aplikacja automatycznie
korzysta z danych zapisanych podczas ostatniej sesji online.
Komunikat o braku internetu pojawi się przy wyborze stacji.
Dane z poprzedniej sesji pozostają dostępne w plikach JSON w folderze API/.


## DYSTRYBUCJA (uruchomienie na nowym komputerze)
--------------------------------------------------
Aplikacja wymaga obecności plików DLL w folderze libs/.
Uruchamiaj zawsze przez Start.bat -- skrypt automatycznie
ustawia PATH do folderu libs/ przed startem aplikacji.

Wymagane narzędzia zewnętrzne:
  - gnuplot (do generowania wykresów)
    Pobierz z: http://www.gnuplot.info/download.html
    Podczas instalacji zaznacz "Add to PATH"


## API
------
Źródło danych: Główny Inspektorat Ochrony Środowiska (GIOŚ)
URL:           https://api.gios.gov.pl/pjp-api/v1/rest
Dokumentacja:  https://api.gios.gov.pl/pjp-api/swagger-ui/

Używane endpointy:
  /station/findAll        -- lista wszystkich stacji pomiarowych
  /station/sensors/{id}  -- sensory dla podanej stacji
  /data/getData/{id}     -- pomiary dla podanego sensora
  /aqindex/getIndex/{id} -- indeks jakości powietrza dla stacji


## AUTOR
--------
Filip Wyrobek
Numer indeksu: 164350
Teleinformatyka | Semestr 4

================================================================================
