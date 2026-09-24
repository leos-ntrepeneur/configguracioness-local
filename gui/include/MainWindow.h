// MainWindow.h
//
// Ventana principal de la version grafica. Cumple el mismo papel que
// main.cpp + Menu en la version de consola: es dueña de Inventario,
// GestorVentas y los repositorios de persistencia, arma las pestañas y
// conecta sus señales para guardar/refrescar automaticamente.

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <memory>

#include "GestorClientes.h"
#include "GestorCortes.h"
#include "GestorCreditos.h"
#include "GestorVentas.h"
#include "IRepositorioAbonos.h"
#include "IRepositorioClientes.h"
#include "IRepositorioCortes.h"
#include "IRepositorioInformacionNegocio.h"
#include "IRepositorioProductos.h"
#include "IRepositorioVentas.h"
#include "InformacionNegocio.h"
#include "Inventario.h"

class QTabWidget;
class PestanaProductos;
class PestanaVentas;
class PestanaReporte;
class PestanaInformacionNegocio;
class PestanaCortes;
class PestanaArchivoVentas;
class PestanaClientes;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* padre = nullptr);

private slots:
    // Conectado a la señal datosModificados() de Productos y de Ventas:
    // guarda ambos archivos y refresca las demas pestañas, sin que
    // Productos/Ventas necesiten saber nada de persistencia entre si.
    void alGuardarYRefrescar();

    // Conectado a QTabWidget::currentChanged: si la pestaña que se acaba
    // de mostrar es la de Reporte, la actualiza (evita recalcular el
    // reporte en cada tecla si el usuario nunca la visita).
    void alCambiarPestana(int indice);

private:
    void guardarDatos();

    // MainWindow SI es dueño de estos dos (a diferencia de Menu, que solo
    // los recibia por referencia desde main.cpp): aqui no hay un "main.cpp"
    // separado, asi que MainWindow hace ese papel de punto de ensamblaje.
    Inventario inventario_;
    GestorVentas gestorVentas_;
    // GestorCortes solo necesita GestorVentas (para generarReporteDelDia()
    // al cerrar el dia), pero igual vive aqui y no dentro de GestorVentas:
    // "cerrar el dia" es un concepto distinto de "registrar una venta", y
    // separarlos evita que GestorVentas crezca con responsabilidades que
    // no le tocan.
    GestorCortes gestorCortes_;
    // Idea de negocio: ventas al fiado (a credito) para abarrotes/
    // ferreterias mexicanas -- ver el comentario grande en GestorCreditos.h.
    // GestorClientes es independiente de todo lo demas (ni GestorVentas ni
    // GestorCortes lo necesitan); GestorCreditos si depende de
    // GestorVentas (para sumar lo fiado de cada cliente).
    GestorClientes gestorClientes_;
    GestorCreditos gestorCreditos_;
    // Igual que InformacionNegocio en Menu (consola): un solo registro
    // chico, se mantiene en memoria y se guarda por su repositorio cuando
    // el usuario la edita en PestanaInformacionNegocio.
    InformacionNegocio informacionNegocio_;

    std::unique_ptr<IRepositorioProductos> repositorioProductos_;
    std::unique_ptr<IRepositorioVentas> repositorioVentas_;
    std::unique_ptr<IRepositorioInformacionNegocio> repositorioInformacionNegocio_;
    std::unique_ptr<IRepositorioCortes> repositorioCortes_;
    std::unique_ptr<IRepositorioClientes> repositorioClientes_;
    std::unique_ptr<IRepositorioAbonos> repositorioAbonos_;

    QTabWidget* tabs_;
    PestanaProductos* pestanaProductos_;
    PestanaVentas* pestanaVentas_;
    PestanaReporte* pestanaReporte_;
    PestanaInformacionNegocio* pestanaInformacionNegocio_;
    PestanaCortes* pestanaCortes_;
    PestanaArchivoVentas* pestanaArchivoVentas_;
    PestanaClientes* pestanaClientes_;
};

#endif // MAIN_WINDOW_H
