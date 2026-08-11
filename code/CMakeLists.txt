cmake_minimum_required(VERSION 3.15)
project(RandomAnki CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(nlohmann_json REQUIRED)
find_package(Catch2 3 REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# Executável principal (Console)
add_executable(app main.cpp)
target_link_libraries(app PRIVATE nlohmann_json::nlohmann_json)

# Executável de Testes
add_executable(testes test_tp3.cpp)
target_link_libraries(testes PRIVATE Catch2::Catch2WithMain nlohmann_json::nlohmann_json)

# Executável Qt GUI (Q6)
add_executable(gui gui_main.cpp janela.cpp)
target_link_libraries(gui PRIVATE Qt6::Widgets nlohmann_json::nlohmann_json)
set_target_properties(gui PROPERTIES AUTOMOC ON)

include(CTest)
include(Catch)
catch_discover_tests(testes)