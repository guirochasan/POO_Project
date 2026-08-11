#include <QApplication>
#include "janela.hpp"
#include "repository.hpp"

int main(int argc, char* argv[]) {
    QApplication qt_app(argc, argv);

    JsonRepository repo_producao;
    MainWindow janela_principal(repo_producao);
    
    janela_principal.show();

    return qt_app.exec();
}