#include "kanji_card.hpp"
#include <iostream>
#include <utility>

KanjiCard::KanjiCard(std::string kanji, float elo, 
                     std::vector<std::string> meanings,
                     std::vector<std::string> kun, 
                     std::vector<std::string> on)
    : Card(std::move(kanji), elo), 
      meanings_(std::move(meanings)), 
      kun_readings_(std::move(kun)), 
      on_readings_(std::move(on)) {
    std::cout << "[KanjiCard] Criado: " << kanji_ << "\n";
}

KanjiCard::~KanjiCard() {
    std::cout << "[KanjiCard] Destruindo KanjiCard: " << kanji_ << "\n";
}

// Lógica Real: Quanto mais difícil o Elo, menor o tempo para revisão
float KanjiCard::calculate_interval() const {
    if (elo_rating_ <= 0.0f) return 30.0f; // Caso limite para evitar divisões por zero
    return (1500.0f / elo_rating_) * 5.0f; 
}

// Sobrescreve chamando a implementação base primeiro (Base::metodo())
void KanjiCard::show() const {
    Card::show(); // Chamada da implementação base (TP2 Q1)
    std::cout << "\n  Significados: ";
    for (const auto& m : meanings_) std::cout << "[" << m << "] ";
    std::cout << "\n  Kun-yomi: ";
    for (const auto& k : kun_readings_) std::cout << k << " ";
    std::cout << "\n  On-yomi: ";
    for (const auto& o : on_readings_) std::cout << o << " ";
    std::cout << "\n  Revisar em: " << calculate_interval() << " dias.\n";
}

// Lógica de revisão da interface pura (sistema de Elo adaptativo)
void KanjiCard::review(bool success) {
    if (success) {
        elo_rating_ -= 60.0f; // Facilita o card
        if (elo_rating_ < 600.0f) elo_rating_ = 600.0f;
        std::cout << "[Revisão] Acerto! Novo Elo do Kanji '" << kanji_ << "': " << elo_rating_ << "\n";
    } else {
        elo_rating_ += 100.0f; // Dificulta o card
        std::cout << "[Revisão] Erro! Novo Elo do Kanji '" << kanji_ << "': " << elo_rating_ << "\n";
    }
}