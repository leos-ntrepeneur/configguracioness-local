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

#include "GestorClientes.h"
#include "GestorVentas.h"
#include "InformacionNegocio.h"
#include "Inventario.h"

class QLineEdit;
class QTableWidget;
class QLabel;
class QPushButton;
class QButtonGroup;

class PestanaVentas : public QWidget {
    Q_OBJECT

public:
    // `informacionNegocio` se recibe por referencia CONSTANTE: esta pestaña
    // solo la LEE para armar el ticket al confirmar una venta, quien la
    // edita es PestanaInformacionNegocio (via MainWindow, misma instancia).
    // `gestorClientes` NO es constante: una venta al fiado puede dar de
    // alta un cliente nuevo ahi mismo (ver SeleccionarClienteDialog).
    PestanaVentas(Inventario& inventario, GestorVentas& gestorVentas, GestorClientes& gestorClientes,
                  const InformacionNegocio& informacionNegocio, QWidget* padre = nullptr);

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

    // Logica compartida por el flujo con dialogo (alAgregarAlCarrito, pide
    // cantidad) y el boton rapido "+" por fila (siempre agrega 1): valida
    // stock disponible (incluyendo lo que ya hubiera en el carrito) y
    // actualiza carrito_. Lanza StockInsuficiente si no alcanza.
    void agregarCantidadAlCarrito(const std::string& codigo, int cantidad);

    // Traduce el boton de metodo de pago actualmente marcado en el grupo
    // (ver grupoMetodoPago_) al enum MetodoPago correspondiente.
    MetodoPago metodoPagoSeleccionado() const;

    Inventario& inventario_;
    GestorVentas& gestorVentas_;
    GestorClientes& gestorClientes_;
    const InformacionNegocio& informacionNegocio_;

    // codigo -> cantidad acumulada, igual que en la version de consola.
    std::map<std::string, int> carrito_;

    QLineEdit* campoBusqueda_;
    QTableWidget* tablaProductos_;
    QPushButton* botonAgregar_;

    QTableWidget* tablaCarrito_;
    QPushButton* botonQuitar_;
    QLabel* etiquetaTotal_;
    // Selector 1/2/3 de la consola (Menu::preguntarMetodoPago), pero como
    // 3 botones tipo "interruptor" en vez de una lista desplegable -- mas
    // rapido de usar con el mouse/tactil que abrir un combo y elegir.
    // QButtonGroup no es un widget visible: solo coordina que, de los 3
    // botones que se le agregan, nada mas uno quede :checked a la vez
    // (setExclusive(true), el valor por defecto).
    QButtonGroup* grupoMetodoPago_;
    QPushButton* botonPagoEfectivo_;
    QPushButton* botonPagoTarjetaCredito_;
    QPushButton* botonPagoTarjetaDebito_;
    QPushButton* botonPagoFiado_;
    QPushButton* botonConfirmar_;
};

#endif // PESTANA_VENTAS_H
