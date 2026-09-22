#include "TemaOscuro.h"

#include <QApplication>
#include <QFile>
#include <QPalette>
#include <QTextStream>

void aplicarTemaOscuro(QApplication& app) {
    // "Fusion" es el unico estilo base de Qt pensado para verse identico
    // (y responder bien a QSS) en cualquier sistema operativo. El estilo
    // nativo de cada SO (ej. "windowsvista" en Windows) resiste que
    // ciertos widgets -- como los botones de un QSpinBox o los
    // scrollbars -- se sobreescriban por completo, porque los dibuja la
    // propia API de temas de Windows por debajo. Fusion, en cambio,
    // dibuja todo el mismo Qt, asi que nuestra paleta y QSS mandan de
    // verdad en cada widget.
    QApplication::setStyle("Fusion");

    // QPalette: colores "de bajo nivel" que usa el motor de estilos de Qt
    // para dibujar elementos que NO pasan por las propiedades normales de
    // QSS -- las flechitas de un QSpinBox (rol ButtonText), el tono
    // atenuado de un campo deshabilitado (grupo QPalette::Disabled), el
    // color de fondo "de base" de los campos de texto antes de que nuestro
    // QSS lo sobreescriba, etc. Un tema oscuro completo en Qt Widgets
    // necesita ajustar esto ADEMAS de la hoja de estilo; si solo se hace
    // una de las dos cosas, esos detalles quedan con los colores claros
    // por defecto y se ven mal sobre fondo oscuro.
    QPalette paleta;

    const QColor fondo(30, 31, 34);          // #1e1f22
    const QColor superficie(43, 45, 49);     // #2b2d31
    const QColor superficieAlterna(49, 51, 56); // #313338
    const QColor texto(227, 229, 232);       // #e3e5e8
    const QColor textoSecundario(148, 155, 164); // #949ba4
    const QColor textoDeshabilitado(106, 110, 120); // #6a6e78
    const QColor acento(88, 101, 242);       // #5865f2

    paleta.setColor(QPalette::Window, fondo);
    paleta.setColor(QPalette::WindowText, texto);
    paleta.setColor(QPalette::Base, superficieAlterna);      // fondo "de base" de inputs.
    paleta.setColor(QPalette::AlternateBase, superficie);    // filas alternas de tablas.
    paleta.setColor(QPalette::ToolTipBase, superficie);
    paleta.setColor(QPalette::ToolTipText, texto);
    paleta.setColor(QPalette::Text, texto);
    paleta.setColor(QPalette::Button, superficieAlterna);
    paleta.setColor(QPalette::ButtonText, texto);            // <- esto es lo que faltaba
    paleta.setColor(QPalette::BrightText, QColor(237, 66, 69)); // #ed4245, para errores.
    paleta.setColor(QPalette::Link, acento);
    paleta.setColor(QPalette::Highlight, acento);
    paleta.setColor(QPalette::HighlightedText, Qt::white);

    // El "grupo" Disabled son los colores que usa CUALQUIER widget cuando
    // setEnabled(false) esta activo (ej. el campo "Codigo" al editar un
    // producto). Sin esto, Qt aplica su propia atenuacion pensada para
    // paletas claras, que sobre un fondo ya oscuro no se distingue casi
    // nada del estado habilitado.
    paleta.setColor(QPalette::Disabled, QPalette::WindowText, textoDeshabilitado);
    paleta.setColor(QPalette::Disabled, QPalette::Text, textoDeshabilitado);
    paleta.setColor(QPalette::Disabled, QPalette::ButtonText, textoDeshabilitado);
    paleta.setColor(QPalette::Disabled, QPalette::Base, fondo);

    app.setPalette(paleta);

    // El ":" al inicio de la ruta le dice a Qt "esto no es un archivo de
    // disco, busca dentro de los recursos empaquetados en el ejecutable"
    // (ver gui/resources/resources.qrc). Asi el .exe final sigue siendo
    // un solo archivo autocontenido.
    QFile archivoEstilo(":/style.qss");
    if (archivoEstilo.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream flujo(&archivoEstilo);
        app.setStyleSheet(flujo.readAll());
    }
}
