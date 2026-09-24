#include "PestanaArchivoVentas.h"
#include "GeneradorTicket.h"
#include "TicketDialog.h"

#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QTableWidget>
#include <QVBoxLayout>

#include <algorithm>

namespace {
constexpr int COL_FOLIO = 0;
constexpr int COL_FECHA = 1;
constexpr int COL_METODO_PAGO = 2;
constexpr int COL_UNIDADES = 3;
constexpr int COL_TOTAL = 4;
} // namespace

PestanaArchivoVentas::PestanaArchivoVentas(const GestorVentas& gestorVentas,
                                            const InformacionNegocio& informacionNegocio, QWidget* padre)
    : QWidget(padre), gestorVentas_(gestorVentas), informacionNegocio_(informacionNegocio) {
    auto* titulo = new QLabel("Archivo de ventas", this);
    titulo->setProperty("clase", "titulo");
    auto* explicacion = new QLabel(
        "Historial COMPLETO de ventas, de todos los dias -- a diferencia de \"Reporte del dia\" "
        "(que solo muestra hoy), aqui no se pierde nada: cada venta queda guardada para "
        "siempre. Doble clic en una fila para ver su ticket completo.",
        this);
    explicacion->setProperty("clase", "secundario");
    explicacion->setWordWrap(true);

    etiquetaTotal_ = new QLabel(this);
    etiquetaTotal_->setStyleSheet("font-family: 'Manrope'; font-weight: 800; font-size: 18px; color: #2dd4bf;");

    campoFiltro_ = new QLineEdit(this);
    campoFiltro_->setPlaceholderText("Buscar por folio o fecha (ej. 2026-09 o 12)...");

    tablaVentas_ = new QTableWidget(this);
    tablaVentas_->setColumnCount(5);
    tablaVentas_->setHorizontalHeaderLabels({"Folio", "Fecha y hora", "Metodo de pago", "Unidades", "Total"});
    tablaVentas_->setEditTriggers(QTableWidget::NoEditTriggers);
    tablaVentas_->setSelectionBehavior(QTableWidget::SelectRows);
    tablaVentas_->setSelectionMode(QTableWidget::SingleSelection);
    tablaVentas_->verticalHeader()->setVisible(false);
    tablaVentas_->setAlternatingRowColors(true);
    tablaVentas_->horizontalHeader()->setSectionResizeMode(COL_FOLIO, QHeaderView::ResizeToContents);
    tablaVentas_->horizontalHeader()->setSectionResizeMode(COL_FECHA, QHeaderView::ResizeToContents);
    tablaVentas_->horizontalHeader()->setSectionResizeMode(COL_METODO_PAGO, QHeaderView::Stretch);
    tablaVentas_->horizontalHeader()->setSectionResizeMode(COL_UNIDADES, QHeaderView::ResizeToContents);
    tablaVentas_->horizontalHeader()->setSectionResizeMode(COL_TOTAL, QHeaderView::ResizeToContents);

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(titulo);
    layout->addWidget(explicacion);
    layout->addWidget(etiquetaTotal_);
    layout->addWidget(campoFiltro_);
    layout->addWidget(tablaVentas_);

    connect(campoFiltro_, &QLineEdit::textChanged, this, &PestanaArchivoVentas::alCambiarFiltro);
    connect(tablaVentas_, &QTableWidget::cellDoubleClicked, this, &PestanaArchivoVentas::alVerTicket);

    actualizar();
}

void PestanaArchivoVentas::actualizar() {
    alCambiarFiltro(campoFiltro_->text());
}

void PestanaArchivoVentas::alCambiarFiltro(const QString& texto) {
    std::vector<Venta> ventas = gestorVentas_.listarVentas(); // copia: se ordena para mostrar, sin tocar el historial real.

    // Mas reciente primero: es lo que casi siempre se quiere revisar al
    // abrir un "archivo" de ventas, a diferencia del CSV (que va de mas
    // vieja a mas nueva porque asi se van agregando).
    std::sort(ventas.begin(), ventas.end(),
              [](const Venta& a, const Venta& b) { return a.getNumeroTransaccion() > b.getNumeroTransaccion(); });

    if (!texto.isEmpty()) {
        std::vector<Venta> filtradas;
        for (const Venta& venta : ventas) {
            QString folioTexto = QString::number(venta.getNumeroTransaccion());
            QString fechaTexto = QString::fromStdString(venta.fechaComoTexto());
            if (folioTexto.contains(texto, Qt::CaseInsensitive) || fechaTexto.contains(texto, Qt::CaseInsensitive)) {
                filtradas.push_back(venta);
            }
        }
        ventas = std::move(filtradas);
    }

    llenarTabla(ventas);
}

void PestanaArchivoVentas::llenarTabla(const std::vector<Venta>& ventas) {
    double totalGeneral = 0.0;
    for (const Venta& venta : ventas) {
        totalGeneral += venta.getTotal();
    }
    etiquetaTotal_->setText(
        QString("%1 venta(s) -- Total: $%2").arg(ventas.size()).arg(totalGeneral, 0, 'f', 2));

    tablaVentas_->setRowCount(static_cast<int>(ventas.size()));
    for (int fila = 0; fila < static_cast<int>(ventas.size()); ++fila) {
        const Venta& venta = ventas[static_cast<std::size_t>(fila)];
        int unidades = 0;
        for (const DetalleVenta& detalle : venta.getDetalles()) {
            unidades += detalle.getCantidad();
        }

        tablaVentas_->setItem(fila, COL_FOLIO, new QTableWidgetItem(QString::number(venta.getNumeroTransaccion())));
        tablaVentas_->setItem(fila, COL_FECHA, new QTableWidgetItem(QString::fromStdString(venta.fechaComoTexto())));
        tablaVentas_->setItem(
            fila, COL_METODO_PAGO, new QTableWidgetItem(QString::fromStdString(metodoPagoATexto(venta.getMetodoPago()))));
        tablaVentas_->setItem(fila, COL_UNIDADES, new QTableWidgetItem(QString::number(unidades)));
        tablaVentas_->setItem(fila, COL_TOTAL, new QTableWidgetItem(QString::number(venta.getTotal(), 'f', 2)));
    }
}

void PestanaArchivoVentas::alVerTicket(int fila, int /*columna*/) {
    QTableWidgetItem* item = tablaVentas_->item(fila, COL_FOLIO);
    if (item == nullptr) {
        return;
    }
    int numeroTransaccion = item->text().toInt();
    const Venta* venta = gestorVentas_.buscarPorNumeroTransaccion(numeroTransaccion);
    if (venta == nullptr) {
        return;
    }

    TicketDialog dialogoTicket(generarTextoTicket(*venta, informacionNegocio_), this);
    dialogoTicket.exec();
}
