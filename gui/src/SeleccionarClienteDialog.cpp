#include "SeleccionarClienteDialog.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QVBoxLayout>

namespace {
// Valor "userData" del combo que representa "quiero dar de alta un
// cliente nuevo" -- ningun Cliente real puede tener este id (GestorClientes
// empieza a asignar ids desde 1), asi que sirve como centinela seguro.
constexpr int ID_NUEVO_CLIENTE = -1;
} // namespace

SeleccionarClienteDialog::SeleccionarClienteDialog(GestorClientes& gestorClientes, QWidget* padre)
    : QDialog(padre), gestorClientes_(gestorClientes) {
    setWindowTitle("Cliente para la venta al fiado");

    auto* etiquetaCombo = new QLabel("Cliente:", this);
    comboClientes_ = new QComboBox(this);
    repoblarCombo();

    campoNombreNuevo_ = new QLineEdit(this);
    campoNombreNuevo_->setMaxLength(Cliente::NOMBRE_LONGITUD_MAXIMA);
    campoTelefonoNuevo_ = new QLineEdit(this);
    campoTelefonoNuevo_->setMaxLength(Cliente::TELEFONO_LONGITUD_MAXIMA);
    campoTelefonoNuevo_->setPlaceholderText("Opcional");

    // Los campos de alta solo se muestran cuando el combo tiene
    // seleccionado "-- Nuevo cliente --" -- panelNuevoCliente_ agrupa
    // ambos campos en un solo widget para poder ocultarlos/mostrarlos de
    // una vez con setVisible(), en vez de uno por uno.
    panelNuevoCliente_ = new QWidget(this);
    auto* formularioNuevo = new QFormLayout(panelNuevoCliente_);
    formularioNuevo->setContentsMargins(0, 0, 0, 0);
    formularioNuevo->addRow("Nombre:", campoNombreNuevo_);
    formularioNuevo->addRow("Telefono:", campoTelefonoNuevo_);

    auto* botones = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(botones, &QDialogButtonBox::accepted, this, &SeleccionarClienteDialog::alAceptar);
    connect(botones, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(etiquetaCombo);
    layout->addWidget(comboClientes_);
    layout->addWidget(panelNuevoCliente_);
    layout->addWidget(botones);

    connect(comboClientes_, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &SeleccionarClienteDialog::alCambiarSeleccion);
    alCambiarSeleccion(comboClientes_->currentIndex());
}

void SeleccionarClienteDialog::repoblarCombo() {
    comboClientes_->clear();
    comboClientes_->addItem("-- Nuevo cliente --", ID_NUEVO_CLIENTE);
    for (const Cliente& cliente : gestorClientes_.listarTodos()) {
        comboClientes_->addItem(QString("%1 - %2").arg(cliente.getId()).arg(QString::fromStdString(cliente.getNombre())),
                                 cliente.getId());
    }
}

void SeleccionarClienteDialog::alCambiarSeleccion(int indice) {
    bool esNuevo = indice < 0 || comboClientes_->itemData(indice).toInt() == ID_NUEVO_CLIENTE;
    panelNuevoCliente_->setVisible(esNuevo);
}

void SeleccionarClienteDialog::alAceptar() {
    int idSeleccionado = comboClientes_->currentData().toInt();

    if (idSeleccionado == ID_NUEVO_CLIENTE) {
        try {
            // El constructor de Cliente (vía agregarCliente) valida
            // nombre/telefono y lanza EntradaInvalida si algo esta mal --
            // se atrapa aqui para mostrarlo como dialogo en vez de cerrar
            // con datos invalidos.
            const Cliente& nuevo =
                gestorClientes_.agregarCliente(campoNombreNuevo_->text().toStdString(),
                                                campoTelefonoNuevo_->text().toStdString());
            clienteIdSeleccionado_ = nuevo.getId();
        } catch (const std::exception& e) {
            QMessageBox::warning(this, "No se pudo dar de alta el cliente", e.what());
            return; // no cierra el dialogo: deja corregir los datos.
        }
    } else {
        clienteIdSeleccionado_ = idSeleccionado;
    }

    accept();
}

int SeleccionarClienteDialog::clienteIdSeleccionado() const {
    return clienteIdSeleccionado_;
}
