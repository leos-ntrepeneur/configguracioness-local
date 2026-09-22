#include "PestanaReporte.h"

#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

PestanaReporte::PestanaReporte(const GestorVentas& gestorVentas, QWidget* padre)
    : QWidget(padre), gestorVentas_(gestorVentas) {
    etiquetaTransacciones_ = new QLabel(this);
    etiquetaTotal_ = new QLabel(this);
    etiquetaTotal_->setStyleSheet("font-weight: bold; font-size: 16px;");

    tablaProductos_ = new QTableWidget(this);
    tablaProductos_->setColumnCount(4);
    tablaProductos_->setHorizontalHeaderLabels({"Codigo", "Nombre", "Unidades", "Total"});
    tablaProductos_->setEditTriggers(QTableWidget::NoEditTriggers);
    tablaProductos_->setSelectionMode(QTableWidget::NoSelection);
    tablaProductos_->horizontalHeader()->setStretchLastSection(true);
    tablaProductos_->verticalHeader()->setVisible(false);

    auto* botonActualizar = new QPushButton("Actualizar", this);
    connect(botonActualizar, &QPushButton::clicked, this, &PestanaReporte::actualizar);

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("Reporte de ventas del dia", this));
    layout->addWidget(etiquetaTransacciones_);
    layout->addWidget(etiquetaTotal_);
    layout->addWidget(new QLabel("Productos mas vendidos:", this));
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
        tablaProductos_->setItem(fila, 0, new QTableWidgetItem(QString::fromStdString(r.codigo)));
        tablaProductos_->setItem(fila, 1, new QTableWidgetItem(QString::fromStdString(r.nombre)));
        tablaProductos_->setItem(fila, 2, new QTableWidgetItem(QString::number(r.cantidadVendida)));
        tablaProductos_->setItem(fila, 3, new QTableWidgetItem(QString::number(r.totalVendido, 'f', 2)));
    }
}
