#pragma once
#include "deck.hpp"

class StudySession {
private:
    Deck* deck_; // Agregação: aponta para um Deck que existe fora desta classe

public:
    // Deck é passado e associado, mas seu ciclo de vida não é controlado aqui
    StudySession(Deck* deck); 
    ~StudySession(); // Destrutor não deleta o deck_

    void start_session();
};