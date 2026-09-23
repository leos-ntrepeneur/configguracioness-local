// PestanaCortes.h
//
// Pestana "Cortes de caja": tabla con el historial de cierres del dia ya
// generados, mas un boton para cerrar el dia ahora mismo. Equivalente
// grafico de Menu::alCerrarDia()/alVerCortesAnteriores() en la consola --
// mismo GestorCortes, mismo GeneradorCorte, solo cambia como se pide/
// muestra.

#ifndef PESTANA_CORTES_H
#define PESTANA_CORTES_H

#include <QWidget>

#include "GestorCortes.h"
#include "IRepositorioCortes.h"
#include "InformacionNegocio.h"

class QLabel;
class QPushButton;
class QTableWidget;

class PestanaCortes : public QWidget {
    Q_OBJECT

public:
    // `informacionNegocio` es la MISMA instancia que PestanaVentas usa
    // para el ticket (ver MainWindow) -- el corte reutiliza el mismo
    // encabezado con los datos del local, por referencia constante porque
    // esta pestana solo lo lee.
    PestanaCortes(GestorCortes& gestorCortes, IRepositorioCortes& repositorioCortes,
                  const InformacionNegocio& informacionNegocio, QWidget* padre = nullptr);

    // Vuelve a llenar la tabla desde gestorCortes_.listarCortes(). No hace
    // falta llamarla despues de alCerrarDia() (ya la llama internamente),
    // pero MainWindow si la usa al cambiar de pestana, igual que con
    // PestanaReporte, por si se cerro el dia desde otra sesion... aunque
    // hoy el programa es de un solo proceso, mantenerla publica cuesta
    // nada y evita sorpresas si el dia de mañana hay mas de una fuente de
    // cortes.
    void actualizar();

private slots:
    void alCerrarDia();
    void alVerDetalle(int fila, int columna);

private:
    void llenarTabla();

    GestorCortes& gestorCortes_;
    IRepositorioCortes& repositorioCortes_;
    const InformacionNegocio& informacionNegocio_;

    QTableWidget* tablaCortes_;
    QPushButton* botonCerrarDia_;
};

#endif // PESTANA_CORTES_H
