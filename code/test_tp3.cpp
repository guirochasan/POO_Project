#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include "repository.hpp"

TEST_CASE("Q2: Busca com optional funciona corretamente", "[optional]") {
    Registry<Card> reg;
    reg.add(std::make_shared<VocabularyCard>("一日", 1000.0f, "tsuitachi", "primeiro dia"));
    
    REQUIRE(reg.buscar("一日").has_value());
    REQUIRE_FALSE(reg.buscar("X").has_value());
}

TEST_CASE("Q2: Excecao do dominio capturada pela base", "[excecoes]") {
    REQUIRE_THROWS_AS([](){
        VocabularyCard v("", 100.0f, "", "");
    }(), study_error); // Captura a exceção específica pela classe base
}

TEST_CASE("Q4: DIP e Serializacao sem tocar o disco", "[solid]") {
    Registry<Card> original;
    original.add(std::make_shared<KanjiCard>("一", 1000.0f, std::vector<std::string>{"um"}, std::vector<std::string>{"hito"}, std::vector<std::string>{"ichi"}));
    
    MemoryRepository repo;
    REQUIRE_NOTHROW(repo.save(original)); 
    
    // Como é MemoryRepository, não cria "estado.json", garantindo teste isolado (side-effect free)
    REQUIRE(repo.storage_["version"] == 1);
    REQUIRE(repo.storage_["itens"].size() == 1);
}