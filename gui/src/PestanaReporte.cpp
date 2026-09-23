#include "PestanaReporte.h"

#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

PestanaReporte::PestanaReporte(const GestorVentas& gestorVentas, QWidget* padre)
    : QWidget(padre), gestorVentas_(gestorVentas) {
    etiquetaTransacciones_ = new QLabel(this);
    etiquetaTransacciones_->setProperty("clase", "secundario");
    etiquetaTotal_ = new QLabel(this);
    etiquetaTotal_->setStyleSheet("font-family: 'Manrope'; font-weight: 800; font-size: 22px; color: #6b76f5;");

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

    auto* botonActualizar = new QPushButton("Actualizar", this);
    connect(botonActualizar, &QPushButton::clicked, this, &PestanaReporte::actualizar);

    auto* tituloProductos = new QLabel("Productos mas vendidos:", this);
    tituloProductos->setProperty("clase", "titulo");

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(etiquetaTransacciones_);
    layout->addWidget(etiquetaTotal_);
    layout->addSpacing(8);
    layout->addWidget(tituloProductos);
    layout->addWidget(tablaProductos_);
    layout->addWidget(botonActualizar);

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
}
