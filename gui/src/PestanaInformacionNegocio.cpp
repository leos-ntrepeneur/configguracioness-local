#include "PestanaInformacionNegocio.h"

#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

PestanaInformacionNegocio::PestanaInformacionNegocio(InformacionNegocio& informacionNegocio,
                                                       IRepositorioInformacionNegocio& repositorio,
                                                       QWidget* padre)
    : QWidget(padre), informacionNegocio_(informacionNegocio), repositorio_(repositorio) {
    auto* titulo = new QLabel("Informacion del local", this);
    titulo->setProperty("clase", "titulo");
    auto* explicacion = new QLabel(
        "Estos datos aparecen en el encabezado de los tickets de venta (pestana Vender).", this);
    explicacion->setProperty("clase", "secundario");
    explicacion->setWordWrap(true);

    campoNombre_ = new QLineEdit(this);
    campoNombre_->setMaxLength(InformacionNegocio::LONGITUD_MAXIMA);
    campoDireccion_ = new QLineEdit(this);
    campoDireccion_->setMaxLength(InformacionNegocio::LONGITUD_MAXIMA);
    campoTelefono_ = new QLineEdit(this);
    campoTelefono_->setMaxLength(InformacionNegocio::LONGITUD_MAXIMA);
    campoRfc_ = new QLineEdit(this);
    campoRfc_->setMaxLength(InformacionNegocio::LONGITUD_MAXIMA);
    // El RFC aqui es SOLO un texto de ejemplo que se imprime en el ticket,
    // como en cualquier ticket de tienda -- no se valida contra el formato
    // real del SAT ni habilita facturacion electronica (ver el comentario
    // grande en InformacionNegocio.h). El placeholder deja eso claro sin
    // que el usuario tenga que adivinarlo.
    campoRfc_->setPlaceholderText("Ejemplo: ABC010101AB1 (solo para el ticket, no es facturacion real)");

    campoNombre_->setText(QString::fromStdString(informacionNegocio_.getNombre()));
    campoDireccion_->setText(QString::fromStdString(informacionNegocio_.getDireccion()));
    campoTelefono_->setText(QString::fromStdString(informacionNegocio_.getTelefono()));
    campoRfc_->setText(QString::fromStdString(informacionNegocio_.getRfc()));

    auto* formulario = new QFormLayout();
    formulario->addRow("Nombre del negocio:", campoNombre_);
    formulario->addRow("Direccion:", campoDireccion_);
    formulario->addRow("Telefono:", campoTelefono_);
    formulario->addRow("RFC:", campoRfc_);

    auto* botonGuardar = new QPushButton("Guardar", this);
    botonGuardar->setProperty("clase", "primario");
    connect(botonGuardar, &QPushButton::clicked, this, &PestanaInformacionNegocio::alGuardar);

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(titulo);
    layout->addWidget(explicacion);
    layout->addSpacing(8);
    layout->addLayout(formulario);
    layout->addWidget(botonGuardar);
    layout->addStretch();
}

void PestanaInformacionNegocio::alGuardar() {
    try {
        // Se construye un objeto TEMPORAL primero (el constructor valida
        // los 4 campos de una vez) en vez de llamar los setters uno por
        // uno sobre informacionNegocio_ directamente: como esa instancia es
        // compartida en vivo con PestanaVentas (ver el comentario en el
        // .h), mutarla campo por campo dejaria datos a medio actualizar
        // visibles en el siguiente ticket si un campo posterior fallara la
        // validacion.
        InformacionNegocio nueva(campoNombre_->text().toStdString(), campoDireccion_->text().toStdString(),
                                  campoTelefono_->text().toStdString(), campoRfc_->text().toStdString());

        repositorio_.guardar(nueva);
        informacionNegocio_ = nueva;
        QMessageBox::information(this, "Informacion del local", "Datos guardados correctamente.");
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "No se pudo guardar", e.what());
    }
}
