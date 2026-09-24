#include "PestanaVentas.h"

#include <QButtonGroup>
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
#include "GeneradorTicket.h"
#include "SeleccionarClienteDialog.h"
#include "TicketDialog.h"

namespace {
constexpr int COL_PROD_CODIGO = 0;
constexpr int COL_PROD_NOMBRE = 1;
constexpr int COL_PROD_PRECIO = 2;
constexpr int COL_PROD_STOCK = 3;
constexpr int COL_PROD_AGREGAR = 4;

constexpr int COL_CARR_CODIGO = 0;
constexpr int COL_CARR_NOMBRE = 1;
constexpr int COL_CARR_CANTIDAD = 2;
constexpr int COL_CARR_SUBTOTAL = 3;
} // namespace

PestanaVentas::PestanaVentas(Inventario& inventario, GestorVentas& gestorVentas, GestorClientes& gestorClientes,
                              const InformacionNegocio& informacionNegocio, QWidget* padre)
    : QWidget(padre),
      inventario_(inventario),
      gestorVentas_(gestorVentas),
      gestorClientes_(gestorClientes),
      informacionNegocio_(informacionNegocio) {
    // --- Panel izquierdo: productos disponibles ---
    campoBusqueda_ = new QLineEdit(this);
    campoBusqueda_->setPlaceholderText("Buscar por nombre o codigo...");

    tablaProductos_ = new QTableWidget(this);
    tablaProductos_->setColumnCount(5);
    tablaProductos_->setHorizontalHeaderLabels({"Codigo", "Nombre", "Precio", "Stock", ""});
    tablaProductos_->setEditTriggers(QTableWidget::NoEditTriggers);
    tablaProductos_->setSelectionBehavior(QTableWidget::SelectRows);
    tablaProductos_->setSelectionMode(QTableWidget::SingleSelection);
    tablaProductos_->verticalHeader()->setVisible(false);
    tablaProductos_->setAlternatingRowColors(true);
    tablaProductos_->horizontalHeader()->setSectionResizeMode(COL_PROD_CODIGO, QHeaderView::ResizeToContents);
    tablaProductos_->horizontalHeader()->setSectionResizeMode(COL_PROD_NOMBRE, QHeaderView::Stretch);
    tablaProductos_->horizontalHeader()->setSectionResizeMode(COL_PROD_PRECIO, QHeaderView::ResizeToContents);
    tablaProductos_->horizontalHeader()->setSectionResizeMode(COL_PROD_STOCK, QHeaderView::ResizeToContents);
    tablaProductos_->horizontalHeader()->setSectionResizeMode(COL_PROD_AGREGAR, QHeaderView::ResizeToContents);

    botonAgregar_ = new QPushButton("Agregar al carrito ->", this);

    auto* panelIzquierdo = new QWidget(this);
    auto* tituloProductos = new QLabel("Productos disponibles", panelIzquierdo);
    tituloProductos->setProperty("clase", "titulo");

    auto* layoutIzquierdo = new QVBoxLayout(panelIzquierdo);
    layoutIzquierdo->addWidget(tituloProductos);
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
    tablaCarrito_->verticalHeader()->setVisible(false);
    tablaCarrito_->setAlternatingRowColors(true);
    tablaCarrito_->horizontalHeader()->setSectionResizeMode(COL_CARR_CODIGO, QHeaderView::ResizeToContents);
    tablaCarrito_->horizontalHeader()->setSectionResizeMode(COL_CARR_NOMBRE, QHeaderView::Stretch);
    tablaCarrito_->horizontalHeader()->setSectionResizeMode(COL_CARR_CANTIDAD, QHeaderView::ResizeToContents);
    tablaCarrito_->horizontalHeader()->setSectionResizeMode(COL_CARR_SUBTOTAL, QHeaderView::ResizeToContents);

    botonQuitar_ = new QPushButton("<- Quitar del carrito", this);
    botonQuitar_->setProperty("clase", "peligro");
    etiquetaTotal_ = new QLabel("Total: $0.00", this);
    // Color de acento para que el total salte a la vista, como en
    // cualquier ticket de venta moderno.
    etiquetaTotal_->setStyleSheet("font-family: 'Manrope'; font-weight: 800; font-size: 19px; color: #2dd4bf;");

    // Mismas 3 opciones que Menu::preguntarMetodoPago() en la consola,
    // pero como botones tipo "interruptor" en vez de una lista
    // desplegable -- un combo obliga a abrirlo y despues elegir (dos
    // clics, y hay que leer la opcion actual para saber que cambiar); con
    // 3 botones siempre visibles, un solo clic ve y cambia la seleccion.
    auto* etiquetaMetodoPago = new QLabel("Metodo de pago:", this);
    botonPagoEfectivo_ = new QPushButton("Efectivo", this);
    botonPagoTarjetaCredito_ = new QPushButton("Tarjeta de credito", this);
    botonPagoTarjetaDebito_ = new QPushButton("Tarjeta de debito", this);
    botonPagoFiado_ = new QPushButton("Fiado", this);
    for (QPushButton* boton : {botonPagoEfectivo_, botonPagoTarjetaCredito_, botonPagoTarjetaDebito_, botonPagoFiado_}) {
        boton->setProperty("clase", "seleccionable");
        boton->setCheckable(true);
    }
    // QButtonGroup con exclusividad (el default) se encarga de que, al
    // marcar un boton, los otros se desmarquen solos -- mismo
    // comportamiento que un radio button, sin tener que coordinarlo a
    // mano en cada slot de clicked().
    grupoMetodoPago_ = new QButtonGroup(this);
    grupoMetodoPago_->addButton(botonPagoEfectivo_);
    grupoMetodoPago_->addButton(botonPagoTarjetaCredito_);
    grupoMetodoPago_->addButton(botonPagoTarjetaDebito_);
    grupoMetodoPago_->addButton(botonPagoFiado_);
    botonPagoEfectivo_->setChecked(true); // Efectivo como opcion por defecto, la mas comun.

    auto* layoutMetodoPago = new QVBoxLayout();
    layoutMetodoPago->addWidget(botonPagoEfectivo_);
    layoutMetodoPago->addWidget(botonPagoTarjetaCredito_);
    layoutMetodoPago->addWidget(botonPagoTarjetaDebito_);
    layoutMetodoPago->addWidget(botonPagoFiado_);

    botonConfirmar_ = new QPushButton("Confirmar venta", this);
    botonConfirmar_->setProperty("clase", "primario");

    auto* panelDerecho = new QWidget(this);
    auto* tituloCarrito = new QLabel("Carrito", panelDerecho);
    tituloCarrito->setProperty("clase", "titulo");

    auto* layoutDerecho = new QVBoxLayout(panelDerecho);
    layoutDerecho->addWidget(tituloCarrito);
    layoutDerecho->addWidget(tablaCarrito_);
    layoutDerecho->addWidget(botonQuitar_);
    layoutDerecho->addWidget(etiquetaTotal_);
    layoutDerecho->addWidget(etiquetaMetodoPago);
    layoutDerecho->addLayout(layoutMetodoPago);
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
    // pasar primero por seleccionar y luego darle al boton. Mismo atajo en
    // el carrito, pero para quitar -- simetria: doble clic agrega de un
    // lado, doble clic quita del otro.
    connect(tablaProductos_, &QTableWidget::cellDoubleClicked, this, [this](int, int) {
        alAgregarAlCarrito();
    });
    connect(tablaCarrito_, &QTableWidget::cellDoubleClicked, this, [this](int, int) {
        alQuitarDelCarrito();
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

        // Boton "+" por fila: agrega 1 unidad de ESTE producto con un solo
        // clic, sin seleccionar la fila primero ni pasar por el dialogo de
        // cantidad (ver agregarCantidadAlCarrito) -- para el caso mas
        // comun ("vender 1 de esto") ya no hace falta bajar hasta el
        // boton "Agregar al carrito ->" al fondo del panel.
        std::string codigo = p.getCodigo();
        auto* botonAgregarRapido = new QPushButton("+", tablaProductos_);
        botonAgregarRapido->setProperty("clase", "primario");
        botonAgregarRapido->setFixedWidth(32);
        botonAgregarRapido->setToolTip("Agregar 1 al carrito");
        connect(botonAgregarRapido, &QPushButton::clicked, this, [this, codigo]() {
            try {
                agregarCantidadAlCarrito(codigo, 1);
            } catch (const std::exception& e) {
                QMessageBox::warning(this, "No se pudo agregar", e.what());
            }
        });
        tablaProductos_->setCellWidget(fila, COL_PROD_AGREGAR, botonAgregarRapido);
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
        // El rango (1, Producto::STOCK_MAXIMO) evita que el propio widget
        // deje escribir una cantidad absurda de entrada; ver el comentario
        // en agregarCantidadAlCarrito() por que ESO SOLO no basta para
        // evitar un desborde de enteros.
        int cantidad = QInputDialog::getInt(this, "Cantidad",
                                             "Cantidad de \"" + QString::fromStdString(producto.getNombre()) + "\":",
                                             1, 1, Producto::STOCK_MAXIMO, 1, &confirmado);
        if (!confirmado) {
            return; // el usuario le dio "Cancelar" en el dialogo de cantidad.
        }

        agregarCantidadAlCarrito(codigo.toStdString(), cantidad);
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "No se pudo agregar", e.what());
    }
}

void PestanaVentas::agregarCantidadAlCarrito(const std::string& codigo, int cantidad) {
    const Producto& producto = inventario_.buscarPorCodigo(codigo); // lanza si no existe.

    // Se suma en long long (64 bits) aunque cada llamada individual ya
    // venga acotada (el dialogo de cantidad limita a STOCK_MAXIMO, el
    // boton rapido "+" siempre pide 1): si el usuario le da "agregar"
    // muchas veces seguidas al mismo producto, yaEnCarrito podria seguir
    // creciendo, y sumar dos int cercanos al maximo de un int (2,147
    // millones) es un desborde de entero con signo -- comportamiento
    // indefinido en C++, no solo "un numero raro".
    int yaEnCarrito = carrito_.count(codigo) ? carrito_.at(codigo) : 0;
    long long totalSolicitado = static_cast<long long>(yaEnCarrito) + cantidad;
    if (totalSolicitado > producto.getStock()) {
        // Seguro: totalSolicitado <= 2 * Producto::STOCK_MAXIMO en el peor
        // caso (ver Producto.h), muy por debajo del limite de un int.
        throw StockInsuficiente(codigo, producto.getStock(), static_cast<int>(totalSolicitado));
    }

    carrito_[codigo] = static_cast<int>(totalSolicitado);
    actualizarCarrito();
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

        MetodoPago metodoPago = metodoPagoSeleccionado();

        // Solo Fiado necesita un cliente asociado -- para cualquier otro
        // metodo de pago se quedan en "no aplica" (ver Venta.h).
        int clienteId = 0;
        std::string nombreCliente;
        if (metodoPago == MetodoPago::Fiado) {
            SeleccionarClienteDialog dialogoCliente(gestorClientes_, this);
            if (dialogoCliente.exec() != QDialog::Accepted) {
                return; // el usuario cancelo: no se registra la venta.
            }
            clienteId = dialogoCliente.clienteIdSeleccionado();
            const Cliente* cliente = gestorClientes_.buscarPorId(clienteId);
            if (cliente == nullptr) {
                // No deberia pasar (el dialogo solo devuelve ids validos),
                // pero mejor abortar la venta que registrarla sin cliente.
                QMessageBox::warning(this, "No se pudo registrar la venta", "No se encontro el cliente elegido.");
                return;
            }
            nombreCliente = cliente->getNombre();
        }

        const Venta& venta = gestorVentas_.registrarVenta(detalles, metodoPago, clienteId, nombreCliente);

        // Igual que en consola: avisar de inmediato si algun producto
        // vendido quedo con stock bajo (Requisito 4), antes de mostrar el
        // ticket para que el aviso no se pierda de vista.
        QString avisos;
        for (const DetalleVenta& detalle : venta.getDetalles()) {
            const Producto& actualizado = inventario_.buscarPorCodigo(detalle.getCodigoProducto());
            if (actualizado.estaBajoStockMinimo()) {
                avisos += QString("\nAVISO: %1 quedo con stock bajo (%2 unidades, minimo %3).")
                              .arg(QString::fromStdString(actualizado.getNombre()))
                              .arg(actualizado.getStock())
                              .arg(actualizado.getStockMinimo());
            }
        }
        if (!avisos.isEmpty()) {
            QMessageBox::warning(this, "Venta confirmada", avisos.trimmed());
        }

        // El ticket (mismo texto que genera la consola, ver GeneradorTicket.h)
        // se muestra en su propio dialogo, con fuente monoespaciada para que
        // el alineado se vea correcto.
        TicketDialog dialogoTicket(generarTextoTicket(venta, informacionNegocio_), this);
        dialogoTicket.exec();

        carrito_.clear();
        actualizarCarrito();
        botonPagoEfectivo_->setChecked(true); // vuelve al default para la siguiente venta.
        refrescarListaProductos(); // el stock de la izquierda tambien cambio.
        emit datosModificados();
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "No se pudo registrar la venta", e.what());
    }
}

MetodoPago PestanaVentas::metodoPagoSeleccionado() const {
    if (botonPagoTarjetaCredito_->isChecked()) {
        return MetodoPago::TarjetaCredito;
    }
    if (botonPagoTarjetaDebito_->isChecked()) {
        return MetodoPago::TarjetaDebito;
    }
    if (botonPagoFiado_->isChecked()) {
        return MetodoPago::Fiado;
    }
    return MetodoPago::Efectivo; // default / botonPagoEfectivo_ marcado.
}
