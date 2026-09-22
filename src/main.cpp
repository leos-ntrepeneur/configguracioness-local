// main.cpp
//
// Punto de entrada del programa. Crea las "piezas" del sistema
// (Inventario, GestorVentas, repositorios de persistencia), carga los
// datos guardados de ejecuciones anteriores y le pasa el control al Menu.
// Nada de logica de negocio ni de presentacion vive aqui.

#include <memory>

#include "GestorVentas.h"
#include "Inventario.h"
#include "Menu.h"
#include "RepositorioProductosCsv.h"
#include "RepositorioVentasCsv.h"

int main() {
    Inventario inventario;
    GestorVentas gestorVentas(inventario);

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

    // Carga inicial: si es la primera vez que se ejecuta el programa y los
    // archivos todavia no existen, cargarTodos()/cargarTodas() devuelven
    // vectores vacios (no lanzan excepcion), asi que esto tambien funciona
    // "en frio".
    inventario.cargarProductos(repositorioProductos->cargarTodos());
    gestorVentas.cargarVentas(repositorioVentas->cargarTodas());

    // `*repositorioProductos` desreferencia el unique_ptr para obtener una
    // IRepositorioProductos& (Menu no necesita ser dueno del repositorio,
    // solo usarlo, igual que con inventario/gestorVentas).
    Menu menu(inventario, gestorVentas, *repositorioProductos, *repositorioVentas);
    menu.ejecutar();
    return 0;
}
