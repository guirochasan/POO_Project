#include "deck.hpp"
#include <iostream>

Deck::~Deck() {
    std::cout << "[Deck] Destruindo o Deck principal... Iniciando limpeza dos Cards alocados.\n";
    // O vector de unique_ptr cuida de desalocar toda a cadeia polimórfica automaticamente!
}

void Deck::add_card(std::unique_ptr<Card> card) {
    if (card) {
        cards_.push_back(std::move(card));
    }
}

const std::vector<std::unique_ptr<Card>>& Deck::cards() const {
    return cards_;
}