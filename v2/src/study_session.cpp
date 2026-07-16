#include "study_session.hpp"
#include <iostream>

StudySession::StudySession(Deck* deck) : deck_(deck) {
    std::cout << "[StudySession] Sessão de estudo criada para o Deck localizado em " << deck_ << "\n";
}

StudySession::~StudySession() {
    std::cout << "[StudySession] Sessão de estudo finalizada. (Nota: O Deck referenciado não foi afetado)\n";
}

void StudySession::start_session() {
    std::cout << "[StudySession] Praticando cards na sessão atual:\n";
    if (!deck_ || deck_->cards().empty()) {
        std::cout << "  Nenhum card disponível para treinar.\n";
        return;
    }
    for (const auto& card : deck_->cards()) {
        std::cout << "  -> Treinando escrita/leitura do item: " << card->kanji() << "\n";
    }
}