#include <iostream>
#include <memory> 
#include "class.cpp" 


int main() {
    std::cout << "--- INÍCIO DO PROGRAMA ---\n";

    // Q2(D): Instanciar pelo menos um objeto de cada classe no main()
    // Q3(D): Criar os objetos que serão usados nas relações
    auto meu_deck = std::make_shared<Deck>("N5 Básico");
    auto aluno = std::make_shared<User>("Guilherme");

    // Aciona Q3(A): Composição - Deck criando Kanjis internamente
    meu_deck->add_kanji("日");
    meu_deck->add_kanji("月");

    std::cout << "\n--- ABRINDO ESCOPO DA SESSÃO DE ESTUDO ---\n";
    {
        // Q3(D): Demonstração das relações (Agregação ocorre aqui)
        StudySession sessao(meu_deck, aluno);
        
        // Q2(D): Demonstrar pelo menos um método com lógica real executando
        sessao.rodar_sessao();
        
    } // Q3(D): Mostra o que acontece quando o dono da agregação (sessao) é destruído
    
    std::cout << "--- SAIU DO ESCOPO DA SESSÃO ---\n\n";

    // Q3(D) e Q3(B): Prova visual no terminal de que os objetos agregados sobrevivem
    std::cout << "Prova de agregação: O usuário '" << aluno->get_name() 
              << "' e o baralho '" << meu_deck->get_name() << "' continuam vivos!\n";

    std::cout << "\n--- FIM DO MAIN ---\n";
    return 0; 
    // Q3(D) e Q3(A): Ao final do programa, 'meu_deck' é destruído.
    // Como é uma composição, o terminal vai mostrar que os objetos 'Kanji'
    // serão destruídos logo em seguida, refletindo o ciclo de vida.
}