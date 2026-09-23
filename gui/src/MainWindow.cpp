#include "MainWindow.h"
#include "PestanaProductos.h"
#include "PestanaReporte.h"
#include "PestanaVentas.h"
#include "RepositorioProductosCsv.h"
#include "RepositorioVentasCsv.h"

#include <QMessageBox>
#include <QStatusBar>
#include <QTabWidget>

MainWindow::MainWindow(QWidget* padre)
    : QMainWindow(padre),
      gestorVentas_(inventario_),
      repositorioProductos_(std::make_unique<RepositorioProductosCsv>("data/productos.csv")),
      repositorioVentas_(std::make_unique<RepositorioVentasCsv>("data/ventas.csv")) {
    setWindowTitle("Inventario POS");
    // Tamaño inicial mas grande, a tono con la fuente mas grande del tema
    // (ver style.qss): con la ventana chica de antes, la tabla de
    // productos con la fuente nueva se sentia apretada. setMinimumSize
    // evita que el usuario la encoja a un punto en que las tablas dejen de
    // verse bien.
    resize(1280, 840);
    setMinimumSize(1000, 650);

    // Misma carga inicial que main.cpp en la version de consola: si los
    // archivos no existen todavia, cargarTodos()/cargarTodas() devuelven
    // vectores vacios en vez de fallar.
    inventario_.cargarProductos(repositorioProductos_->cargarTodos());
    gestorVentas_.cargarVentas(repositorioVentas_->cargarTodas());

    tabs_ = new QTabWidget(this);
    pestanaProductos_ = new PestanaProductos(inventario_, tabs_);
    pestanaVentas_ = new PestanaVentas(inventario_, gestorVentas_, tabs_);
    pestanaReporte_ = new PestanaReporte(gestorVentas_, tabs_);

    tabs_->addTab(pestanaProductos_, "Productos");
    tabs_->addTab(pestanaVentas_, "Vender");
    tabs_->addTab(pestanaReporte_, "Reporte del dia");
    setCentralWidget(tabs_);

    statusBar()->showMessage("Datos cargados desde data/productos.csv y data/ventas.csv", 5000);

    connect(pestanaProductos_, &PestanaProductos::datosModificados, this, &MainWindow::alGuardarYRefrescar);
    connect(pestanaVentas_, &PestanaVentas::datosModificados, this, &MainWindow::alGuardarYRefrescar);
    connect(tabs_, &QTabWidget::currentChanged, this, &MainWindow::alCambiarPestana);
}

void MainWindow::alGuardarYRefrescar() {
    guardarDatos();
    // Un cambio en Productos puede afectar la lista de Ventas (precio,
    // stock, un producto nuevo/eliminado) y viceversa (una venta cambia el
    // stock que Productos muestra), asi que refrescamos ambas siempre; es
    // barato (son listas en memoria, no hay round-trip a disco de mas).
    pestanaProductos_->refrescar();
    pestanaVentas_->refrescarListaProductos();
}

void MainWindow::alCambiarPestana(int indice) {
    if (tabs_->widget(indice) == pestanaReporte_) {
        pestanaReporte_->actualizar();
    }
}

void MainWindow::guardarDatos() {
    // Misma logica que Menu::guardarDatos() en la version de consola: un
    // fallo al escribir a disco se avisa, no tumba la aplicacion.
    try {
        repositorioProductos_->guardarTodos(inventario_.listarTodos());
        repositorioVentas_->guardarTodas(gestorVentas_.listarVentas());
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Aviso", QString("No se pudieron guardar los datos en disco: %1").arg(e.what()));
    }
}
