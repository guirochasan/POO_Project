#include <QApplication>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QTimer>
#include <QMessageBox>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QPainter>
#include <QPixmap>
#include <QDateTime>
#include <QDebug>
#include <nlohmann/json.hpp>
#include <fstream>
#include <random>
#include <set>
#include <iostream>

using json = nlohmann::json;

// Configurações - Caminhos atualizados para refletir o novo script de limpeza
const std::string KANJI_FILE = "kanji_no_romaji.json";
const std::string JLPT_LIST_FILE = "jlpt_listas.json";
const std::string SCHEDULE_FILE = "schedule.json";
const int CHECK_INTERVAL_MS = 30000;
const int DELAY_INICIAL_MS = 20000;

class KanjiApp : public QObject {
private:
    json kanjis_db;      // Contém os dados limpos (significado, leitura) de cada Kanji
    json jlpt_listas;    // Contém as listas de chaves ("一", "丁") divididas por N5, N4, etc.
    json schedule;
    QSystemTrayIcon *trayIcon;
    QTimer *clockTimer;
    
    // Nível atual selecionado pelo usuário para o estudo (Padrão: "Todos")
    std::string nivel_atual_estudo = "Todos";

    // Métodos de Persistência Atualizados
    void load_data_files() {
        // Carrega o banco de dados de Kanjis sem Romaji
        std::ifstream f_kanji(KANJI_FILE);
        if (f_kanji.is_open()) {
            f_kanji >> kanjis_db;
        } else {
            std::cerr << "Aviso: " << KANJI_FILE << " nao encontrado!\n";
        }

        // Carrega o mapeamento dos níveis do JLPT
        std::ifstream f_jlpt(JLPT_LIST_FILE);
        if (f_jlpt.is_open()) {
            f_jlpt >> jlpt_listas;
        } else {
            std::cerr << "Aviso: " << JLPT_LIST_FILE << " nao encontrado!\n";
        }
    }

    void save_kanjis() {
        std::ofstream f(KANJI_FILE);
        f << kanjis_db.dump(2);
    }

    void load_schedule() {
        QString hoje = QDate::currentDate().toString(Qt::ISODate);
        std::ifstream f(SCHEDULE_FILE);
        if (f.is_open()) {
            f >> schedule;
            if (schedule.value("date", "") == hoje.toStdString()) return;
        }
        
        schedule["date"] = hoje.toStdString();
        schedule["times"] = json::array({"10:00", "14:30", "19:00", "22:15"});
        schedule["shown"] = json::array();
        save_schedule();
    }

    void save_schedule() {
        std::ofstream f(SCHEDULE_FILE);
        f << schedule.dump(2);
    }

    QPixmap create_image() {
        QPixmap pixmap(64, 64);
        pixmap.fill(QColor(73, 109, 137));
        QPainter painter(&pixmap);
        painter.setBrush(Qt::white);
        painter.setPen(Qt::NoPen);
        painter.drawRect(16, 16, 32, 32);
        return pixmap;
    }

    // Lógica SRS Modificada para filtrar por nível JLPT selecionado
    json get_due_kanji() {
        QString agora = QDateTime::currentDateTime().toString(Qt::ISODate);
        std::vector<json> candidatas_vencidas;
        std::vector<json> todas_candidatas;

        // Criamos um set com os kanjis que pertencem ao filtro para busca rápida
        std::set<std::string> kanjis_filtrados;
        bool filtrar_por_nivel = (nivel_atual_estudo != "Todos");

        if (filtrar_por_nivel && jlpt_listas.contains(nivel_atual_estudo)) {
            for (const auto& k : jlpt_listas[nivel_atual_estudo]) {
                kanjis_filtrados.insert(k.get<std::string>());
            }
        }

        // Iterar pelo banco de dados mapeando quem obedece ao filtro do JLPT
        for (auto& [kanji_key, item] : kanjis_db.items()) {
            if (filtrar_por_nivel && kanjis_filtrados.find(kanji_key) == kanjis_filtrados.end()) {
                continue; // Pula este kanji se ele não pertence ao nível selecionado
            }

            std::string prox = item.value("proxima_revisao", "");
            if (!prox.empty() && QString::fromStdString(prox) <= agora) {
                candidatas_vencidas.push_back(item);
            }
            todas_candidatas.push_back(item);
        }

        std::random_device rd;
        std::mt19937 gen(rd());

        // Prioridade 1: Pegar uma que esteja vencida dentro do nível selecionado
        if (!candidatas_vencidas.empty()) {
            std::uniform_int_distribution<> distr(0, candidatas_vencidas.size() - 1);
            return candidatas_vencidas[distr(gen)];
        } 
        // Prioridade 2: Se nenhuma estiver vencida, traz uma aleatória do nível selecionado
        else if (!todas_candidatas.empty()) {
            std::uniform_int_distribution<> distr(0, todas_candidatas.size() - 1);
            return todas_candidatas[distr(gen)];
        }
        
        return json({});
    }

    void salvar_progresso(std::string kanji_str, int performance) {
        QDateTime agora = QDateTime::currentDateTime();
        
        if (kanjis_db.contains(kanji_str)) {
            auto& k = kanjis_db[kanji_str];
            int nivel = k.value("nivel", 0);
            QDateTime prox;

            if (performance == 0) {
                nivel = 0;
                prox = agora.addSecs(10 * 60); // 10 min
            } else if (performance == 1) {
                nivel += 1;
                prox = agora.addDays(1); // 1 dia
            } else {
                nivel += 2;
                int dias = std::max(4, (nivel + 2) * 3);
                prox = agora.addDays(dias);
            }

            k["nivel"] = nivel;
            k["proxima_revisao"] = prox.toString(Qt::ISODate).toStdString();
            save_kanjis();
        }
    }

public:
    KanjiApp(QObject *parent = nullptr) : QObject(parent) {
        load_data_files();
        load_schedule();

        // 1. Configurar Ícone da Bandeja
        trayIcon = new QSystemTrayIcon(create_image(), this);
        QMenu *trayMenu = new QMenu();
        
        QAction *verKanjiAction = new QAction("Ver Kanji Agora", this);
        connect(verKanjiAction, &QAction::triggered, this, &KanjiApp::show_flashcard);
        
        QAction *sairAction = new QAction("Sair", this);
        connect(sairAction, &QAction::triggered, qApp, &QApplication::quit);

        trayMenu->addAction(verKanjiAction);
        trayMenu->addAction(sairAction);
        trayIcon->setContextMenu(trayMenu);
        trayIcon->show();

        // 2. Temporizador Inicial (20 segundos)
        QTimer::singleShot(DELAY_INICIAL_MS, this, [this]() {
            int resposta = QMessageBox::question(nullptr, "Kanji App", 
                           "Deseja iniciar o programa de Kanjis agora?", 
                           QMessageBox::Yes | QMessageBox::No);
            if (resposta == QMessageBox::Yes) {
                show_flashcard();
            }
        });

        // 3. Temporizador Contínuo (30 segundos)
        clockTimer = new QTimer(this);
        connect(clockTimer, &QTimer::timeout, this, &KanjiApp::check_clock);
        clockTimer->start(CHECK_INTERVAL_MS);
    }

    void check_clock() {
        QString now_str = QTime::currentTime().toString("HH:mm");
        std::string now_std = now_str.toStdString();

        bool is_scheduled = false;
        for (const auto& t : schedule["times"]) {
            if (t == now_std) is_scheduled = true;
        }

        bool already_shown = false;
        for (const auto& s : schedule["shown"]) {
            if (s == now_std) already_shown = true;
        }

        if (is_scheduled && !already_shown) {
            schedule["shown"].push_back(now_std);
            save_schedule();
            show_flashcard();
        }
    }

    void show_flashcard() {
        json card = get_due_kanji();
        if(card.empty()) {
            QMessageBox::information(nullptr, "Kanji App", "Nenhum Kanji encontrado para o nivel selecionado.");
            return;
        }

        // Cria a janela do Flashcard
        QWidget *window = new QWidget;
        window->setWindowTitle("Revisão de Kanji");
        window->resize(420, 400);
        window->setAttribute(Qt::WA_DeleteOnClose);
        window->setWindowFlags(Qt::WindowStaysOnTopHint); // Sempre no topo

        QVBoxLayout *mainLayout = new QVBoxLayout(window);

        // Seletor de Nível JLPT no topo da janela de estudo
        QHBoxLayout *filterLayout = new QHBoxLayout();
        QLabel *lblFilter = new QLabel("Estudar nível:");
        QComboBox *comboFilter = new QComboBox();
        comboFilter->addItems({"Todos", "N5", "N4", "N3", "N2"});
        comboFilter->setCurrentText(QString::fromStdString(nivel_atual_estudo));
        
        filterLayout->addWidget(lblFilter);
        filterLayout->addWidget(comboFilter);
        mainLayout->addLayout(filterLayout);

        // Atualiza o escopo caso o usuário mude o dropdown durante a sessão
        connect(comboFilter, &QComboBox::currentTextChanged, [this](const QString &text){
            nivel_atual_estudo = text.toStdString();
        });

        // Kanji Grande Centralizado
        QLabel *kanjiLabel = new QLabel(QString::fromStdString(card.value("kanji", "")));
        QFont font("Arial", 75, QFont::Bold);
        kanjiLabel->setFont(font);
        kanjiLabel->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(kanjiLabel);

        // Informação Oculta (Hiragana/Katakana e Significado)
        QLabel *infoLabel = new QLabel("?");
        infoLabel->setFont(QFont("Arial", 14));
        infoLabel->setAlignment(Qt::AlignCenter);
        infoLabel->setStyleSheet("color: gray;");
        mainLayout->addWidget(infoLabel);

        // Bloco das respostas (Fica invisível até clicar em revelar)
        QWidget *respWidget = new QWidget;
        QHBoxLayout *respLayout = new QHBoxLayout(respWidget);
        
        QPushButton *btnReveal = new QPushButton("Mostrar Resposta");
        btnReveal->setMinimumHeight(45);
        mainLayout->addWidget(btnReveal);
        mainLayout->addWidget(respWidget);
        respWidget->hide();

        // Ação para exibir leituras e significados convertidos
        connect(btnReveal, &QPushButton::clicked, [=]() {
            btnReveal->hide();
            
            // Unindo as leituras em texto estruturado sem romaji
            std::string leitura_acumulada = "";
            if (card.contains("on_readings") && card["on_readings"].is_array()) {
                for (const auto& on : card["on_readings"]) leitura_acumulada += on.get<std::string>() + " ";
            }
            if (card.contains("kun_readings") && card["kun_readings"].is_array()) {
                if(!leitura_acumulada.empty()) leitura_acumulada += "/ ";
                for (const auto& kun : card["kun_readings"]) leitura_acumulada += kun.get<std::string>() + " ";
            }

            std::string significado_acumulado = "";
            if (card.contains("meanings") && card["meanings"].is_array()) {
                for (size_t i = 0; i < card["meanings"].size(); ++i) {
                    significado_acumulado += card["meanings"][i].get<std::string>();
                    if (i < card["meanings"].size() - 1) significado_acumulado += ", ";
                }
            }

            QString r_str = QString::fromStdString(leitura_acumulada);
            QString m_str = QString::fromStdString(significado_acumulado);
            
            infoLabel->setText("Leituras: " + r_str + "\nSignificado: " + m_str);
            infoLabel->setStyleSheet("color: #1a237e; font-weight: bold;");
            respWidget->show();
        });

        // Botões do SRS (Sistema de Repetição Espaçada)
        QStringList nomes = {"Não conheço", "Hesitei", "Fácil"};
        QStringList cores = {"#ffcccb", "#ffffcc", "#ccffcc"};
        
        for (int i = 0; i < 3; ++i) {
            QPushButton *btn = new QPushButton(nomes[i]);
            btn->setStyleSheet("background-color: " + cores[i] + "; padding: 12px; font-weight: bold;");
            respLayout->addWidget(btn);
            
            connect(btn, &QPushButton::clicked, [this, window, card, i]() {
                salvar_progresso(card.value("kanji", ""), i);
                window->close(); 
            });
        }

        window->show();
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false); // Garante que o trayicon segure o app aberto

    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(nullptr, "Erro fatal", "Bandeja de sistema indisponivel neste OS.");
        return 1;
    }

    KanjiApp kanjiApp;
    return app.exec();
}