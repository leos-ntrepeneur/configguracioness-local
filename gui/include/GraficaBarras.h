// GraficaBarras.h
//
// Widget generico de barras horizontales, dibujado a mano con QPainter en
// vez de depender del modulo QtCharts (que no siempre viene instalado
// junto con Qt Widgets -- ver CMakeLists.txt, solo se exige Qt6::Widgets).
// No sabe nada de ventas ni categorias: solo recibe pares
// (etiqueta, valor) ya calculados y los dibuja. Quien la usa (PestanaReporte)
// es responsable de darle datos frescos cada vez que el reporte cambia --
// por eso "automatico" en la pestana significa que actualizar() la
// repuebla junto con las demas tablas, sin un boton aparte.

#ifndef GRAFICA_BARRAS_H
#define GRAFICA_BARRAS_H

#include <QString>
#include <QWidget>
#include <vector>

class GraficaBarras : public QWidget {
    Q_OBJECT

public:
    struct Barra {
        QString etiqueta;
        double valor = 0.0;   // determina el largo relativo de la barra.
        QString valorTexto;   // texto ya formateado (ej. "$1,250.00") a mostrar junto a la barra.
    };

    explicit GraficaBarras(QWidget* padre = nullptr);

    // Reemplaza los datos y repinta. Un vector vacio muestra un mensaje en
    // vez de un area en blanco confusa.
    void setDatos(std::vector<Barra> barras);

    // Qt consulta esto para decidir cuanto espacio pedirle al layout --
    // se recalcula segun cuantas barras hay, para que agregar una
    // categoria nueva no las deje amontonadas ni deje espacio de sobra
    // con pocas.
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

protected:
    void paintEvent(QPaintEvent* evento) override;

private:
    std::vector<Barra> barras_;
};

#endif // GRAFICA_BARRAS_H
