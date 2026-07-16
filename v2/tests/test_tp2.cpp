#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>      // <-- Esta é a linha que faltava!
#include <memory>
#include <vector>

// Inclusão dos cabeçalhos do projeto
#include "../include/card.hpp"
#include "../include/kanji_card.hpp"
#include "../include/vocabulary_card.hpp"
#include "../include/reviewable.hpp"

// TEST_CASE 1 (Q1): Construtores e destrutores da hierarquia[cite: 2]
// Verifica se os objetos são alocados e desalocados corretamente (evitando leaks).
TEST_CASE("Verificando construtores e destrutores da hierarquia", "[Q1]") {
    // Aloca uma classe derivada (KanjiCard) apontada por um ponteiro da classe Base (Card)
    std::unique_ptr<Card> my_card = std::make_unique<KanjiCard>(
        "水", 1000.0f, 
        std::vector<std::string>{"água"}, 
        std::vector<std::string>{"mizu"}, 
        std::vector<std::string>{"sui"}
    );
    
    // Testa se os dados base foram inicializados corretamente
    REQUIRE(my_card->kanji() == "水");
    REQUIRE(my_card->elo_rating() == Catch::Approx(1000.0f));

    // Ao fim deste bloco, o destrutor de 'my_card' será chamado. 
    // Como o destrutor da classe base Card é 'virtual', isso acionará a destruição 
    // em cadeia (KanjiCard -> Card), impedindo memory leaks das std::vector[cite: 2].
}

// TEST_CASE 2 (Q2): Polimorfismo[cite: 2]
// Cria derivadas via unique_ptr<Base>, chama calcular() e verifica o retorno.
TEST_CASE("Polimorfismo dinâmico nos intervalos de revisão", "[Q2]") {
    std::vector<std::unique_ptr<Card>> cards;
    
    // Adiciona KanjiCard (Dificuldade/Elo base de 1000)
    cards.push_back(std::make_unique<KanjiCard>(
        "木", 1000.0f, 
        std::vector<std::string>{"árvore"}, 
        std::vector<std::string>{"ki"}, 
        std::vector<std::string>{"moku"}
    ));
        
    // Adiciona VocabularyCard (Dificuldade/Elo base de 1000)
    cards.push_back(std::make_unique<VocabularyCard>(
        "木曜日", 1000.0f, "mokuyoubi", "quinta-feira"
    ));
        
    // Fórmula do KanjiCard: (1500 / 1000) * 5 = 7.5[cite: 2]
    REQUIRE(cards[0]->calculate_interval() == Catch::Approx(7.5f));
    
    // Fórmula do VocabularyCard: (2000 / 1000) * 7 = 14.0[cite: 2]
    REQUIRE(cards[1]->calculate_interval() == Catch::Approx(14.0f));
}

// TEST_CASE 3 (Q3): Interface pura[cite: 2]
// Cria objeto que implementa a interface e passa por referência para verificar comportamento.
TEST_CASE("Uso da interface pura Reviewable", "[Q3]") {
    // Instancia a classe concreta
    KanjiCard kanji_fogo("火", 1000.0f, {"fogo"}, {"hi"}, {"ka"});
    
    // Passa por referência conhecendo apenas a Interface
    Reviewable& interface_ref = kanji_fogo;
    
    // Simula um acerto (success = true). A lógica em KanjiCard reduz o Elo em 60.
    interface_ref.review(true);
    REQUIRE(kanji_fogo.elo_rating() == Catch::Approx(940.0f));
    
    // Simula um erro (success = false). A lógica em KanjiCard aumenta o Elo em 100.
    interface_ref.review(false);
    REQUIRE(kanji_fogo.elo_rating() == Catch::Approx(1040.0f));
}