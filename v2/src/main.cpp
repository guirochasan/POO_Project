#include <iostream>
#include <memory>
#include "kanji_card.hpp"
#include "vocabulary_card.hpp"
#include "deck.hpp"
#include "study_session.hpp"
#include "json_helper.hpp"

// Função Livre exigida na Questão 2 do TP2: Operação sobre vetor polimórfico
const Card* get_hardest_card(const std::vector<std::unique_ptr<Card>>& cards) {
    if (cards.empty()) return nullptr;
    const Card* hardest = cards[0].get();
    for (const auto& card : cards) {
        // Menor intervalo de revisão equivale a maior Elo (mais urgente/difícil)
        if (card->calculate_interval() < hardest->calculate_interval()) {
            hardest = card.get();
        }
    }
    return hardest;
}

// Passagem de interface pura por referência conforme exigido no TP2 Q3
void simulate_review_event(Reviewable& reviewable_item, bool success) {
    reviewable_item.review(success);
}

int main() {
    std::cout << "========================================================\n";
    std::cout << "         SISTEMA DE ESTUDO DE KANJI - POO UFPB          \n";
    std::cout << "========================================================\n\n";

    // 1. Criando Deck Principal (Composição)
    auto my_deck = std::make_unique<Deck>();

    // 2. Carregando dados a partir do seu JSON físico do repositório
    std::string json_file = "data/kanji_data.json";
    bool success = JsonHelper::load_cards_from_file(json_file, *my_deck);

    // Salvaguarda: Caso o arquivo JSON não seja encontrado na execução, preenche manualmente
    if (!success) {
        std::cout << "\n[Aviso] Criando dados mock para demonstração...\n";
        my_deck->add_card(std::make_unique<KanjiCard>(
            "一", 1000.0f, 
            std::vector<std::string>{"um"}, 
            std::vector<std::string>{"hito-", "hito.tsu"}, 
            std::vector<std::string>{"ichi"}
        ));
    }

    // Adiciona uma carta de vocabulário (Classe final derivada)
    my_deck->add_card(std::make_unique<VocabularyCard>(
        "一日", 1200.0f, "tsuitachi", "primeiro dia do mês"
    ));

    std::cout << "\n=== DEMONSTRANDO AGREGAÇÃO & CICLO DE VIDA (TP1 Q3) ===\n";
    {
        // Sessão depende do deck, mas o deck vive fora dela
        StudySession session(my_deck.get());
        session.start_session();
        std::cout << "--- Fim do escopo interno (Sessão será destruída) ---\n";
    }
    std::cout << "A sessão de estudos foi limpa, mas o Deck principal continua intacto!\n";

    std::cout << "\n=== DEMONSTRANDO POLIMORFISMO DINÂMICO (TP2 Q2) ===\n";
    for (const auto& card : my_deck->cards()) {
        std::cout << "--------------------------------------------------------\n";
        card->show(); // Despacho dinâmico virtual correto
    }
    std::cout << "--------------------------------------------------------\n";

    // Busca o cartão mais difícil usando a função livre
    const Card* hardest = get_hardest_card(my_deck->cards());
    if (hardest) {
        std::cout << "\n=> Item com maior dificuldade identificado:\n";
        hardest->show();
    }

    std::cout << "\n=== DEMONSTRANDO INTERFACE PURA POR REFERÊNCIA (TP2 Q3) ===\n";
    if (!my_deck->cards().empty()) {
        // Tenta converter polimorficamente o primeiro card para a interface Reviewable
        auto* reviewable = dynamic_cast<Reviewable*>(my_deck->cards()[0].get());
        if (reviewable) {
            std::cout << "Estado inicial do Kanji antes da revisão:\n";
            my_deck->cards()[0]->show();

            std::cout << "\n[Simulação] Registrando um ERRO na revisão do Kanji:\n";
            simulate_review_event(*reviewable, false); // Enviado por referência

            std::cout << "\nEstado atualizado após o erro:\n";
            my_deck->cards()[0]->show();
        }
    }

    std::cout << "\n=== DEMONSTRANDO DESTRUIÇÃO EM CADEIA (TP1 Q3 & TP2 Q2) ===\n";
    // Libera o deck principal da memória de forma segura.
    // Os logs nos destrutores provarão a liberação completa sem memory leaks.
    my_deck.reset();

    std::cout << "\n========================================================\n";
    std::cout << "             EXECUÇÃO FINALIZADA COM SUCESSO             \n";
    std::cout << "========================================================\n";

    return 0;
}