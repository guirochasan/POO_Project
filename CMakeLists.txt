cmake_minimum_required(VERSION 3.15)
project(RandomAnki CXX)

# Exige C++20 conforme TP3
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -------------------------------------------------------------
# MODO OFFLINE: Procura as bibliotecas já instaladas no PC
# -------------------------------------------------------------
find_package(nlohmann_json REQUIRED)
find_package(Catch2 3 REQUIRED)

# Executável principal
add_executable(app main.cpp)
target_link_libraries(app PRIVATE nlohmann_json::nlohmann_json)

# Executável de Testes
add_executable(testes test_tp3.cpp)
target_link_libraries(testes PRIVATE Catch2::Catch2WithMain nlohmann_json::nlohmann_json)

# Integração do Catch2 com o CTest
include(CTest)
include(Catch)
catch_discover_tests(testes)