#pragma once
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QLabel>
#include <vector>
#include <memory>
#include "repository.hpp"

class MainWindow : public QWidget {
    Q_OBJECT
private:
    CardRepository& repo_;
    Registry<Card> deck_;
    std::vector<std::shared_ptr<Card>> cards_list_;
    size_t current_index_;

    QLabel* lbl_status_;
    QTextEdit* text_output_;
    QPushButton* btn_mostrar_resposta_;
    QPushButton* btn_proximo_;
    QPushButton* btn_salvar_;

public:
    explicit MainWindow(CardRepository& repo, QWidget* parent = nullptr);

private slots:
    void proximo_card();
    void mostrar_resposta();
    void salvar_estado();
    void atualizar_flashcard();
};