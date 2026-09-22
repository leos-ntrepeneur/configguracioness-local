// main_gui.cpp
//
// Punto de entrada de la version grafica. QApplication es el "motor" de
// Qt: administra el bucle de eventos (procesar clics, teclas, repintar
// ventanas) igual que, en la consola, nuestro Menu::ejecutar() tenia su
// propio bucle `while` leyendo con std::cin. app.exec() bloquea aqui hasta
// que el usuario cierra la ventana principal.

#include <QApplication>
#include <QFile>
#include <QTextStream>

#include "MainWindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // "Fusion" es el unico estilo base de Qt pensado para verse identico
    // (y responder bien a QSS) en cualquier sistema operativo. El estilo
    // nativo de cada SO (ej. "windowsvista" en Windows) resiste que
    // ciertos widgets -- como QComboBox o los scrollbars -- se
    // sobreescriban por completo con una hoja de estilo, porque los
    // dibuja la propia API de temas de Windows por debajo. Fusion, en
    // cambio, dibuja todo el mismo Qt, asi que nuestro QSS manda de
    // verdad en cada widget.
    QApplication::setStyle("Fusion");

    // El ":" al inicio de la ruta le dice a Qt "esto no es un archivo de
    // disco, busca dentro de los recursos empaquetados en el ejecutable"
    // (ver gui/resources/resources.qrc). Asi el .exe final sigue siendo
    // un solo archivo autocontenido, sin un style.qss suelto que se
    // pueda perder u olvidar al copiar el programa a otra maquina.
    QFile archivoEstilo(":/style.qss");
    if (archivoEstilo.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream flujo(&archivoEstilo);
        app.setStyleSheet(flujo.readAll());
    }

    MainWindow ventana;
    ventana.show();

    return app.exec();
}
