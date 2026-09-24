#include "PestanaClientes.h"

#include <QHeaderView>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

namespace {
constexpr int COL_ID = 0;
constexpr int COL_NOMBRE = 1;
constexpr int COL_TELEFONO = 2;
constexpr int COL_SALDO = 3;
} // namespace

PestanaClientes::PestanaClientes(GestorClientes& gestorClientes, GestorCreditos& gestorCreditos,
                                  IRepositorioClientes& repositorioClientes, IRepositorioAbonos& repositorioAbonos,
                                  QWidget* padre)
    : QWidget(padre),
      gestorClientes_(gestorClientes),
      gestorCreditos_(gestorCreditos),
      repositorioClientes_(repositorioClientes),
      repositorioAbonos_(repositorioAbonos) {
    auto* titulo = new QLabel("Clientes", this);
    titulo->setProperty("clase", "titulo");
    auto* explicacion = new QLabel(
        "Clientes de confianza a los que se les vende al fiado (a credito). El saldo pendiente se "
        "calcula solo -- nunca hay que editarlo a mano.",
        this);
    explicacion->setProperty("clase", "secundario");
    explicacion->setWordWrap(true);

    tablaClientes_ = new QTableWidget(this);
    tablaClientes_->setColumnCount(4);
    tablaClientes_->setHorizontalHeaderLabels({"Id", "Nombre", "Telefono", "Saldo pendiente"});
    tablaClientes_->setEditTriggers(QTableWidget::NoEditTriggers);
    tablaClientes_->setSelectionBehavior(QTableWidget::SelectRows);
    tablaClientes_->setSelectionMode(QTableWidget::SingleSelection);
    tablaClientes_->verticalHeader()->setVisible(false);
    tablaClientes_->setAlternatingRowColors(true);
    tablaClientes_->horizontalHeader()->setSectionResizeMode(COL_ID, QHeaderView::ResizeToContents);
    tablaClientes_->horizontalHeader()->setSectionResizeMode(COL_NOMBRE, QHeaderView::Stretch);
    tablaClientes_->horizontalHeader()->setSectionResizeMode(COL_TELEFONO, QHeaderView::ResizeToContents);
    tablaClientes_->horizontalHeader()->setSectionResizeMode(COL_SALDO, QHeaderView::ResizeToContents);

    botonNuevoCliente_ = new QPushButton("Nuevo cliente", this);
    botonNuevoCliente_->setProperty("clase", "primario");
    botonRegistrarAbono_ = new QPushButton("Registrar abono (pago)", this);

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(titulo);
    layout->addWidget(explicacion);
    layout->addWidget(tablaClientes_);
    layout->addWidget(botonNuevoCliente_);
    layout->addWidget(botonRegistrarAbono_);

    connect(botonNuevoCliente_, &QPushButton::clicked, this, &PestanaClientes::alNuevoCliente);
    connect(botonRegistrarAbono_, &QPushButton::clicked, this, &PestanaClientes::alRegistrarAbono);

    actualizar();
}

void PestanaClientes::actualizar() {
    llenarTabla();
}

void PestanaClientes::llenarTabla() {
    std::vector<Cliente> clientes = gestorClientes_.listarTodos();
    tablaClientes_->setRowCount(static_cast<int>(clientes.size()));
    for (int fila = 0; fila < static_cast<int>(clientes.size()); ++fila) {
        const Cliente& cliente = clientes[static_cast<std::size_t>(fila)];
        // El saldo NUNCA se lee de un campo guardado -- se calcula en
        // vivo cada vez (ver GestorCreditos::saldoPendiente).
        double saldo = gestorCreditos_.saldoPendiente(cliente.getId());
        tablaClientes_->setItem(fila, COL_ID, new QTableWidgetItem(QString::number(cliente.getId())));
        tablaClientes_->setItem(fila, COL_NOMBRE, new QTableWidgetItem(QString::fromStdString(cliente.getNombre())));
        tablaClientes_->setItem(fila, COL_TELEFONO,
                                 new QTableWidgetItem(QString::fromStdString(cliente.getTelefono())));
        tablaClientes_->setItem(fila, COL_SALDO, new QTableWidgetItem(QString::number(saldo, 'f', 2)));
    }
}

void PestanaClientes::alNuevoCliente() {
    bool ok = false;
    QString nombre = QInputDialog::getText(this, "Nuevo cliente", "Nombre:", QLineEdit::Normal, "", &ok);
    if (!ok || nombre.trimmed().isEmpty()) {
        return; // cancelado, o sin nombre -- Cliente exige nombre no vacio de todos modos.
    }
    QString telefono =
        QInputDialog::getText(this, "Nuevo cliente", "Telefono (opcional):", QLineEdit::Normal, "", &ok);

    try {
        gestorClientes_.agregarCliente(nombre.toStdString(), telefono.toStdString());
        repositorioClientes_.guardarTodos(gestorClientes_.listarTodos());
        actualizar();
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "No se pudo agregar el cliente", e.what());
    }
}

void PestanaClientes::alRegistrarAbono() {
    int fila = tablaClientes_->currentRow();
    if (fila < 0) {
        QMessageBox::information(this, "Registrar abono", "Selecciona un cliente de la lista primero.");
        return;
    }

    int clienteId = tablaClientes_->item(fila, COL_ID)->text().toInt();
    const Cliente* cliente = gestorClientes_.buscarPorId(clienteId);
    if (cliente == nullptr) {
        return;
    }

    double pendiente = gestorCreditos_.saldoPendiente(clienteId);
    if (pendiente <= 0.0) {
        QMessageBox::information(this, "Registrar abono",
                                  QString::fromStdString(cliente->getNombre()) + " no tiene saldo pendiente.");
        return;
    }

    bool ok = false;
    double monto = QInputDialog::getDouble(
        this, "Registrar abono",
        QString("Saldo pendiente de %1: $%2\nMonto a abonar:")
            .arg(QString::fromStdString(cliente->getNombre()))
            .arg(pendiente, 0, 'f', 2),
        pendiente, 0.01, pendiente, 2, &ok);
    if (!ok) {
        return;
    }

    try {
        // registrarAbono lanza EntradaInvalida si el monto excede el
        // saldo pendiente (ver GestorCreditos.cpp) -- aunque el rango del
        // QInputDialog de arriba ya lo acota, se deja la validacion real
        // en GestorCreditos, no en el widget.
        const Abono& abono = gestorCreditos_.registrarAbono(clienteId, monto);
        try {
            repositorioAbonos_.agregar(abono);
        } catch (const std::exception& e) {
            QMessageBox::warning(this, "Aviso", QString("No se pudo guardar el abono en disco: %1").arg(e.what()));
        }
        actualizar();
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "No se pudo registrar el abono", e.what());
    }
}
