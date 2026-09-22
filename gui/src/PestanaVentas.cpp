#include "PestanaVentas.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSplitter>
#include <QTableWidget>
#include <QVBoxLayout>

#include "Excepciones.h"

namespace {
constexpr int COL_PROD_CODIGO = 0;
constexpr int COL_PROD_NOMBRE = 1;
constexpr int COL_PROD_PRECIO = 2;
constexpr int COL_PROD_STOCK = 3;

constexpr int COL_CARR_CODIGO = 0;
constexpr int COL_CARR_NOMBRE = 1;
constexpr int COL_CARR_CANTIDAD = 2;
constexpr int COL_CARR_SUBTOTAL = 3;
} // namespace

PestanaVentas::PestanaVentas(Inventario& inventario, GestorVentas& gestorVentas, QWidget* padre)
    : QWidget(padre), inventario_(inventario), gestorVentas_(gestorVentas) {
    // --- Panel izquierdo: productos disponibles ---
    campoBusqueda_ = new QLineEdit(this);
    campoBusqueda_->setPlaceholderText("Buscar por nombre o codigo...");

    tablaProductos_ = new QTableWidget(this);
    tablaProductos_->setColumnCount(4);
    tablaProductos_->setHorizontalHeaderLabels({"Codigo", "Nombre", "Precio", "Stock"});
    tablaProductos_->setEditTriggers(QTableWidget::NoEditTriggers);
    tablaProductos_->setSelectionBehavior(QTableWidget::SelectRows);
    tablaProductos_->setSelectionMode(QTableWidget::SingleSelection);
    tablaProductos_->horizontalHeader()->setStretchLastSection(true);
    tablaProductos_->verticalHeader()->setVisible(false);

    botonAgregar_ = new QPushButton("Agregar al carrito ->", this);

    auto* panelIzquierdo = new QWidget(this);
    auto* layoutIzquierdo = new QVBoxLayout(panelIzquierdo);
    layoutIzquierdo->addWidget(new QLabel("Productos disponibles", panelIzquierdo));
    layoutIzquierdo->addWidget(campoBusqueda_);
    layoutIzquierdo->addWidget(tablaProductos_);
    layoutIzquierdo->addWidget(botonAgregar_);

    // --- Panel derecho: carrito ---
    tablaCarrito_ = new QTableWidget(this);
    tablaCarrito_->setColumnCount(4);
    tablaCarrito_->setHorizontalHeaderLabels({"Codigo", "Nombre", "Cantidad", "Subtotal"});
    tablaCarrito_->setEditTriggers(QTableWidget::NoEditTriggers);
    tablaCarrito_->setSelectionBehavior(QTableWidget::SelectRows);
    tablaCarrito_->setSelectionMode(QTableWidget::SingleSelection);
    tablaCarrito_->horizontalHeader()->setStretchLastSection(true);
    tablaCarrito_->verticalHeader()->setVisible(false);

    botonQuitar_ = new QPushButton("<- Quitar del carrito", this);
    etiquetaTotal_ = new QLabel("Total: $0.00", this);
    etiquetaTotal_->setStyleSheet("font-weight: bold; font-size: 14px;");
    botonConfirmar_ = new QPushButton("Confirmar venta", this);

    auto* panelDerecho = new QWidget(this);
    auto* layoutDerecho = new QVBoxLayout(panelDerecho);
    layoutDerecho->addWidget(new QLabel("Carrito", panelDerecho));
    layoutDerecho->addWidget(tablaCarrito_);
    layoutDerecho->addWidget(botonQuitar_);
    layoutDerecho->addWidget(etiquetaTotal_);
    layoutDerecho->addWidget(botonConfirmar_);

    // QSplitter deja al usuario arrastrar la division entre los dos
    // paneles con el mouse, algo que un QHBoxLayout simple no ofrece.
    auto* separador = new QSplitter(Qt::Horizontal, this);
    separador->addWidget(panelIzquierdo);
    separador->addWidget(panelDerecho);

    auto* layoutPrincipal = new QVBoxLayout(this);
    layoutPrincipal->addWidget(separador);

    connect(campoBusqueda_, &QLineEdit::textChanged, this, &PestanaVentas::alCambiarBusqueda);
    connect(botonAgregar_, &QPushButton::clicked, this, &PestanaVentas::alAgregarAlCarrito);
    connect(botonQuitar_, &QPushButton::clicked, this, &PestanaVentas::alQuitarDelCarrito);
    connect(botonConfirmar_, &QPushButton::clicked, this, &PestanaVentas::alConfirmarVenta);
    // Doble clic en un producto es un atajo para agregarlo directo, sin
    // pasar primero por seleccionar y luego darle al boton.
    connect(tablaProductos_, &QTableWidget::cellDoubleClicked, this, [this](int, int) {
        alAgregarAlCarrito();
    });

    refrescarListaProductos();
    actualizarCarrito();
}

void PestanaVentas::refrescarListaProductos() {
    if (campoBusqueda_->text().isEmpty()) {
        llenarTablaProductos(inventario_.listarTodos());
    } else {
        alCambiarBusqueda(campoBusqueda_->text());
    }
}

void PestanaVentas::llenarTablaProductos(const std::vector<Producto>& productos) {
    tablaProductos_->setRowCount(static_cast<int>(productos.size()));
    for (int fila = 0; fila < static_cast<int>(productos.size()); ++fila) {
        const Producto& p = productos[static_cast<std::size_t>(fila)];
        tablaProductos_->setItem(fila, COL_PROD_CODIGO, new QTableWidgetItem(QString::fromStdString(p.getCodigo())));
        tablaProductos_->setItem(fila, COL_PROD_NOMBRE, new QTableWidgetItem(QString::fromStdString(p.getNombre())));
        tablaProductos_->setItem(fila, COL_PROD_PRECIO, new QTableWidgetItem(QString::number(p.getPrecio(), 'f', 2)));
        tablaProductos_->setItem(fila, COL_PROD_STOCK, new QTableWidgetItem(QString::number(p.getStock())));
    }
}

void PestanaVentas::actualizarCarrito() {
    tablaCarrito_->setRowCount(static_cast<int>(carrito_.size()));
    double total = 0.0;

    int fila = 0;
    for (const auto& [codigo, cantidad] : carrito_) {
        try {
            const Producto& producto = inventario_.buscarPorCodigo(codigo);
            double subtotal = producto.getPrecio() * cantidad;
            total += subtotal;
            tablaCarrito_->setItem(fila, COL_CARR_CODIGO, new QTableWidgetItem(QString::fromStdString(codigo)));
            tablaCarrito_->setItem(fila, COL_CARR_NOMBRE,
                                    new QTableWidgetItem(QString::fromStdString(producto.getNombre())));
            tablaCarrito_->setItem(fila, COL_CARR_CANTIDAD, new QTableWidgetItem(QString::number(cantidad)));
            tablaCarrito_->setItem(fila, COL_CARR_SUBTOTAL, new QTableWidgetItem(QString::number(subtotal, 'f', 2)));
        } catch (const ProductoNoEncontrado&) {
            // El producto se elimino del inventario mientras estaba en el
            // carrito. Se muestra la fila igual, marcada, en vez de
            // tronar; al confirmar la venta GestorVentas la rechazara con
            // un mensaje claro.
            tablaCarrito_->setItem(fila, COL_CARR_CODIGO, new QTableWidgetItem(QString::fromStdString(codigo)));
            tablaCarrito_->setItem(fila, COL_CARR_NOMBRE, new QTableWidgetItem("(producto ya no existe)"));
            tablaCarrito_->setItem(fila, COL_CARR_CANTIDAD, new QTableWidgetItem(QString::number(cantidad)));
            tablaCarrito_->setItem(fila, COL_CARR_SUBTOTAL, new QTableWidgetItem("-"));
        }
        ++fila;
    }

    etiquetaTotal_->setText(QString("Total: $%1").arg(total, 0, 'f', 2));
}

QString PestanaVentas::codigoSeleccionadoEnProductos() const {
    int fila = tablaProductos_->currentRow();
    if (fila < 0) {
        return QString();
    }
    return tablaProductos_->item(fila, COL_PROD_CODIGO)->text();
}

QString PestanaVentas::codigoSeleccionadoEnCarrito() const {
    int fila = tablaCarrito_->currentRow();
    if (fila < 0) {
        return QString();
    }
    return tablaCarrito_->item(fila, COL_CARR_CODIGO)->text();
}

void PestanaVentas::alCambiarBusqueda(const QString& texto) {
    std::string textoBuscado = texto.toStdString();
    if (textoBuscado.empty()) {
        llenarTablaProductos(inventario_.listarTodos());
        return;
    }
    if (inventario_.existeCodigo(textoBuscado)) {
        llenarTablaProductos({inventario_.buscarPorCodigo(textoBuscado)});
        return;
    }
    llenarTablaProductos(inventario_.buscarPorNombre(textoBuscado));
}

void PestanaVentas::alAgregarAlCarrito() {
    QString codigo = codigoSeleccionadoEnProductos();
    if (codigo.isEmpty()) {
        QMessageBox::information(this, "Agregar al carrito", "Selecciona un producto de la lista primero.");
        return;
    }

    try {
        const Producto& producto = inventario_.buscarPorCodigo(codigo.toStdString());

        bool confirmado = false;
        int cantidad = QInputDialog::getInt(this, "Cantidad",
                                             "Cantidad de \"" + QString::fromStdString(producto.getNombre()) + "\":",
                                             1, 1, 1'000'000, 1, &confirmado);
        if (!confirmado) {
            return; // el usuario le dio "Cancelar" en el dialogo de cantidad.
        }

        int yaEnCarrito = carrito_.count(codigo.toStdString()) ? carrito_.at(codigo.toStdString()) : 0;
        int totalSolicitado = yaEnCarrito + cantidad;
        if (totalSolicitado > producto.getStock()) {
            throw StockInsuficiente(codigo.toStdString(), producto.getStock(), totalSolicitado);
        }

        carrito_[codigo.toStdString()] = totalSolicitado;
        actualizarCarrito();
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "No se pudo agregar", e.what());
    }
}

void PestanaVentas::alQuitarDelCarrito() {
    QString codigo = codigoSeleccionadoEnCarrito();
    if (codigo.isEmpty()) {
        QMessageBox::information(this, "Quitar del carrito", "Selecciona un producto del carrito primero.");
        return;
    }
    carrito_.erase(codigo.toStdString());
    actualizarCarrito();
}

void PestanaVentas::alConfirmarVenta() {
    if (carrito_.empty()) {
        QMessageBox::information(this, "Confirmar venta", "El carrito esta vacio.");
        return;
    }

    std::vector<DetalleVenta> detalles;
    detalles.reserve(carrito_.size());
    try {
        for (const auto& [codigo, cantidad] : carrito_) {
            const Producto& producto = inventario_.buscarPorCodigo(codigo);
            detalles.emplace_back(codigo, producto.getNombre(), cantidad, producto.getPrecio());
        }

        const Venta& venta = gestorVentas_.registrarVenta(detalles);

        QString resumen = QString("Venta registrada.\nFecha: %1\nTotal: $%2")
                               .arg(QString::fromStdString(venta.fechaComoTexto()))
                               .arg(venta.getTotal(), 0, 'f', 2);

        // Igual que en consola: avisar de inmediato si algun producto
        // vendido quedo con stock bajo (Requisito 4).
        for (const DetalleVenta& detalle : venta.getDetalles()) {
            const Producto& actualizado = inventario_.buscarPorCodigo(detalle.getCodigoProducto());
            if (actualizado.estaBajoStockMinimo()) {
                resumen += QString("\n\nAVISO: %1 quedo con stock bajo (%2 unidades, minimo %3).")
                               .arg(QString::fromStdString(actualizado.getNombre()))
                               .arg(actualizado.getStock())
                               .arg(actualizado.getStockMinimo());
            }
        }

        QMessageBox::information(this, "Venta confirmada", resumen);

        carrito_.clear();
        actualizarCarrito();
        refrescarListaProductos(); // el stock de la izquierda tambien cambio.
        emit datosModificados();
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "No se pudo registrar la venta", e.what());
    }
}
