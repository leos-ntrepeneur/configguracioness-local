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
#include "InformacionNegocio.h"

class QLabel;
class QTableWidget;
class GraficaBarras;

class PestanaReporte : public QWidget {
    Q_OBJECT

public:
    // `informacionNegocio` se recibe por referencia CONSTANTE, igual que
    // en PestanaVentas: esta pestaña solo la necesita para poder reabrir
    // el ticket exacto de una transaccion (ver alVerTicketTransaccion), no
    // la edita.
    PestanaReporte(const GestorVentas& gestorVentas, const InformacionNegocio& informacionNegocio,
                   QWidget* padre = nullptr);

    // Vuelve a calcular el reporte del dia y repinta todo. MainWindow la
    // llama cada vez que esta pestana se vuelve visible.
    void actualizar();

private slots:
    // Doble clic en una fila del historial de transacciones: reabre el
    // ticket exacto de esa venta (mismo texto que se vio al confirmarla),
    // para poder revisar que se compro sin tener que adivinarlo a partir
    // de folio/hora/total sueltos.
    void alVerTicketTransaccion(int fila, int columna);

private:
    const GestorVentas& gestorVentas_;
    const InformacionNegocio& informacionNegocio_;

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
