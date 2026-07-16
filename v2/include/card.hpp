#pragma once
#include <string>

// Classe base abstrata para itens de flashcard
class Card {
protected:
    std::string kanji_;
    float elo_rating_; // Sistema Elo dinâmico de dificuldade

public:
    // Construtor com lista de inicialização
    Card(std::string kanji, float elo_rating);
    virtual ~Card(); // Destrutor virtual explícito com log no terminal

    // Métodos virtuais exigidos no TP2
    virtual float calculate_interval() const = 0; // Puro
    virtual void show() const;                    // Não-puro (com default)

    // Getters constantes
    std::string kanji() const;
    float elo_rating() const;
};