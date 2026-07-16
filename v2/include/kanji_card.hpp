#pragma once
#include "card.hpp"
#include "reviewable.hpp"
#include <vector>

// Herança múltipla segura (sem diamante) de Card e Reviewable
class KanjiCard final : public Card, public Reviewable {
private:
    std::vector<std::string> meanings_;
    std::vector<std::string> kun_readings_;
    std::vector<std::string> on_readings_;

public:
    KanjiCard(std::string kanji, float elo, 
              std::vector<std::string> meanings,
              std::vector<std::string> kun, 
              std::vector<std::string> on);
    ~KanjiCard() override;

    // Métodos obrigatórios sobrescritos
    float calculate_interval() const override; 
    void show() const override;      // Irá chamar Card::show() internamente
    void review(bool success) override;
};