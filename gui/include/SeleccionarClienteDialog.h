// SeleccionarClienteDialog.h
//
// Dialogo emergente que se muestra al confirmar una venta al fiado (ver
// PestanaVentas::alConfirmarVenta): deja elegir un cliente YA REGISTRADO
// de una lista, o dar de alta uno nuevo ahi mismo sin tener que ir primero
// a la pestaña "Clientes". Equivalente grafico de Menu::elegirOCrearCliente
// en la consola.

#ifndef SELECCIONAR_CLIENTE_DIALOG_H
#define SELECCIONAR_CLIENTE_DIALOG_H

#include <QDialog>

#include "GestorClientes.h"

class QComboBox;
class QLineEdit;
class QWidget;

class SeleccionarClienteDialog : public QDialog {
    Q_OBJECT

public:
    // `gestorClientes` por referencia NO constante: si el usuario elige
    // dar de alta un cliente nuevo, este dialogo lo agrega directamente
    // (igual que Menu::elegirOCrearCliente llama a
    // gestorClientes_.agregarCliente). Quien abre el dialogo (PestanaVentas)
    // es responsable de persistir el cambio despues, igual que con
    // cualquier otra alta.
    explicit SeleccionarClienteDialog(GestorClientes& gestorClientes, QWidget* padre = nullptr);

    // Solo tiene sentido llamarlo despues de que exec() devolvio
    // QDialog::Accepted -- devuelve el id del cliente elegido (existente)
    // o el del que se acaba de crear.
    int clienteIdSeleccionado() const;

private slots:
    // Conectado en vez del accept() directo de QDialogButtonBox: valida
    // (nombre no vacio si se esta dando de alta un cliente nuevo) ANTES
    // de cerrar el dialogo, para poder mostrar el error y dejarlo abierto
    // en vez de cerrarse con datos invalidos.
    void alAceptar();
    void alCambiarSeleccion(int indice);

private:
    void repoblarCombo();

    GestorClientes& gestorClientes_;
    int clienteIdSeleccionado_ = 0;

    QComboBox* comboClientes_;
    QWidget* panelNuevoCliente_;
    QLineEdit* campoNombreNuevo_;
    QLineEdit* campoTelefonoNuevo_;
};

#endif // SELECCIONAR_CLIENTE_DIALOG_H
