#pragma once
#include "card.hpp"
#include <string>

// Classe concreta para cartões de vocabulário.
// Marcada como 'final' para cumprir a Questão 3 do TP2.
class VocabularyCard final : public Card {
private:
    std::string reading_;     // Leitura em romaji/kana (ex: tsuitachi)
    std::string translation_; // Tradução (ex: primeiro dia do mês)

public:
    // Construtor usando lista de inicialização
    VocabularyCard(std::string vocabulary, float elo_rating, 
                   std::string reading, std::string translation);
    
    // Destrutor explícito observável
    ~VocabularyCard() override;

    // Sobrescrita obrigatória dos métodos virtuais da base
    float calculate_interval() const override;
    void show() const override;
};