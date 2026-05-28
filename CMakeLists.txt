cmake_minimum_required(VERSION 3.10)
project(KanjiStudySystem LANGUAGES CXX)

# Exigência do trabalho: Configurar para C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Alerta o compilador para ser rigoroso (Boas práticas)
add_compile_options(-Wall -Wextra)

# --- REQUISITO DO TESTE 7 (AddressSanitizer) ---
# Se compilado em modo Debug, ativa a sanitização de memória contra memory leaks
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Modo Debug detetado: Ativando AddressSanitizer e UndefinedBehaviorSanitizer")
    add_compile_options(-fsanitize=address,undefined)
    add_link_options(-fsanitize=address,undefined)
endif()

# Define o executável e onde o código fonte principal está
add_executable(meu-projeto src/main.cpp)