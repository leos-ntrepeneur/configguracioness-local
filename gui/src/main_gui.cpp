// main_gui.cpp
//
// Punto de entrada de la version grafica. QApplication es el "motor" de
// Qt: administra el bucle de eventos (procesar clics, teclas, repintar
// ventanas) igual que, en la consola, nuestro Menu::ejecutar() tenia su
// propio bucle `while` leyendo con std::cin. app.exec() bloquea aqui hasta
// que el usuario cierra la ventana principal.

#include <QApplication>

#include "MainWindow.h"
#include "TemaOscuro.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    aplicarTemaOscuro(app);

    MainWindow ventana;
    ventana.show();

    return app.exec();
}
