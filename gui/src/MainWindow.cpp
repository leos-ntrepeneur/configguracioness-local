#include "MainWindow.h"
#include "PestanaArchivoVentas.h"
#include "PestanaClientes.h"
#include "PestanaCortes.h"
#include "PestanaInformacionNegocio.h"
#include "PestanaProductos.h"
#include "PestanaReporte.h"
#include "PestanaVentas.h"
#include "RepositorioAbonosCsv.h"
#include "RepositorioClientesCsv.h"
#include "RepositorioCortesCsv.h"
#include "RepositorioInformacionNegocioCsv.h"
#include "RepositorioProductosCsv.h"
#include "RepositorioVentasCsv.h"

#include <QMessageBox>
#include <QStatusBar>
#include <QTabWidget>

MainWindow::MainWindow(QWidget* padre)
    : QMainWindow(padre),
      gestorVentas_(inventario_),
      gestorCortes_(gestorVentas_),
      gestorCreditos_(gestorVentas_),
      repositorioProductos_(std::make_unique<RepositorioProductosCsv>("data/productos.csv")),
      repositorioVentas_(std::make_unique<RepositorioVentasCsv>("data/ventas.csv")),
      repositorioInformacionNegocio_(std::make_unique<RepositorioInformacionNegocioCsv>("data/negocio.csv")),
      repositorioCortes_(std::make_unique<RepositorioCortesCsv>("data/cortes.csv")),
      repositorioClientes_(std::make_unique<RepositorioClientesCsv>("data/clientes.csv")),
      repositorioAbonos_(std::make_unique<RepositorioAbonosCsv>("data/abonos.csv")) {
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
    gestorCortes_.cargarCortes(repositorioCortes_->cargarTodos());
    gestorClientes_.cargarClientes(repositorioClientes_->cargarTodos());
    gestorCreditos_.cargarAbonos(repositorioAbonos_->cargarTodos());
    informacionNegocio_ = repositorioInformacionNegocio_->cargar();

    tabs_ = new QTabWidget(this);
    pestanaProductos_ = new PestanaProductos(inventario_, tabs_);
    pestanaVentas_ = new PestanaVentas(inventario_, gestorVentas_, gestorClientes_, informacionNegocio_, tabs_);
    pestanaReporte_ = new PestanaReporte(gestorVentas_, informacionNegocio_, tabs_);
    pestanaInformacionNegocio_ =
        new PestanaInformacionNegocio(informacionNegocio_, *repositorioInformacionNegocio_, tabs_);
    pestanaCortes_ = new PestanaCortes(gestorCortes_, *repositorioCortes_, informacionNegocio_, tabs_);
    pestanaArchivoVentas_ = new PestanaArchivoVentas(gestorVentas_, informacionNegocio_, tabs_);
    pestanaClientes_ =
        new PestanaClientes(gestorClientes_, gestorCreditos_, *repositorioClientes_, *repositorioAbonos_, tabs_);

    tabs_->addTab(pestanaProductos_, "Productos");
    tabs_->addTab(pestanaVentas_, "Vender");
    tabs_->addTab(pestanaReporte_, "Reporte del dia");
    tabs_->addTab(pestanaInformacionNegocio_, "Mi negocio");
    tabs_->addTab(pestanaCortes_, "Cortes de caja");
    tabs_->addTab(pestanaArchivoVentas_, "Archivo de ventas");
    tabs_->addTab(pestanaClientes_, "Clientes");
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
    pestanaArchivoVentas_->actualizar();
    // Una venta al fiado pudo haber dado de alta un cliente nuevo desde
    // PestanaVentas (ver SeleccionarClienteDialog) -- se refresca tambien
    // para que la pestaña "Clientes" lo muestre sin tener que cambiar de
    // pestaña primero.
    pestanaClientes_->actualizar();
}

void MainWindow::alCambiarPestana(int indice) {
    if (tabs_->widget(indice) == pestanaReporte_) {
        pestanaReporte_->actualizar();
    } else if (tabs_->widget(indice) == pestanaArchivoVentas_) {
        pestanaArchivoVentas_->actualizar();
    } else if (tabs_->widget(indice) == pestanaClientes_) {
        pestanaClientes_->actualizar();
    }
}

void MainWindow::guardarDatos() {
    // Misma logica que Menu::guardarDatos() en la version de consola: un
    // fallo al escribir a disco se avisa, no tumba la aplicacion.
    try {
        repositorioProductos_->guardarTodos(inventario_.listarTodos());
        repositorioVentas_->guardarTodas(gestorVentas_.listarVentas());
        repositorioClientes_->guardarTodos(gestorClientes_.listarTodos());
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Aviso", QString("No se pudieron guardar los datos en disco: %1").arg(e.what()));
    }
}
