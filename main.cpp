#include <iostream>
#include <variant>
#include <ranges>
#include "repository.hpp"

// Q2(C): variant
using ParseResult = std::variant<std::shared_ptr<Card>, std::string>;

ParseResult parse_entrada(const std::string& entrada) {
    if (entrada.empty()) return "Erro: Entrada invalida!";
    return std::make_shared<VocabularyCard>(entrada, 1000.0f, "kana", "traducao");
}

class App {
    CardRepository& repo_; // Q4(C): Dependência injetada
public:
    explicit App(CardRepository& r) : repo_{r} {}
    
    void executar() {
        std::cout << "=== TP3 POO UFPB ===\n\n";
        Registry<Card> my_deck;

        // Q2(D): Try/Catch capturando exceção base
        try {
            my_deck.add(std::make_shared<VocabularyCard>("", 100.0f, "", ""));
        } catch (const study_error& e) {
            std::cout << "[Excecao] Capturada pela base: " << e.what() << '\n';
        }

        my_deck.add(std::make_shared<KanjiCard>("水", 1500.0f, std::vector<std::string>{"água"}, std::vector<std::string>{"mizu"}, std::vector<std::string>{"sui"}));
        my_deck.add(std::make_shared<VocabularyCard>("木曜日", 800.0f, "mokuyoubi", "quinta-feira"));
        my_deck.add(std::make_shared<VocabularyCard>("一日", 1100.0f, "tsuitachi", "primeiro dia"));

        // Q1(E): Pipeline de Ranges C++20
        std::cout << "\n[Ranges] Cards dificeis (Elo > 1000):\n";
        auto dificeis = my_deck.items() 
            | std::views::values 
            | std::views::filter([](const auto& c) { return c->elo_rating() > 1000.0f; })
            | std::views::transform([](const auto& c) { return c->kanji(); });
        
        for (const auto& nome : dificeis) {
            std::cout << " -> " << nome << '\n';
        }

        // Q2(B): Optional
        std::cout << "\n[Optional] Busca por '水': " << (my_deck.buscar("水").has_value() ? "Achou" : "Falhou") << '\n';
        std::cout << "[Optional] Busca por 'X': " << (my_deck.buscar("X").has_value() ? "Achou" : "Falhou") << '\n';

        // Q2(C): Visit no Variant
        std::cout << "\n[Variant] Teste de Parse:\n";
        ParseResult resultado = parse_entrada("");
        std::visit([](const auto& x) {
            using T = std::decay_t<decltype(x)>;
            if constexpr (std::is_same_v<T, std::string>) std::cout << " Erro tratado: " << x << '\n';
            else std::cout << " Card gerado: " << x->kanji() << '\n';
        }, resultado);

        // Q3(B e C): Algoritmos e Threads
        std::cout << "\n[STL] Cards com Elo > 1000: " << my_deck.contar_dificeis(1000.0f) << '\n';
        std::cout << "[Threads] Elo medio calculado via async: " << my_deck.calcular_elo_medio_concorrente() << '\n';
        
        // Q4(D): DIP - Executa a interface sem saber se é Json ou Memory
        repo_.save(my_deck);
        std::cout << "\n[DIP] Estado salvo no repositorio configurado.\n";
    }
};

int main() {
    // Para testar a injeção, instanciamos o JsonRepository (Produção)
    JsonRepository repo_producao;
    App core(repo_producao);
    core.executar();
    
    // Q1(B): Verificando CRTP
    std::cout << "\nCards vivos na memoria (CRTP): " << Card::alive() << '\n';
    return 0;
}