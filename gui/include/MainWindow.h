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

#include "GestorVentas.h"
#include "IRepositorioProductos.h"
#include "IRepositorioVentas.h"
#include "Inventario.h"

class QTabWidget;
class PestanaProductos;
class PestanaVentas;
class PestanaReporte;

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

    std::unique_ptr<IRepositorioProductos> repositorioProductos_;
    std::unique_ptr<IRepositorioVentas> repositorioVentas_;

    QTabWidget* tabs_;
    PestanaProductos* pestanaProductos_;
    PestanaVentas* pestanaVentas_;
    PestanaReporte* pestanaReporte_;
};

#endif // MAIN_WINDOW_H
