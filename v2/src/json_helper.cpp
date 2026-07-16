#include "json_helper.hpp"
#include "kanji_card.hpp"
#include "vocabulary_card.hpp"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

bool JsonHelper::load_cards_from_file(const std::string& filepath, Deck& deck) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[JSON Helper] Erro crítico ao abrir: " << filepath << "\n";
        return false;
    }

    try {
        json data;
        file >> data;

        if (data.is_object()) {
            for (auto& [key, val] : data.items()) {
                std::string kanji = val.value("kanji", key);
                
                // Extração dinâmica dos arrays JSON do seu repositório
                std::vector<std::string> meanings = val.value("meanings", std::vector<std::string>{});
                std::vector<std::string> kun = val.value("kun_readings", std::vector<std::string>{});
                std::vector<std::string> on = val.value("on_readings", std::vector<std::string>{});
                
                int jlpt = val.value("jlpt", 5);
                // Gera uma dificuldade dinâmica com base no nível JLPT (N5 mais fácil, N1 mais difícil)
                float calculated_elo = 1000.0f + (5 - jlpt) * 100.0f;

                // Criamos o objeto polimórfico e transferimos posse para o deck
                deck.add_card(std::make_unique<KanjiCard>(kanji, calculated_elo, meanings, kun, on));
            }
            std::cout << "[JSON Helper] Sucesso! " << data.size() << " itens importados do JSON.\n";
            return true;
        }
    } catch (const std::exception& e) {
        std::cerr << "[JSON Helper] Erro de parseamento: " << e.what() << "\n";
    }
    return false;
}