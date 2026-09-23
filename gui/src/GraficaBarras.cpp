#include "GraficaBarras.h"

#include <QColor>
#include <QFontMetrics>
#include <QLinearGradient>
#include <QPainter>
#include <QPaintEvent>
#include <algorithm>

namespace {
constexpr int ALTO_FILA = 32;
constexpr int MARGEN = 8;
constexpr int ANCHO_ETIQUETA = 150;
constexpr int ANCHO_VALOR = 90;
} // namespace

GraficaBarras::GraficaBarras(QWidget* padre) : QWidget(padre) {}

void GraficaBarras::setDatos(std::vector<Barra> barras) {
    barras_ = std::move(barras);
    // updateGeometry() le avisa al layout que contiene este widget que
    // sizeHint() pudo haber cambiado (ej. de 2 a 5 categorias); sin esto
    // el layout seguiria usando el tamaño calculado la primera vez.
    updateGeometry();
    update(); // pide un repintado con los datos nuevos.
}

QSize GraficaBarras::sizeHint() const {
    int alto = MARGEN * 2 + static_cast<int>(barras_.size()) * ALTO_FILA;
    return QSize(400, std::max(alto, ALTO_FILA + MARGEN * 2));
}

QSize GraficaBarras::minimumSizeHint() const {
    return sizeHint();
}

void GraficaBarras::paintEvent(QPaintEvent*) {
    QPainter pintor(this);
    pintor.setRenderHint(QPainter::Antialiasing);

    if (barras_.empty()) {
        pintor.setPen(QColor("#949ba4"));
        pintor.drawText(rect(), Qt::AlignCenter, "Sin ventas registradas hoy.");
        return;
    }

    double mayorValor = 0.0;
    for (const Barra& b : barras_) {
        mayorValor = std::max(mayorValor, b.valor);
    }

    // Lo que sobra del ancho del widget despues de la columna de etiquetas
    // y la de valores es el espacio real disponible para las barras --
    // se recalcula en cada pintado porque el usuario puede redimensionar
    // la ventana.
    int anchoDisponible = width() - ANCHO_ETIQUETA - ANCHO_VALOR - MARGEN * 3;
    anchoDisponible = std::max(anchoDisponible, 20);

    int y = MARGEN;
    QFontMetrics metricas(font());
    for (const Barra& b : barras_) {
        // Etiqueta (nombre de categoria), a la izquierda. elidedText corta
        // con "..." si el nombre no cabe, en vez de desbordar la columna.
        pintor.setPen(QColor("#e3e5e8"));
        QRect rectoEtiqueta(MARGEN, y, ANCHO_ETIQUETA, ALTO_FILA);
        pintor.drawText(rectoEtiqueta, Qt::AlignVCenter | Qt::AlignLeft,
                         metricas.elidedText(b.etiqueta, Qt::ElideRight, ANCHO_ETIQUETA - 4));

        // Barra: largo proporcional al valor mas grande del conjunto (esa
        // categoria pinta la barra completa, las demas se escalan contra
        // ella) -- mismo criterio que la version ASCII de la consola.
        int largoBarra = mayorValor > 0.0
                              ? static_cast<int>((b.valor / mayorValor) * anchoDisponible)
                              : 0;
        // Una categoria con ventas (aunque sean chicas) siempre pinta un
        // trocito de barra -- si no, se veria identica a "sin ventas".
        if (largoBarra < 3 && b.valor > 0.0) {
            largoBarra = 3;
        }

        int altoBarra = ALTO_FILA - 12;
        QRect rectoBarra(MARGEN + ANCHO_ETIQUETA, y + 6, std::max(largoBarra, 1), altoBarra);
        QLinearGradient degradado(rectoBarra.topLeft(), rectoBarra.topRight());
        degradado.setColorAt(0, QColor("#0d9488"));
        degradado.setColorAt(1, QColor("#2dd4bf"));
        pintor.setPen(Qt::NoPen);
        pintor.setBrush(degradado);
        pintor.drawRoundedRect(rectoBarra, 4, 4);

        // Valor en texto, despues del espacio reservado para la barra --
        // en una columna fija, no pegado al extremo de la barra, para que
        // todos los valores queden alineados entre si sin importar el
        // largo de cada barra.
        pintor.setPen(QColor("#e3e5e8"));
        QRect rectoValor(MARGEN + ANCHO_ETIQUETA + anchoDisponible + MARGEN, y, ANCHO_VALOR, ALTO_FILA);
        pintor.drawText(rectoValor, Qt::AlignVCenter | Qt::AlignLeft, b.valorTexto);

        y += ALTO_FILA;
    }
}
