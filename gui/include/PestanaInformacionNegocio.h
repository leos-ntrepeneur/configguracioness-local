// PestanaInformacionNegocio.h
//
// Pestana "Mi negocio": formulario para capturar los datos que aparecen
// en el encabezado de los tickets (nombre, direccion, telefono, RFC). Es
// el equivalente grafico de Menu::alConfigurarInformacionNegocio() en la
// consola, pero como formulario siempre visible en vez de preguntas por
// teclado -- aqui no tiene sentido el patron "Enter para dejar igual"
// porque los campos ya se ven precargados en pantalla.

#ifndef PESTANA_INFORMACION_NEGOCIO_H
#define PESTANA_INFORMACION_NEGOCIO_H

#include <QWidget>

#include "IRepositorioInformacionNegocio.h"
#include "InformacionNegocio.h"

class QLineEdit;

class PestanaInformacionNegocio : public QWidget {
    Q_OBJECT

public:
    // Recibe la InformacionNegocio por REFERENCIA, dueña de MainWindow --
    // mismo patron que Inventario/GestorVentas en las demas pestañas: esta
    // pestaña la edita y la guarda, pero PestanaVentas lee la MISMA
    // instancia (no una copia) para armar el ticket, asi que un cambio
    // guardado aqui se ve de inmediato en la siguiente venta.
    PestanaInformacionNegocio(InformacionNegocio& informacionNegocio,
                               IRepositorioInformacionNegocio& repositorio,
                               QWidget* padre = nullptr);

private slots:
    void alGuardar();

private:
    InformacionNegocio& informacionNegocio_;
    IRepositorioInformacionNegocio& repositorio_;

    QLineEdit* campoNombre_;
    QLineEdit* campoDireccion_;
    QLineEdit* campoTelefono_;
    QLineEdit* campoRfc_;
};

#endif // PESTANA_INFORMACION_NEGOCIO_H
