cmake_minimum_required(VERSION 3.10)
project(KanjiAppSystem LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Habilita os geradores automáticos do Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Encontra as dependências instaladas globalmente no Ubuntu
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

# 1. Executável do Aplicativo Principal (Interface Gráfica + JSON)
add_executable(kanjiapp main.cpp)
target_link_libraries(kanjiapp PRIVATE Qt5::Widgets nlohmann_json::nlohmann_json)

# 2. Executável do Separador/Limpador (Apenas JSON, sem interface gráfica)
add_executable(separador separador.cpp)
target_link_libraries(separador PRIVATE nlohmann_json::nlohmann_json)