#include "PestanaProductos.h"
#include "ProductoDialog.h"

#include <QColor>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

namespace {
constexpr int COL_CODIGO = 0;
constexpr int COL_NOMBRE = 1;
constexpr int COL_PRECIO = 2;
constexpr int COL_STOCK = 3;
constexpr int COL_CATEGORIA = 4;
constexpr int COL_STOCK_MINIMO = 5;
} // namespace

PestanaProductos::PestanaProductos(Inventario& inventario, QWidget* padre)
    : QWidget(padre), inventario_(inventario) {
    campoBusqueda_ = new QLineEdit(this);
    campoBusqueda_->setPlaceholderText("Buscar por nombre o codigo...");

    etiquetaAlerta_ = new QLabel(this);
    // Rojo pensado para verse bien sobre fondo oscuro (el mismo tono
    // "peligro" que usan los botones destructivos, ver style.qss).
    etiquetaAlerta_->setStyleSheet("color: #ed4245; font-weight: bold;");

    tabla_ = new QTableWidget(this);
    tabla_->setColumnCount(6);
    tabla_->setHorizontalHeaderLabels({"Codigo", "Nombre", "Precio", "Stock", "Categoria", "Stock min."});
    // NoEditTriggers: la tabla es de solo lectura para el usuario (para
    // editar un producto se usa el boton "Editar" + el dialogo, no
    // escribiendo directo en la celda). SelectRows: al hacer clic se
    // selecciona la fila completa, no una celda suelta.
    tabla_->setEditTriggers(QTableWidget::NoEditTriggers);
    tabla_->setSelectionBehavior(QTableWidget::SelectRows);
    tabla_->setSelectionMode(QTableWidget::SingleSelection);
    tabla_->verticalHeader()->setVisible(false);
    tabla_->setAlternatingRowColors(true); // filas alternas mas claras, ver style.qss.

    // La columna Nombre es la unica que "estira" para llenar el espacio
    // sobrante; el resto se ajusta a su contenido. Sin esto, Qt reparte el
    // ancho a partes iguales y los nombres largos se truncan con "...".
    tabla_->horizontalHeader()->setSectionResizeMode(COL_CODIGO, QHeaderView::ResizeToContents);
    tabla_->horizontalHeader()->setSectionResizeMode(COL_NOMBRE, QHeaderView::Stretch);
    tabla_->horizontalHeader()->setSectionResizeMode(COL_PRECIO, QHeaderView::ResizeToContents);
    tabla_->horizontalHeader()->setSectionResizeMode(COL_STOCK, QHeaderView::ResizeToContents);
    tabla_->horizontalHeader()->setSectionResizeMode(COL_CATEGORIA, QHeaderView::ResizeToContents);
    tabla_->horizontalHeader()->setSectionResizeMode(COL_STOCK_MINIMO, QHeaderView::ResizeToContents);

    auto* botonNuevo = new QPushButton("Nuevo", this);
    auto* botonEditar = new QPushButton("Editar", this);
    auto* botonEliminar = new QPushButton("Eliminar", this);
    // "clase" es una propiedad Qt arbitraria (no existe en QPushButton por
    // defecto); se la inventamos solo para que style.qss pueda distinguir
    // botones por selector de atributo (QPushButton[clase="primario"]),
    // igual que se usaria una clase CSS en HTML.
    botonNuevo->setProperty("clase", "primario");
    botonEliminar->setProperty("clase", "peligro");

    auto* filaBotones = new QHBoxLayout();
    filaBotones->addWidget(botonNuevo);
    filaBotones->addWidget(botonEditar);
    filaBotones->addWidget(botonEliminar);
    filaBotones->addStretch(); // empuja los botones a la izquierda.

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(campoBusqueda_);
    layout->addWidget(etiquetaAlerta_);
    layout->addWidget(tabla_);
    layout->addLayout(filaBotones);

    // connect(emisor, señal, receptor, metodo-o-lambda): la sintaxis
    // moderna de Qt (con &Clase::miembro en vez de nombres como texto)
    // permite que el compilador revise en tiempo de compilacion que la
    // señal y el slot existen y son compatibles -- versiones viejas de Qt
    // usaban macros con strings (SIGNAL/SLOT) que solo fallaban en tiempo
    // de ejecucion si te equivocabas de nombre.
    connect(botonNuevo, &QPushButton::clicked, this, &PestanaProductos::alHacerNuevo);
    connect(botonEditar, &QPushButton::clicked, this, &PestanaProductos::alHacerEditar);
    connect(botonEliminar, &QPushButton::clicked, this, &PestanaProductos::alHacerEliminar);
    connect(campoBusqueda_, &QLineEdit::textChanged, this, &PestanaProductos::alCambiarBusqueda);

    refrescar();
}

void PestanaProductos::refrescar() {
    if (campoBusqueda_->text().isEmpty()) {
        llenarTabla(inventario_.listarTodos());
    } else {
        alCambiarBusqueda(campoBusqueda_->text());
    }
}

void PestanaProductos::llenarTabla(const std::vector<Producto>& productos) {
    tabla_->setRowCount(static_cast<int>(productos.size()));

    int filasConStockBajo = 0;
    for (int fila = 0; fila < static_cast<int>(productos.size()); ++fila) {
        const Producto& p = productos[static_cast<std::size_t>(fila)];

        tabla_->setItem(fila, COL_CODIGO, new QTableWidgetItem(QString::fromStdString(p.getCodigo())));
        tabla_->setItem(fila, COL_NOMBRE, new QTableWidgetItem(QString::fromStdString(p.getNombre())));
        tabla_->setItem(fila, COL_PRECIO, new QTableWidgetItem(QString::number(p.getPrecio(), 'f', 2)));
        tabla_->setItem(fila, COL_STOCK, new QTableWidgetItem(QString::number(p.getStock())));
        tabla_->setItem(fila, COL_CATEGORIA, new QTableWidgetItem(QString::fromStdString(p.getCategoria())));
        tabla_->setItem(fila, COL_STOCK_MINIMO, new QTableWidgetItem(QString::number(p.getStockMinimo())));

        if (p.estaBajoStockMinimo()) {
            ++filasConStockBajo;
            // Pinta toda la fila de un rojo oscuro (mismo tono que el
            // resto de las alertas del tema) para que salte a la vista
            // sin tener que leer numero por numero (Requisito 4). El
            // color del TEXTO no se toca aqui: lo sigue controlando
            // style.qss (QTableWidget::item { color: ... }), setBackground
            // solo cambia el relleno de la celda.
            QColor colorAlerta(92, 43, 47);
            for (int columna = 0; columna < tabla_->columnCount(); ++columna) {
                tabla_->item(fila, columna)->setBackground(colorAlerta);
            }
        }
    }

    etiquetaAlerta_->setText(filasConStockBajo > 0
                                  ? QString("%1 producto(s) con stock bajo").arg(filasConStockBajo)
                                  : "");
}

QString PestanaProductos::codigoSeleccionado() const {
    int fila = tabla_->currentRow();
    if (fila < 0) {
        return QString();
    }
    return tabla_->item(fila, COL_CODIGO)->text();
}

void PestanaProductos::alHacerNuevo() {
    ProductoDialog dialogo(ProductoDialog::Modo::Nuevo, nullptr, this);
    if (dialogo.exec() != QDialog::Accepted) {
        return;
    }
    try {
        Producto nuevo = dialogo.obtenerProducto();
        inventario_.agregarProducto(nuevo);
        refrescar();
        emit datosModificados();
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "No se pudo agregar el producto", e.what());
    }
}

void PestanaProductos::alHacerEditar() {
    QString codigo = codigoSeleccionado();
    if (codigo.isEmpty()) {
        QMessageBox::information(this, "Editar producto", "Selecciona un producto de la tabla primero.");
        return;
    }
    try {
        const Producto& actual = inventario_.buscarPorCodigo(codigo.toStdString());
        ProductoDialog dialogo(ProductoDialog::Modo::Editar, &actual, this);
        if (dialogo.exec() != QDialog::Accepted) {
            return;
        }
        Producto editado = dialogo.obtenerProducto();
        inventario_.editarProducto(editado.getCodigo(), editado.getNombre(), editado.getPrecio(),
                                    editado.getCategoria(), editado.getStockMinimo());
        refrescar();
        emit datosModificados();
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "No se pudo editar el producto", e.what());
    }
}

void PestanaProductos::alHacerEliminar() {
    QString codigo = codigoSeleccionado();
    if (codigo.isEmpty()) {
        QMessageBox::information(this, "Eliminar producto", "Selecciona un producto de la tabla primero.");
        return;
    }
    auto respuesta = QMessageBox::question(this, "Confirmar eliminacion",
                                            "¿Eliminar el producto " + codigo + "?");
    if (respuesta != QMessageBox::Yes) {
        return;
    }
    try {
        inventario_.eliminarProducto(codigo.toStdString());
        refrescar();
        emit datosModificados();
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "No se pudo eliminar el producto", e.what());
    }
}

void PestanaProductos::alCambiarBusqueda(const QString& texto) {
    std::string textoBuscado = texto.toStdString();
    if (textoBuscado.empty()) {
        llenarTabla(inventario_.listarTodos());
        return;
    }
    if (inventario_.existeCodigo(textoBuscado)) {
        llenarTabla({inventario_.buscarPorCodigo(textoBuscado)});
        return;
    }
    llenarTabla(inventario_.buscarPorNombre(textoBuscado));
}
