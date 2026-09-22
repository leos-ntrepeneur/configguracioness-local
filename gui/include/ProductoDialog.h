// ProductoDialog.h
//
// Formulario emergente (QDialog) para dar de alta o editar un producto.
// Un QDialog es una ventana secundaria modal: mientras esta abierta, la
// ventana principal queda bloqueada hasta que el usuario la cierra
// (aceptar o cancelar) -- es el equivalente grafico de lo que en la
// consola era "detener el programa y pedir datos por teclado".
//
// NOTA DE C++/Qt: las clases que quieren usar señales y slots (el sistema
// de Qt para conectar eventos con acciones, ej. "cuando le den clic a este
// boton, llama a esta funcion") DEBEN heredar de QObject (QDialog ya lo
// hace por ti) y llevar la macro Q_OBJECT en la declaracion. Esa macro no
// es C++ estandar: la procesa una herramienta de Qt llamada "moc" (Meta
// Object Compiler) antes de compilar, que genera codigo C++ adicional por
// detras. CMake la ejecuta automaticamente para nosotros (ver
// CMakeLists.txt, AUTOMOC).

#ifndef PRODUCTO_DIALOG_H
#define PRODUCTO_DIALOG_H

#include <QDialog>

#include "Producto.h"

class QLineEdit;
class QDoubleSpinBox;
class QSpinBox;

class ProductoDialog : public QDialog {
    Q_OBJECT

public:
    enum class Modo { Nuevo, Editar };

    // En modo Editar, `existente` trae los datos actuales para precargar
    // el formulario; en modo Nuevo se ignora (se deja vacio/ceros).
    ProductoDialog(Modo modo, const Producto* existente, QWidget* padre = nullptr);

    // Construye un Producto con lo que el usuario capturo. Solo tiene
    // sentido llamarlo despues de que el dialogo se cerro con "Aceptar"
    // (exec() devolvio QDialog::Accepted); puede lanzar EntradaInvalida
    // si algo no paso las validaciones de Producto.
    Producto obtenerProducto() const;

private:
    Modo modo_;

    QLineEdit* campoCodigo_;
    QLineEdit* campoNombre_;
    QDoubleSpinBox* campoPrecio_;
    QSpinBox* campoStock_;
    QLineEdit* campoCategoria_;
    QSpinBox* campoStockMinimo_;
};

#endif // PRODUCTO_DIALOG_H
