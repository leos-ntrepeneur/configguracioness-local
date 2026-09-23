// PestanaReporte.h
//
// Pestana "Reporte del dia": totales y ranking de productos mas vendidos.
// A diferencia de Productos/Ventas, esta pestana no modifica nada -- solo
// LEE, asi que no emite datosModificados() ni necesita botones de accion,
// nada mas un boton "Actualizar" por si el usuario quiere refrescar sin
// cambiar de pestana.

#ifndef PESTANA_REPORTE_H
#define PESTANA_REPORTE_H

#include <QWidget>

#include "GestorVentas.h"

class QLabel;
class QTableWidget;
class GraficaBarras;

class PestanaReporte : public QWidget {
    Q_OBJECT

public:
    explicit PestanaReporte(const GestorVentas& gestorVentas, QWidget* padre = nullptr);

    // Vuelve a calcular el reporte del dia y repinta todo. MainWindow la
    // llama cada vez que esta pestana se vuelve visible.
    void actualizar();

private:
    const GestorVentas& gestorVentas_;

    QLabel* etiquetaTransacciones_;
    QLabel* etiquetaTotal_;
    QTableWidget* tablaProductos_;
    // Tabla de transacciones individuales del dia (folio, hora, metodo de
    // pago, total) -- es lo que hacia falta para distinguir dos ventas del
    // mismo producto entre si, en vez de solo ver el total sumado por
    // producto en tablaProductos_.
    QTableWidget* tablaTransacciones_;
    // Grafica de ventas por categoria -- se repuebla en cada actualizar(),
    // igual que las tablas de arriba, asi que aparece "automaticamente"
    // sin que el usuario tenga que pedirla aparte.
    GraficaBarras* graficaCategorias_;
};

#endif // PESTANA_REPORTE_H
