#include "card.hpp"
#include <iostream>
#include <utility>

// Construtor usa lista de inicialização conforme padrão TP1 Q2
Card::Card(std::string kanji, float elo_rating)
    : kanji_(std::move(kanji)), elo_rating_(elo_rating) {
    std::cout << "[Card] Criado cartão base para: " << kanji_ << " (Elo: " << elo_rating_ << ")\n";
}

// Destrutor virtual explícito produzindo logs observáveis no terminal
Card::~Card() {
    std::cout << "[Card] Destruindo Card base para: " << kanji_ << "\n";
}

// Método virtual comum (não-puro) com implementação padrão
void Card::show() const {
    std::cout << "Cartão [" << kanji_ << "] | Dificuldade (Elo): " << elo_rating_;
}

// Getters constantes
std::string Card::kanji() const { return kanji_; }
float Card::elo_rating() const { return elo_rating_; }