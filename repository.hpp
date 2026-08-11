#pragma once
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include <future>
#include <mutex>
#include <numeric>
#include <algorithm>
#include <fstream>
#include "classes.hpp"

// Q1(A): Template Genérico restrito pelo Concept
template <Calculavel T>
class Registry {
private:
    // Q3(A): 2 Contêineres STL justificados (acesso O(1) e unicidade)
    std::unordered_map<std::string, std::shared_ptr<T>> items_;
    std::unordered_set<std::string> tags_;

public:
    void add(std::shared_ptr<T> item) {
        items_[item->kanji()] = item;
    }

    // Q2(B): optional em busca
    std::optional<std::shared_ptr<T>> buscar(const std::string& key) const {
        auto it = items_.find(key);
        if (it != items_.end()) return it->second;
        return std::nullopt;
    }

    const auto& items() const { return items_; }

    // Q3(B): Algoritmos STL + Lambda
    int contar_dificeis(float limite) const {
        return std::count_if(items_.begin(), items_.end(), [limite](const auto& pair) {
            return pair.second->elo_rating() > limite;
        });
    }

    // Q3(C e D): Paralelismo e Mutex
    float calcular_elo_medio_concorrente() const {
        if(items_.empty()) return 0.0f;
        
        std::mutex mtx;
        float soma_total = 0.0f;
        std::vector<std::future<float>> futs;

        for (const auto& [key, item] : items_) {
            futs.push_back(std::async(std::launch::async, [&item]{
                return item->elo_rating(); 
            }));
        }

        for (auto& f : futs) {
            float parcial = f.get();
            std::lock_guard lock{mtx};
            soma_total += parcial;
        }
        return soma_total / items_.size();
    }
};

// ==========================================
// Q4(C e D): DIP (Princípio da Inversão de Dependência)
// ==========================================
class CardRepository {
public:
    virtual ~CardRepository() = default;
    virtual void save(const Registry<Card>& reg) = 0;
};

// Implementação 1: Produção (Escreve em arquivo)
class JsonRepository : public CardRepository {
public:
    void save(const Registry<Card>& reg) override {
        json doc;
        doc["version"] = 1; 
        doc["itens"] = json::array();
        for (const auto& [key, card] : reg.items()) {
            if (auto k = std::dynamic_pointer_cast<KanjiCard>(card)) doc["itens"].push_back(*k);
            else if (auto v = std::dynamic_pointer_cast<VocabularyCard>(card)) doc["itens"].push_back(*v);
        }
        std::ofstream{"estado.json"} << doc.dump(2);
    }
};

// Implementação 2: Testes (Apenas em memória, sem arquivo)
class MemoryRepository : public CardRepository {
public:
    json storage_;
    void save(const Registry<Card>& reg) override {
        storage_["version"] = 1; 
        storage_["itens"] = json::array();
        for (const auto& [key, card] : reg.items()) {
            if (auto k = std::dynamic_pointer_cast<KanjiCard>(card)) storage_["itens"].push_back(*k);
            else if (auto v = std::dynamic_pointer_cast<VocabularyCard>(card)) storage_["itens"].push_back(*v);
        }
    }
};