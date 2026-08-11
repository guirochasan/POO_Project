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
    virtual std::string details_string() const = 0; // Método polimórfico para os detalhes do verso
    
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

    std::string details_string() const override {
        std::string res = "<b>Significados:</b> ";
        for (size_t i = 0; i < meanings_.size(); ++i) {
            res += meanings_[i] + (i + 1 < meanings_.size() ? ", " : "");
        }
        res += "<br><b>Kun-yomi:</b> ";
        for (size_t i = 0; i < kun_readings_.size(); ++i) {
            res += kun_readings_[i] + (i + 1 < kun_readings_.size() ? ", " : "");
        }
        res += "<br><b>On-yomi:</b> ";
        for (size_t i = 0; i < on_readings_.size(); ++i) {
            res += on_readings_[i] + (i + 1 < on_readings_.size() ? ", " : "");
        }
        return res;
    }

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
            if (kanji_.empty()) {
                throw invalid_card_error("Vocabulario vazio.");
            }
        }

    float calculate_interval() const override {
        if (elo_rating_ <= 0.0f) return 45.0f;
        return (2000.0f / elo_rating_) * 7.0f;
    }

    std::string type_name() const override { return "vocabulary"; }

    std::string details_string() const override {
        return "<b>Leitura:</b> " + reading_ + "<br><b>Tradução:</b> " + translation_;
    }

    friend void to_json(json& j, const VocabularyCard& c) {
        j = json{{"type", c.type_name()}, {"kanji", c.kanji_}, {"elo", c.elo_rating_},
                 {"reading", c.reading_}, {"translation", c.translation_}};
    }
};