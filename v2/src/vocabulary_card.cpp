#include "vocabulary_card.hpp"
#include <iostream>
#include <utility>

VocabularyCard::VocabularyCard(std::string vocabulary, float elo_rating, 
                               std::string reading, std::string translation)
    : Card(std::move(vocabulary), elo_rating), 
      reading_(std::move(reading)), 
      translation_(std::move(translation)) {
    std::cout << "[VocabularyCard] Criado: " << kanji_ << "\n";
}

VocabularyCard::~VocabularyCard() {
    std::cout << "[VocabularyCard] Destruindo VocabularyCard: " << kanji_ << "\n";
}

// Lógica de intervalo adaptada para retenção de vocabulários longos
float VocabularyCard::calculate_interval() const {
    if (elo_rating_ <= 0.0f) return 45.0f;
    return (2000.0f / elo_rating_) * 7.0f; // Dá mais folga de estudo
}

void VocabularyCard::show() const {
    Card::show();
    std::cout << "\n  Leitura (Kana): " << reading_ 
              << "\n  Tradução: " << translation_
              << "\n  Revisar em: " << calculate_interval() << " dias.\n";
}