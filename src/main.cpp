// main.cpp
//
// Punto de entrada del programa. Crea las "piezas" del sistema
// (Inventario, GestorVentas, repositorios de persistencia), carga los
// datos guardados de ejecuciones anteriores y le pasa el control al Menu.
// Nada de logica de negocio ni de presentacion vive aqui.

#include <memory>

#include "GestorClientes.h"
#include "GestorCortes.h"
#include "GestorCreditos.h"
#include "GestorVentas.h"
#include "Inventario.h"
#include "Menu.h"
#include "RepositorioAbonosCsv.h"
#include "RepositorioClientesCsv.h"
#include "RepositorioCortesCsv.h"
#include "RepositorioInformacionNegocioCsv.h"
#include "RepositorioProductosCsv.h"
#include "RepositorioVentasCsv.h"

int main() {
    Inventario inventario;
    GestorVentas gestorVentas(inventario);
    GestorCortes gestorCortes(gestorVentas);
    GestorClientes gestorClientes;
    GestorCreditos gestorCreditos(gestorVentas);

    // std::unique_ptr<Interfaz> es el reemplazo moderno de un puntero
    // crudo (Interfaz*) a un objeto creado con `new`: es dueno exclusivo
    // del objeto y lo destruye automaticamente cuando sale de alcance (al
    // terminar main), sin necesidad de un `delete` manual. Se guarda como
    // puntero a la INTERFAZ (no a RepositorioProductosCsv) a proposito:
    // asi, para migrar a SQLite el dia de manana, solo cambia esta linea
    // (por `std::make_unique<RepositorioProductosSqlite>(...)`) y nada
    // mas en el programa se entera del cambio.
    std::unique_ptr<IRepositorioProductos> repositorioProductos =
        std::make_unique<RepositorioProductosCsv>("data/productos.csv");
    std::unique_ptr<IRepositorioVentas> repositorioVentas =
        std::make_unique<RepositorioVentasCsv>("data/ventas.csv");
    std::unique_ptr<IRepositorioInformacionNegocio> repositorioInformacionNegocio =
        std::make_unique<RepositorioInformacionNegocioCsv>("data/negocio.csv");
    std::unique_ptr<IRepositorioCortes> repositorioCortes =
        std::make_unique<RepositorioCortesCsv>("data/cortes.csv");
    std::unique_ptr<IRepositorioClientes> repositorioClientes =
        std::make_unique<RepositorioClientesCsv>("data/clientes.csv");
    std::unique_ptr<IRepositorioAbonos> repositorioAbonos =
        std::make_unique<RepositorioAbonosCsv>("data/abonos.csv");

    // Carga inicial: si es la primera vez que se ejecuta el programa y los
    // archivos todavia no existen, cargarTodos()/cargarTodas() devuelven
    // vectores vacios (no lanzan excepcion), asi que esto tambien funciona
    // "en frio".
    inventario.cargarProductos(repositorioProductos->cargarTodos());
    gestorVentas.cargarVentas(repositorioVentas->cargarTodas());
    gestorCortes.cargarCortes(repositorioCortes->cargarTodos());
    gestorClientes.cargarClientes(repositorioClientes->cargarTodos());
    gestorCreditos.cargarAbonos(repositorioAbonos->cargarTodos());

    // `*repositorioProductos` desreferencia el unique_ptr para obtener una
    // IRepositorioProductos& (Menu no necesita ser dueno del repositorio,
    // solo usarlo, igual que con inventario/gestorVentas).
    Menu menu(inventario, gestorVentas, gestorCortes, gestorClientes, gestorCreditos, *repositorioProductos,
              *repositorioVentas, *repositorioInformacionNegocio, *repositorioCortes, *repositorioClientes,
              *repositorioAbonos);
    menu.ejecutar();
    return 0;
}
