// PestanaClientes.h
//
// Pestana "Clientes": lista de clientes con su saldo pendiente (calculado
// en vivo, ver GestorCreditos), un boton para dar de alta un cliente
// nuevo y otro para registrar un abono (pago) del cliente seleccionado.
// Equivalente grafico de Menu::alGestionarClientes() en la consola.

#ifndef PESTANA_CLIENTES_H
#define PESTANA_CLIENTES_H

#include <QWidget>

#include "GestorClientes.h"
#include "GestorCreditos.h"
#include "IRepositorioAbonos.h"
#include "IRepositorioClientes.h"

class QPushButton;
class QTableWidget;

class PestanaClientes : public QWidget {
    Q_OBJECT

public:
    PestanaClientes(GestorClientes& gestorClientes, GestorCreditos& gestorCreditos,
                     IRepositorioClientes& repositorioClientes, IRepositorioAbonos& repositorioAbonos,
                     QWidget* padre = nullptr);

    // Vuelve a leer clientes/saldos y repuebla la tabla. MainWindow la
    // llama cuando una venta al fiado da de alta un cliente nuevo desde
    // PestanaVentas, para que esta pestaña no se quede desactualizada.
    void actualizar();

private slots:
    void alNuevoCliente();
    void alRegistrarAbono();

private:
    void llenarTabla();

    GestorClientes& gestorClientes_;
    GestorCreditos& gestorCreditos_;
    IRepositorioClientes& repositorioClientes_;
    IRepositorioAbonos& repositorioAbonos_;

    QTableWidget* tablaClientes_;
    QPushButton* botonNuevoCliente_;
    QPushButton* botonRegistrarAbono_;
};

#endif // PESTANA_CLIENTES_H
