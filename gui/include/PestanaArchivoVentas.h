// PestanaArchivoVentas.h
//
// Pestana "Archivo de ventas": a diferencia de PestanaReporte (que SOLO
// muestra las ventas de HOY, recalculadas cada vez), esta pestana lista
// el historial COMPLETO -- todas las ventas que se han registrado desde
// que el negocio empezo a usar el programa, sin limite de fecha. Los datos
// nunca se pierden porque ya se guardan completos en ventas.csv desde
// siempre (ver RepositorioVentasCsv); esta pestana es solo la vista que
// faltaba para poder revisarlos.

#ifndef PESTANA_ARCHIVO_VENTAS_H
#define PESTANA_ARCHIVO_VENTAS_H

#include <QWidget>

#include "GestorVentas.h"
#include "InformacionNegocio.h"

class QLabel;
class QLineEdit;
class QTableWidget;

class PestanaArchivoVentas : public QWidget {
    Q_OBJECT

public:
    PestanaArchivoVentas(const GestorVentas& gestorVentas, const InformacionNegocio& informacionNegocio,
                          QWidget* padre = nullptr);

    // Vuelve a leer gestorVentas_.listarVentas() y repuebla la tabla.
    // MainWindow la llama cuando se registra una venta nueva y cada vez
    // que esta pestaña se vuelve visible (por si se cerro el dia o se
    // cargaron datos desde otro lado mientras tanto).
    void actualizar();

private slots:
    // Filtra la tabla por folio o por texto de fecha mientras se escribe
    // (mismo patron "en vivo" que la busqueda de la pestaña Vender).
    void alCambiarFiltro(const QString& texto);
    void alVerTicket(int fila, int columna);

private:
    void llenarTabla(const std::vector<Venta>& ventas);

    const GestorVentas& gestorVentas_;
    const InformacionNegocio& informacionNegocio_;

    QLabel* etiquetaTotal_;
    QLineEdit* campoFiltro_;
    QTableWidget* tablaVentas_;
};

#endif // PESTANA_ARCHIVO_VENTAS_H
