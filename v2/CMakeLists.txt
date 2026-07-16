cmake_minimum_required(VERSION 3.15)
project(POO_Kanji_App LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Habilita warnings para garantir nota máxima nas avaliações de qualidade de código
add_compile_options(-Wall -Wextra -Wpedantic)

# Indica onde estão os headers (.hpp)
include_directories(include)

# Módulo para baixar bibliotecas da internet durante a configuração
include(FetchContent)

# --- DEPENDÊNCIA 1: nlohmann/json (Para ler seus dados) ---
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(json)

# --- DEPENDÊNCIA 2: Catch2 (Para a Questão 4 do TP2) ---
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.5.2
)
FetchContent_MakeAvailable(Catch2)

# Agrupamos as classes centrais do projeto (Tudo exceto a main e os testes)
set(CORE_SOURCES
    src/card.cpp
    src/kanji_card.cpp
    src/vocabulary_card.cpp
    src/deck.cpp
    src/study_session.cpp
    src/json_helper.cpp
)

# 1º Executável: O seu programa principal
add_executable(kanji_app src/main.cpp ${CORE_SOURCES})
target_link_libraries(kanji_app PRIVATE nlohmann_json::nlohmann_json)

# 2º Executável: A sua bateria de testes (Catch2)
add_executable(run_tests tests/test_tp2.cpp ${CORE_SOURCES})
# Catch2::Catch2WithMain já fornece uma função main() interna para rodar os testes
target_link_libraries(run_tests PRIVATE Catch2::Catch2WithMain nlohmann_json::nlohmann_json)

# Copia sua pasta data/ para o diretório de compilação automaticamente
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})