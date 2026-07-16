#pragma once
#include "deck.hpp"
#include <string>

class JsonHelper {
public:
    // Carrega cartões polimorficamente a partir do seu arquivo JSON de dados
    static bool load_cards_from_file(const std::string& filepath, Deck& deck);
};