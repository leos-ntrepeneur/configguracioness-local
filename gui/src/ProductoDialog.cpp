#include "ProductoDialog.h"

#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QVBoxLayout>

ProductoDialog::ProductoDialog(Modo modo, const Producto* existente, QWidget* padre)
    : QDialog(padre), modo_(modo) {
    setWindowTitle(modo_ == Modo::Nuevo ? "Nuevo producto" : "Editar producto");

    // Los rangos/longitudes vienen de Producto (Producto::STOCK_MAXIMO,
    // etc.) en vez de numeros sueltos aqui: es la MISMA regla de negocio
    // que ya valida Producto en su constructor, asi que si algun dia
    // cambia, cambia en un solo lugar y consola+GUI quedan sincronizadas.
    // Ademas de evitar que el usuario escriba datos invalidos, esto evita
    // por completo que existan productos con mas stock del que el resto
    // del programa (carrito de ventas, sumas de totales) esta preparado
    // para manejar sin desbordar un entero -- ver el comentario en
    // Producto.h.
    campoCodigo_ = new QLineEdit(this);
    campoCodigo_->setMaxLength(Producto::CODIGO_LONGITUD_MAXIMA);

    campoNombre_ = new QLineEdit(this);
    campoNombre_->setMaxLength(Producto::NOMBRE_LONGITUD_MAXIMA);

    campoPrecio_ = new QDoubleSpinBox(this);
    campoPrecio_->setRange(0.0, Producto::PRECIO_MAXIMO);
    campoPrecio_->setDecimals(2);
    campoPrecio_->setPrefix("$ ");

    campoStock_ = new QSpinBox(this);
    campoStock_->setRange(0, Producto::STOCK_MAXIMO);

    campoCategoria_ = new QLineEdit(this);
    campoCategoria_->setMaxLength(Producto::CATEGORIA_LONGITUD_MAXIMA);

    campoStockMinimo_ = new QSpinBox(this);
    campoStockMinimo_->setRange(0, Producto::STOCK_MAXIMO);

    if (existente != nullptr) {
        campoCodigo_->setText(QString::fromStdString(existente->getCodigo()));
        campoNombre_->setText(QString::fromStdString(existente->getNombre()));
        campoPrecio_->setValue(existente->getPrecio());
        campoStock_->setValue(existente->getStock());
        campoCategoria_->setText(QString::fromStdString(existente->getCategoria()));
        campoStockMinimo_->setValue(existente->getStockMinimo());
    }

    // En modo Editar, el codigo no se puede tocar (es la clave del
    // producto) y el stock tampoco se edita aqui a mano -- se mueve solo
    // via ventas, igual que en la version de consola (Inventario::
    // editarProducto ni siquiera recibe un stock nuevo como parametro).
    if (modo_ == Modo::Editar) {
        campoCodigo_->setEnabled(false);
        campoStock_->setEnabled(false);
    }

    auto* formulario = new QFormLayout();
    formulario->addRow("Codigo:", campoCodigo_);
    formulario->addRow("Nombre:", campoNombre_);
    formulario->addRow("Precio:", campoPrecio_);
    formulario->addRow("Stock:", campoStock_);
    formulario->addRow("Categoria:", campoCategoria_);
    formulario->addRow("Stock minimo:", campoStockMinimo_);

    // QDialogButtonBox arma automaticamente los botones "Aceptar"/
    // "Cancelar" (con el texto traducido al idioma del sistema) y ya
    // conectados a accept()/reject(), los metodos que QDialog usa para
    // saber con que resultado se cerro.
    auto* botones = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(botones, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(botones, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto* layoutPrincipal = new QVBoxLayout(this);
    layoutPrincipal->addLayout(formulario);
    layoutPrincipal->addWidget(botones);
}

Producto ProductoDialog::obtenerProducto() const {
    // El constructor de Producto valida todo (comas, negativos, vacios) y
    // lanza EntradaInvalida si algo esta mal; quien llame a este metodo
    // decide como mostrar ese error (ver PestanaProductos.cpp).
    return Producto(campoCodigo_->text().toStdString(),
                     campoNombre_->text().toStdString(),
                     campoPrecio_->value(),
                     campoStock_->value(),
                     campoCategoria_->text().toStdString(),
                     campoStockMinimo_->value());
}
