// PestanaProductos.h
//
// Pestana "Productos": tabla con todo el inventario, busqueda, y botones
// para alta/edicion/baja. Equivale al submenu "Gestion de productos" de la
// version de consola, pero con tabla y clics en vez de texto.

#ifndef PESTANA_PRODUCTOS_H
#define PESTANA_PRODUCTOS_H

#include <QWidget>

#include "Inventario.h"

class QLineEdit;
class QTableWidget;
class QLabel;

class PestanaProductos : public QWidget {
    Q_OBJECT

public:
    // Recibe una REFERENCIA a Inventario (igual que Menu en la version de
    // consola): esta pestana no es dueña del inventario, solo lo usa.
    explicit PestanaProductos(Inventario& inventario, QWidget* padre = nullptr);

    // Vuelve a leer Inventario y repinta la tabla. Se llama al arrancar,
    // despues de cada alta/edicion/baja, y cuando otra pestana (Ventas)
    // avisa que cambio el stock.
    void refrescar();

signals:
    // Qt usa "signals" para avisar "algo paso" sin que esta clase necesite
    // saber quien esta escuchando ni que va a hacer con la noticia (bajo
    // acoplamiento). MainWindow se conecta a esta señal para guardar en
    // disco y refrescar otras pestañas. Es el equivalente, en GUI, de que
    // Menu::alDarAltaProducto() llamara a guardarDatos() directamente --
    // aqui en cambio, esta clase ni sabe que existe la persistencia.
    void datosModificados();

private slots:
    // Un "slot" es simplemente un metodo que puede conectarse a una señal
    // (aqui, a la señal `clicked()` de cada QPushButton). El compilador no
    // exige la palabra `private slots:` para que funcione tecnicamente,
    // pero es la convencion de Qt para dejar claro cuales metodos existen
    // para responder a eventos de la UI.
    void alHacerNuevo();
    void alHacerEditar();
    void alHacerEliminar();
    void alCambiarBusqueda(const QString& texto);

private:
    void llenarTabla(const std::vector<Producto>& productos);
    // Devuelve el codigo del producto seleccionado en la tabla, o una
    // cadena vacia si no hay seleccion.
    QString codigoSeleccionado() const;

    Inventario& inventario_;

    QLineEdit* campoBusqueda_;
    QTableWidget* tabla_;
    QLabel* etiquetaAlerta_;
};

#endif // PESTANA_PRODUCTOS_H
