#pragma once
#include "card.hpp"
#include <vector>
#include <memory>

class Deck {
private:
    // Composição: Deck é dono absoluto dos Cards via unique_ptr
    std::vector<std::unique_ptr<Card>> cards_; 

public:
    Deck() = default;
    ~Deck(); // Imprime mensagem indicando a destruição de cada Card contido

    void add_card(std::unique_ptr<Card> card);
    const std::vector<std::unique_ptr<Card>>& cards() const;
};