cmake_minimum_required(VERSION 3.15)
project(AirQuality)

set(CMAKE_CXX_STANDARD 17)

find_package(cpr REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(nana REQUIRED)

add_executable(AirQuality
    main.cpp
    API/ApiClient.cpp
    Plot/PlotManager.cpp
    MainWindow/MainWindow.cpp
)

target_include_directories(AirQuality PRIVATE
    API
    Plot
    MainWindow
)

target_link_libraries(AirQuality
    cpr::cpr
    nlohmann_json::nlohmann_json
    nana
}