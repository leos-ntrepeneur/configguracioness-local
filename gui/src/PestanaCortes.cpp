#include "PestanaCortes.h"
#include "TicketDialog.h"

#include "GeneradorCorte.h"

#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

namespace {
constexpr int COL_NUMERO = 0;
constexpr int COL_CIERRE = 1;
constexpr int COL_FOLIOS = 2;
constexpr int COL_TRANSACCIONES = 3;
constexpr int COL_TOTAL = 4;
} // namespace

PestanaCortes::PestanaCortes(GestorCortes& gestorCortes, IRepositorioCortes& repositorioCortes,
                              const InformacionNegocio& informacionNegocio, QWidget* padre)
    : QWidget(padre),
      gestorCortes_(gestorCortes),
      repositorioCortes_(repositorioCortes),
      informacionNegocio_(informacionNegocio) {
    auto* titulo = new QLabel("Cortes de caja", this);
    titulo->setProperty("clase", "titulo");
    auto* explicacion = new QLabel(
        "Un corte cierra el dia con un resumen PERMANENTE de ventas (folios, total por "
        "efectivo/tarjeta). No se puede deshacer y no modifica las ventas ya registradas. "
        "Doble clic en una fila para ver su detalle completo.",
        this);
    explicacion->setProperty("clase", "secundario");
    explicacion->setWordWrap(true);

    tablaCortes_ = new QTableWidget(this);
    tablaCortes_->setColumnCount(5);
    tablaCortes_->setHorizontalHeaderLabels({"Corte", "Cierre", "Folios", "Transacciones", "Total"});
    tablaCortes_->setEditTriggers(QTableWidget::NoEditTriggers);
    tablaCortes_->setSelectionBehavior(QTableWidget::SelectRows);
    tablaCortes_->setSelectionMode(QTableWidget::SingleSelection);
    tablaCortes_->verticalHeader()->setVisible(false);
    tablaCortes_->setAlternatingRowColors(true);
    tablaCortes_->horizontalHeader()->setSectionResizeMode(COL_NUMERO, QHeaderView::ResizeToContents);
    tablaCortes_->horizontalHeader()->setSectionResizeMode(COL_CIERRE, QHeaderView::Stretch);
    tablaCortes_->horizontalHeader()->setSectionResizeMode(COL_FOLIOS, QHeaderView::ResizeToContents);
    tablaCortes_->horizontalHeader()->setSectionResizeMode(COL_TRANSACCIONES, QHeaderView::ResizeToContents);
    tablaCortes_->horizontalHeader()->setSectionResizeMode(COL_TOTAL, QHeaderView::ResizeToContents);

    botonCerrarDia_ = new QPushButton("Cerrar el dia (corte de caja)", this);
    botonCerrarDia_->setProperty("clase", "primario");

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(titulo);
    layout->addWidget(explicacion);
    layout->addWidget(tablaCortes_);
    layout->addWidget(botonCerrarDia_);

    connect(botonCerrarDia_, &QPushButton::clicked, this, &PestanaCortes::alCerrarDia);
    connect(tablaCortes_, &QTableWidget::cellDoubleClicked, this, &PestanaCortes::alVerDetalle);

    actualizar();
}

void PestanaCortes::actualizar() {
    llenarTabla();
}

void PestanaCortes::llenarTabla() {
    const std::vector<CorteCaja>& cortes = gestorCortes_.listarCortes();
    tablaCortes_->setRowCount(static_cast<int>(cortes.size()));
    for (int fila = 0; fila < static_cast<int>(cortes.size()); ++fila) {
        const CorteCaja& corte = cortes[static_cast<std::size_t>(fila)];
        QString folios = corte.getNumeroTransacciones() > 0
                              ? QString("%1-%2").arg(corte.getFolioInicial()).arg(corte.getFolioFinal())
                              : "-";
        tablaCortes_->setItem(fila, COL_NUMERO, new QTableWidgetItem(QString::number(corte.getNumeroCorte())));
        tablaCortes_->setItem(fila, COL_CIERRE,
                               new QTableWidgetItem(QString::fromStdString(corte.fechaHoraCierreComoTexto())));
        tablaCortes_->setItem(fila, COL_FOLIOS, new QTableWidgetItem(folios));
        tablaCortes_->setItem(fila, COL_TRANSACCIONES,
                               new QTableWidgetItem(QString::number(corte.getNumeroTransacciones())));
        tablaCortes_->setItem(fila, COL_TOTAL, new QTableWidgetItem(QString::number(corte.getTotalVendido(), 'f', 2)));
    }
}

void PestanaCortes::alCerrarDia() {
    auto respuesta = QMessageBox::question(
        this, "Cerrar el dia",
        "Esto genera un registro PERMANENTE con el resumen de las ventas de hoy "
        "(folios incluidos, total por efectivo/tarjeta). No modifica las ventas ya "
        "registradas -- es solo un resumen archivado, y no se puede deshacer.\n\n"
        "Confirmas el cierre del dia?",
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (respuesta != QMessageBox::Yes) {
        return;
    }

    const CorteCaja& corte = gestorCortes_.cerrarDia();
    try {
        repositorioCortes_.agregar(corte);
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Aviso", QString("No se pudo guardar el corte en disco: %1").arg(e.what()));
    }

    actualizar();

    TicketDialog dialogoCorte(generarTextoCorte(corte, informacionNegocio_), this, "Corte de caja");
    dialogoCorte.exec();
}

void PestanaCortes::alVerDetalle(int fila, int /*columna*/) {
    QTableWidgetItem* item = tablaCortes_->item(fila, COL_NUMERO);
    if (item == nullptr) {
        return;
    }
    int numeroCorte = item->text().toInt();
    const CorteCaja* corte = gestorCortes_.buscarPorNumeroCorte(numeroCorte);
    if (corte == nullptr) {
        return;
    }

    TicketDialog dialogoCorte(generarTextoCorte(*corte, informacionNegocio_), this, "Corte de caja");
    dialogoCorte.exec();
}
