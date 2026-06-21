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
#include <QPainter>
#include <QPixmap>
#include <QDateTime>
#include <QDebug>
#include <nlohmann/json.hpp>
#include <fstream>
#include <random>
#include <iostream>

using json = nlohmann::json;

// Configurações
const std::string KANJI_FILE = "kanjis1.json";
const std::string SCHEDULE_FILE = "schedule.json";
const int CHECK_INTERVAL_MS = 30000;
const int DELAY_INICIAL_MS = 20000;

class KanjiApp : public QObject {
private:
    json kanjis;
    json schedule;
    QSystemTrayIcon *trayIcon;
    QTimer *clockTimer;

    // Métodos de Persistência
    json load_kanjis() {
        std::ifstream f(KANJI_FILE);
        if (!f.is_open()) return json::array();
        json data;
        f >> data;
        return data;
    }

    void save_kanjis() {
        std::ofstream f(KANJI_FILE);
        f << kanjis.dump(2);
    }

    void load_schedule() {
        QString hoje = QDate::currentDate().toString(Qt::ISODate);
        std::ifstream f(SCHEDULE_FILE);
        if (f.is_open()) {
            f >> schedule;
            if (schedule.value("date", "") == hoje.toStdString()) return;
        }
        
        // Gera agendamento simples se for dia novo
        schedule["date"] = hoje.toStdString();
        schedule["times"] = json::array({"10:00", "14:30", "19:00", "22:15"}); // Adapte o gerador aqui
        schedule["shown"] = json::array();
        save_schedule();
    }

    void save_schedule() {
        std::ofstream f(SCHEDULE_FILE);
        f << schedule.dump(2);
    }

    // Cria o ícone dinamicamente igual ao PIL.Image do Python
    QPixmap create_image() {
        QPixmap pixmap(64, 64);
        pixmap.fill(QColor(73, 109, 137));
        QPainter painter(&pixmap);
        painter.setBrush(Qt::white);
        painter.setPen(Qt::NoPen);
        painter.drawRect(16, 16, 32, 32); // Quadrado central
        return pixmap;
    }

    // Lógica SRS (Pegar Kanji)
    json get_due_kanji() {
        QString agora = QDateTime::currentDateTime().toString(Qt::ISODate);
        std::vector<json> vencidos;

        for (const auto& k : kanjis) {
            std::string prox = k.value("proxima_revisao", "");
            if (QString::fromStdString(prox) <= agora) {
                vencidos.push_back(k);
            }
        }

        std::random_device rd;
        std::mt19937 gen(rd());

        if (!vencidos.empty()) {
            std::uniform_int_distribution<> distr(0, vencidos.size() - 1);
            return vencidos[distr(gen)];
        } else if (!kanjis.empty()) {
            std::uniform_int_distribution<> distr(0, kanjis.size() - 1);
            return kanjis[distr(gen)];
        }
        return json({});
    }

    void salvar_progresso(std::string kanji_str, int performance) {
        QDateTime agora = QDateTime::currentDateTime();
        
        for (auto& k : kanjis) {
            if (k["kanji"] == kanji_str) {
                int nivel = k.value("nivel", 0);
                QDateTime prox;

                if (performance == 0) {
                    nivel = 0;
                    prox = agora.addSecs(10 * 60); // 10 minutos
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
                break;
            }
        }
        save_kanjis();
    }

public:
    KanjiApp(QObject *parent = nullptr) : QObject(parent) {
        kanjis = load_kanjis();
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

        // 2. Temporizador Inicial (Substitui o delay inicial de 20s)
        QTimer::singleShot(DELAY_INICIAL_MS, this, [this]() {
            int resposta = QMessageBox::question(nullptr, "Kanji App", 
                           "Deseja iniciar o programa de Kanjis agora?", 
                           QMessageBox::Yes | QMessageBox::No);
            if (resposta == QMessageBox::Yes) {
                show_flashcard();
            }
        });

        // 3. Temporizador Contínuo (Relógio a cada 30s)
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
        if(card.empty()) return;

        // Cria a janela do Flashcard
        QWidget *window = new QWidget;
        window->setWindowTitle("Revisão de Kanji");
        window->resize(400, 350);
        window->setAttribute(Qt::WA_DeleteOnClose); // Evita memory leak ao fechar
        window->setWindowFlags(Qt::WindowStaysOnTopHint); // Sempre no topo (Topmost)

        QVBoxLayout *mainLayout = new QVBoxLayout(window);

        // Kanji Grande
        QLabel *kanjiLabel = new QLabel(QString::fromStdString(card.value("kanji", "")));
        QFont font("Arial", 70, QFont::Bold);
        kanjiLabel->setFont(font);
        kanjiLabel->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(kanjiLabel);

        // Informação Oculta
        QLabel *infoLabel = new QLabel("?");
        infoLabel->setFont(QFont("Arial", 14));
        infoLabel->setAlignment(Qt::AlignCenter);
        infoLabel->setStyleSheet("color: gray;");
        mainLayout->addWidget(infoLabel);

        // Frame das respostas
        QWidget *respWidget = new QWidget;
        QHBoxLayout *respLayout = new QHBoxLayout(respWidget);
        
        QPushButton *btnReveal = new QPushButton("Mostrar Resposta");
        btnReveal->setMinimumHeight(50);
        mainLayout->addWidget(btnReveal);
        
        mainLayout->addWidget(respWidget);
        respWidget->hide(); // Escondido no início

        // O que acontece ao clicar em "Mostrar Resposta"
        connect(btnReveal, &QPushButton::clicked, [=]() {
            btnReveal->hide();
            QString reading = QString::fromStdString(card.value("leitura", "---"));
            QString meaning = QString::fromStdString(card.value("significado", "---"));
            infoLabel->setText(reading + "\n" + meaning);
            infoLabel->setStyleSheet("color: blue;");
            respWidget->show();
        });

        // Botões de Performance
        QStringList nomes = {"Não conheço", "Hesitei", "Fácil"};
        QStringList cores = {"#ffcccb", "#ffffcc", "#ccffcc"};
        
        for (int i = 0; i < 3; ++i) {
            QPushButton *btn = new QPushButton(nomes[i]);
            btn->setStyleSheet("background-color: " + cores[i] + "; padding: 15px;");
            respLayout->addWidget(btn);
            
            connect(btn, &QPushButton::clicked, [this, window, card, i]() {
                salvar_progresso(card.value("kanji", ""), i);
                window->close(); // Fecha a janela após responder
            });
        }

        window->show();
    }
};

int main(int argc, char *argv[]) {
    // QApplication é o equivalente ao tk.Tk() no C++
    // Ele gerencia a interface gráfica e os eventos do sistema
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false); // Mantém o app vivo mesmo se a janela de flashcard fechar

    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(nullptr, "Erro", "Bandeja de sistema não detectada.");
        return 1;
    }

    KanjiApp kanjiApp;

    // Equivalente ao root.mainloop()
    return app.exec();
}