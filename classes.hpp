#pragma once
#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <nlohmann/json.hpp>
#include "core_utils.hpp"

using json = nlohmann::json;

class Card : public Counted<Card> {
protected:
    std::string kanji_;
    float elo_rating_;

public:
    Card(std::string kanji, float elo_rating) 
        : kanji_(std::move(kanji)), elo_rating_(elo_rating) {}
    virtual ~Card() = default;

    virtual float calculate_interval() const = 0;
    virtual std::string type_name() const = 0; 
    
    virtual void show() const {
        std::cout << "[" << type_name() << "] " << kanji_ << " | Elo: " << elo_rating_;
    }

    std::string kanji() const { return kanji_; }
    float elo_rating() const { return elo_rating_; }
};

class KanjiCard final : public Card {
private:
    std::vector<std::string> meanings_;
    std::vector<std::string> kun_readings_;
    std::vector<std::string> on_readings_;

public:
    KanjiCard(std::string kanji, float elo, 
              std::vector<std::string> meanings,
              std::vector<std::string> kun, 
              std::vector<std::string> on)
        : Card(std::move(kanji), elo), meanings_(std::move(meanings)),
          kun_readings_(std::move(kun)), on_readings_(std::move(on)) {}

    float calculate_interval() const override {
        if (elo_rating_ <= 0.0f) return 30.0f;
        return (1500.0f / elo_rating_) * 5.0f;
    }
    
    std::string type_name() const override { return "kanji"; }

    friend void to_json(json& j, const KanjiCard& c) {
        j = json{{"type", c.type_name()}, {"kanji", c.kanji_}, {"elo", c.elo_rating_},
                 {"meanings", c.meanings_}, {"kun", c.kun_readings_}, {"on", c.on_readings_}};
    }
};

class VocabularyCard final : public Card {
private:
    std::string reading_;     
    std::string translation_; 

public:
    VocabularyCard(std::string vocabulary, float elo_rating, 
                   std::string reading, std::string translation)
        : Card(std::move(vocabulary), elo_rating), reading_(std::move(reading)), translation_(std::move(translation)) {
            // Valida utilizando kanji_ para garantir que a string atribuída não é vazia
            if (kanji_.empty()) {
                throw invalid_card_error("Vocabulario vazio.");
            }
        }

    float calculate_interval() const override {
        if (elo_rating_ <= 0.0f) return 45.0f;
        return (2000.0f / elo_rating_) * 7.0f;
    }

    std::string type_name() const override { return "vocabulary"; }

    friend void to_json(json& j, const VocabularyCard& c) {
        j = json{{"type", c.type_name()}, {"kanji", c.kanji_}, {"elo", c.elo_rating_},
                 {"reading", c.reading_}, {"translation", c.translation_}};
    }
};