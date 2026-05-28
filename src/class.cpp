#include <iostream>
#include <string>
#include <vector>
#include <memory> 

// Coloquei comwntários indicando quais linhas respondem quais questões pra facilitar sua correção :D

// Q2(A): Pelo menos 4 classes modelando o domínio (1/4: Kanji)
class Kanji {
private:
    std::string literal_; // Q2(A): Membro privado com sufixo '_'

public:
    // Q2(A): Construtor com lista de inicialização
    Kanji(std::string literal) : literal_(literal) {
        std::cout << "  -> Kanji ('" << literal_ << "') criado.\n";
    }
    
    // Q2(B): Destrutor explícito que produza efeito observável (imprime mensagem)
    ~Kanji() {
        std::cout << "  -> ~Kanji ('" << literal_ << "') destruído.\n";
    }
    
    // Q2(A): Getter const para atributos que precisam ser lidos
    std::string get_literal() const { return literal_; }
};

// Q2(A): Pelo menos 4 classes (2/4: Deck)
class Deck {
private:
    std::string name_; // Q2(A): Membro privado com sufixo '_'
    
    // Q3(A): Composição - A classe dona (Deck) possui os objetos dependentes (Kanji).
    // Q4(A): Substituição de raw pointer por unique_ptr (posse exclusiva).
    std::vector<std::unique_ptr<Kanji>> kanjis_;

public:
    // Q2(A): Construtor com lista de inicialização
    Deck(std::string name) : name_(name) {
        std::cout << "Deck ('" << name_ << "') criado.\n";
    }
    
    // Q2(B): Destrutor explícito observável
    // Q4(A): Destrutor manual eliminado (não precisa de 'delete', unique_ptr cuida disso)
    ~Deck() {
        std::cout << "~Deck ('" << name_ << "') destruído. Seus kanjis irão junto:\n";
        // Q3(A): O dependente não existe sem o dono, destruído junto (automático pelo unique_ptr).
    }
    
    // Q3(A): Na composição, a classe dona cria o objeto dependente (neste método)
    void add_kanji(std::string literal) {
        kanjis_.push_back(std::make_unique<Kanji>(literal));
    }
    
    std::string get_name() const { return name_; } // Q2(A): Getter const
    
    // Q2(C): Pelo menos um método por classe com lógica real
    void listar_kanjis() const {
        std::cout << "Baralho '" << name_ << "' contém: ";
        for (const auto& k : kanjis_) {
            std::cout << k->get_literal() << " ";
        }
        std::cout << "\n";
    }
};

// Q2(A): Pelo menos 4 classes (3/4: User)
class User {
private:
    std::string name_; // Q2(A): Membro privado com sufixo '_'

public:
    // Q2(A): Construtor com lista de inicialização
    User(std::string name) : name_(name) {
        std::cout << "User ('" << name_ << "') criado.\n";
    }
    
    ~User() { // Q2(B): Destrutor observável
        std::cout << "~User ('" << name_ << "') destruído.\n";
    }
    
    std::string get_name() const { return name_; } // Q2(A): Getter const
    
    // Q2(C): Método com lógica real
    void estudar() const { std::cout << name_ << " está focado estudando!\n"; }
};

// Q2(A): Pelo menos 4 classes (4/4: StudySession)
class StudySession {
private:
    // Q3(B): Agregação - A classe principal referencia objetos independentes.
    // Q4(A): Substituição de raw pointer por shared_ptr (observador sem posse).
    std::shared_ptr<Deck> deck_ref_;
    std::shared_ptr<User> user_ref_;

public:
    // Q2(A): Construtor com lista de inicialização
    StudySession(std::shared_ptr<Deck> deck, std::shared_ptr<User> user)
        : deck_ref_(deck), user_ref_(user) {
        std::cout << "StudySession iniciada para '" << user_ref_->get_name()
                  << "' com o baralho '" << deck_ref_->get_name() << "'.\n";
    }
    
    // Q2(B): Destrutor observável
    // Q3(B): O destrutor não deve deletar os objetos agregados (shared_ptr garante isso).
    ~StudySession() {
        std::cout << "~StudySession encerrada. (Deck e User não são afetados)\n";
    }
    
    // Q2(C): Método com lógica real
    void rodar_sessao() const {
        user_ref_->estudar();
        deck_ref_->listar_kanjis();
    }
};

