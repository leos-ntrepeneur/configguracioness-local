// PestanaVentas.h
//
// Pestana "Vender": tabla de productos disponibles a la izquierda, carrito
// a la derecha. Mismo diseño que el flujo de consola (Menu::registrarVenta
// + Menu::agregarAlCarrito/quitarDelCarrito/confirmarVenta), pero armando
// el carrito con clics en vez de escribir codigos a mano.

#ifndef PESTANA_VENTAS_H
#define PESTANA_VENTAS_H

#include <QWidget>
#include <map>
#include <string>

#include "GestorVentas.h"
#include "Inventario.h"

class QLineEdit;
class QTableWidget;
class QLabel;
class QPushButton;

class PestanaVentas : public QWidget {
    Q_OBJECT

public:
    PestanaVentas(Inventario& inventario, GestorVentas& gestorVentas, QWidget* padre = nullptr);

    // Se llama cuando otra pestana (Productos) modifico el inventario, para
    // que la lista de productos disponibles y sus precios/stock queden al
    // dia aqui tambien.
    void refrescarListaProductos();

signals:
    void datosModificados();

private slots:
    void alCambiarBusqueda(const QString& texto);
    void alAgregarAlCarrito();
    void alQuitarDelCarrito();
    void alConfirmarVenta();

private:
    void llenarTablaProductos(const std::vector<Producto>& productos);
    void actualizarCarrito();
    QString codigoSeleccionadoEnProductos() const;
    QString codigoSeleccionadoEnCarrito() const;

    Inventario& inventario_;
    GestorVentas& gestorVentas_;

    // codigo -> cantidad acumulada, igual que en la version de consola.
    std::map<std::string, int> carrito_;

    QLineEdit* campoBusqueda_;
    QTableWidget* tablaProductos_;
    QPushButton* botonAgregar_;

    QTableWidget* tablaCarrito_;
    QPushButton* botonQuitar_;
    QLabel* etiquetaTotal_;
    QPushButton* botonConfirmar_;
};

#endif // PESTANA_VENTAS_H
