#include "janela.hpp"
#include <QMessageBox>
#include <QString>

MainWindow::MainWindow(CardRepository& repo, QWidget* parent) 
    : QWidget(parent), repo_(repo), current_index_(0) {
    
    setWindowTitle("Random Anki - Flashcard Mode (Qt GUI)");
    resize(500, 400);

    QVBoxLayout* layout = new QVBoxLayout(this);

    lbl_status_ = new QLabel("Modo Flashcard", this);
    layout->addWidget(lbl_status_);

    text_output_ = new QTextEdit(this);
    text_output_->setReadOnly(true);
    layout->addWidget(text_output_);

    QHBoxLayout* btn_layout = new QHBoxLayout();
    btn_mostrar_resposta_ = new QPushButton("Mostrar Resposta", this);
    btn_proximo_ = new QPushButton("Próximo Card", this);
    btn_salvar_ = new QPushButton("Salvar Estado (JSON)", this);

    btn_layout->addWidget(btn_mostrar_resposta_);
    btn_layout->addWidget(btn_proximo_);
    layout->addLayout(btn_layout);
    layout->addWidget(btn_salvar_);

    // Populando dados de exemplo do projeto com significados detalhados
    deck_.add(std::make_shared<KanjiCard>("水", 1500.0f, 
        std::vector<std::string>{"água"}, 
        std::vector<std::string>{"mizu"}, 
        std::vector<std::string>{"sui"}));
        
    deck_.add(std::make_shared<VocabularyCard>("木曜日", 800.0f, "mokuyoubi", "quinta-feira"));

    for (const auto& [key, card] : deck_.items()) {
        cards_list_.push_back(card);
    }

    connect(btn_mostrar_resposta_, &QPushButton::clicked, this, &MainWindow::mostrar_resposta);
    connect(btn_proximo_, &QPushButton::clicked, this, &MainWindow::proximo_card);
    connect(btn_salvar_, &QPushButton::clicked, this, &MainWindow::salvar_estado);

    atualizar_flashcard();
}

void MainWindow::atualizar_flashcard() {
    if (cards_list_.empty()) {
        text_output_->setText("Nenhum card disponível no deck.");
        lbl_status_->setText("Deck Vazio");
        return;
    }

    if (current_index_ >= cards_list_.size()) {
        current_index_ = 0;
    }

    auto card = cards_list_[current_index_];
    QString status = QString("Card %1 de %2 | Tipo: %3 | Elo: %4")
        .arg(current_index_ + 1)
        .arg(cards_list_.size())
        .arg(QString::fromStdString(card->type_name()))
        .arg(card->elo_rating());
    
    lbl_status_->setText(status);

    QString frente = QString("<b>FRENTE DO CARD:</b><br><br><h1 align='center'>%1</h1>")
        .arg(QString::fromStdString(card->kanji()));
    text_output_->setHtml(frente);
}

void MainWindow::mostrar_resposta() {
    if (cards_list_.empty()) return;

    auto card = cards_list_[current_index_];
    QString verso = QString("<b>FRENTE:</b> %1<br><hr><br><b>VERSO / SIGNIFICADOS:</b><br>%2<br><br>Intervalo de revisão calculado: <b>%3 dias</b>")
        .arg(QString::fromStdString(card->kanji()))
        .arg(QString::fromStdString(card->details_string()))
        .arg(card->calculate_interval());
    
    text_output_->setHtml(verso);
}

void MainWindow::proximo_card() {
    if (cards_list_.empty()) return;
    current_index_++;
    if (current_index_ >= cards_list_.size()) {
        current_index_ = 0;
    }
    atualizar_flashcard();
}

void MainWindow::salvar_estado() {
    repo_.save(deck_);
    QMessageBox::information(this, "Sucesso", "Estado persistido em JSON com sucesso!");
}