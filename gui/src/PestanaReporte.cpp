#include "PestanaReporte.h"
#include "GeneradorTicket.h"
#include "GraficaBarras.h"
#include "TicketDialog.h"

#include <QFrame>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QTableWidget>
#include <QVBoxLayout>

PestanaReporte::PestanaReporte(const GestorVentas& gestorVentas, const InformacionNegocio& informacionNegocio,
                                QWidget* padre)
    : QWidget(padre), gestorVentas_(gestorVentas), informacionNegocio_(informacionNegocio) {
    etiquetaTransacciones_ = new QLabel(this);
    etiquetaTransacciones_->setProperty("clase", "secundario");
    etiquetaTotal_ = new QLabel(this);
    etiquetaTotal_->setStyleSheet("font-family: 'Manrope'; font-weight: 800; font-size: 22px; color: #2dd4bf;");

    tablaProductos_ = new QTableWidget(this);
    tablaProductos_->setColumnCount(5);
    tablaProductos_->setHorizontalHeaderLabels({"Codigo", "Nombre", "Unidades", "Precio", "Total"});
    tablaProductos_->setEditTriggers(QTableWidget::NoEditTriggers);
    tablaProductos_->setSelectionMode(QTableWidget::NoSelection);
    tablaProductos_->verticalHeader()->setVisible(false);
    tablaProductos_->setAlternatingRowColors(true);
    tablaProductos_->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    tablaProductos_->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    tablaProductos_->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    tablaProductos_->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    tablaProductos_->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);

    // Tabla de transacciones: folio, fecha y hora, metodo de pago, unidades
    // y total -- mismo contenido que Menu::mostrarHistorialTransacciones()
    // en la consola.
    tablaTransacciones_ = new QTableWidget(this);
    tablaTransacciones_->setColumnCount(5);
    tablaTransacciones_->setHorizontalHeaderLabels({"Folio", "Fecha y hora", "Metodo de pago", "Unidades", "Total"});
    tablaTransacciones_->setEditTriggers(QTableWidget::NoEditTriggers);
    // A diferencia de tablaProductos_ (solo lectura, sin seleccion): esta
    // tabla SI se puede seleccionar/hacer doble clic, para reabrir el
    // ticket de esa transaccion (ver alVerTicketTransaccion).
    tablaTransacciones_->setSelectionBehavior(QTableWidget::SelectRows);
    tablaTransacciones_->setSelectionMode(QTableWidget::SingleSelection);
    tablaTransacciones_->verticalHeader()->setVisible(false);
    tablaTransacciones_->setAlternatingRowColors(true);
    tablaTransacciones_->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    tablaTransacciones_->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    tablaTransacciones_->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    tablaTransacciones_->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    tablaTransacciones_->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);

    // La grafica no lleva boton ni pestana propia -- se repuebla junto con
    // las tablas en actualizar(), asi que aparece "automaticamente" apenas
    // se entra a esta pestana o se registra una venta nueva.
    graficaCategorias_ = new GraficaBarras(this);

    auto* botonActualizar = new QPushButton("Actualizar", this);
    connect(botonActualizar, &QPushButton::clicked, this, &PestanaReporte::actualizar);

    auto* tituloProductos = new QLabel("Productos mas vendidos:", this);
    tituloProductos->setProperty("clase", "titulo");

    auto* tituloTransacciones = new QLabel("Historial de transacciones:", this);
    tituloTransacciones->setProperty("clase", "titulo");
    auto* ayudaTransacciones = new QLabel("Doble clic en una fila para ver su ticket completo.", this);
    ayudaTransacciones->setProperty("clase", "secundario");

    auto* tituloCategorias = new QLabel("Ventas por categoria:", this);
    tituloCategorias->setProperty("clase", "titulo");

    // Con dos tablas mas la grafica, el contenido puede superar el alto
    // de la ventana en pantallas chicas -- se envuelve todo en un
    // QScrollArea (en vez de dejarlo suelto en `this`) para que aparezca
    // una barra de desplazamiento vertical en vez de recortar contenido.
    auto* contenido = new QWidget();
    auto* layout = new QVBoxLayout(contenido);
    layout->addWidget(etiquetaTransacciones_);
    layout->addWidget(etiquetaTotal_);
    layout->addSpacing(8);
    layout->addWidget(tituloProductos);
    layout->addWidget(tablaProductos_);
    layout->addSpacing(8);
    layout->addWidget(tituloTransacciones);
    layout->addWidget(ayudaTransacciones);
    layout->addWidget(tablaTransacciones_);
    layout->addSpacing(8);
    layout->addWidget(tituloCategorias);
    layout->addWidget(graficaCategorias_);
    layout->addWidget(botonActualizar);

    auto* areaDesplazable = new QScrollArea(this);
    areaDesplazable->setWidget(contenido);
    areaDesplazable->setWidgetResizable(true);
    areaDesplazable->setFrameShape(QFrame::NoFrame);

    auto* layoutPrincipal = new QVBoxLayout(this);
    layoutPrincipal->setContentsMargins(0, 0, 0, 0);
    layoutPrincipal->addWidget(areaDesplazable);

    connect(tablaTransacciones_, &QTableWidget::cellDoubleClicked, this,
            &PestanaReporte::alVerTicketTransaccion);

    actualizar();
}

void PestanaReporte::actualizar() {
    ReporteVentasDia reporte = gestorVentas_.generarReporteDelDia();

    etiquetaTransacciones_->setText(QString("Transacciones: %1").arg(reporte.numeroTransacciones));
    etiquetaTotal_->setText(QString("Total vendido: $%1").arg(reporte.totalVendido, 0, 'f', 2));

    tablaProductos_->setRowCount(static_cast<int>(reporte.productosMasVendidos.size()));
    for (int fila = 0; fila < static_cast<int>(reporte.productosMasVendidos.size()); ++fila) {
        const ResumenProducto& r = reporte.productosMasVendidos[static_cast<std::size_t>(fila)];
        // Precio unitario promedio del dia (ver comentario equivalente en
        // Menu::alVerReporteVentasDelDia): se calcula, no se guarda, para
        // reflejar lo realmente cobrado aunque el precio haya cambiado.
        double precioPromedio = r.cantidadVendida > 0 ? r.totalVendido / r.cantidadVendida : 0.0;
        tablaProductos_->setItem(fila, 0, new QTableWidgetItem(QString::fromStdString(r.codigo)));
        tablaProductos_->setItem(fila, 1, new QTableWidgetItem(QString::fromStdString(r.nombre)));
        tablaProductos_->setItem(fila, 2, new QTableWidgetItem(QString::number(r.cantidadVendida)));
        tablaProductos_->setItem(fila, 3, new QTableWidgetItem(QString::number(precioPromedio, 'f', 2)));
        tablaProductos_->setItem(fila, 4, new QTableWidgetItem(QString::number(r.totalVendido, 'f', 2)));
    }

    tablaTransacciones_->setRowCount(static_cast<int>(reporte.transacciones.size()));
    for (int fila = 0; fila < static_cast<int>(reporte.transacciones.size()); ++fila) {
        const TransaccionDia& t = reporte.transacciones[static_cast<std::size_t>(fila)];
        tablaTransacciones_->setItem(fila, 0, new QTableWidgetItem(QString::number(t.numeroTransaccion)));
        tablaTransacciones_->setItem(fila, 1, new QTableWidgetItem(QString::fromStdString(t.fechaHoraTexto)));
        tablaTransacciones_->setItem(fila, 2,
                                      new QTableWidgetItem(QString::fromStdString(metodoPagoATexto(t.metodoPago))));
        tablaTransacciones_->setItem(fila, 3, new QTableWidgetItem(QString::number(t.cantidadProductos)));
        tablaTransacciones_->setItem(fila, 4, new QTableWidgetItem(QString::number(t.total, 'f', 2)));
    }

    std::vector<GraficaBarras::Barra> barras;
    barras.reserve(reporte.ventasPorCategoria.size());
    for (const ResumenCategoria& c : reporte.ventasPorCategoria) {
        GraficaBarras::Barra barra;
        barra.etiqueta = QString::fromStdString(c.categoria);
        barra.valor = c.totalVendido;
        barra.valorTexto = QString("$%1").arg(c.totalVendido, 0, 'f', 2);
        barras.push_back(barra);
    }
    graficaCategorias_->setDatos(std::move(barras));
}

void PestanaReporte::alVerTicketTransaccion(int fila, int /*columna*/) {
    QTableWidgetItem* item = tablaTransacciones_->item(fila, 0); // columna 0 = folio.
    if (item == nullptr) {
        return;
    }
    int numeroTransaccion = item->text().toInt();
    const Venta* venta = gestorVentas_.buscarPorNumeroTransaccion(numeroTransaccion);
    if (venta == nullptr) {
        return; // no deberia pasar (el folio viene de la misma tabla), pero por si acaso.
    }

    TicketDialog dialogoTicket(generarTextoTicket(*venta, informacionNegocio_), this);
    dialogoTicket.exec();
}
