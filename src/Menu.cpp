#include "Menu.h"
#include "Excepciones.h"
#include "Utilidades.h"

#include <iostream>
#include <iomanip>

using utilidades::confirmar;
using utilidades::leerDouble;
using utilidades::leerEntero;
using utilidades::leerLinea;

// Lista de inicializacion de miembros (": inventario_(inventario)"): es la
// forma idiomatica de inicializar referencias y const en C++. No se puede
// asignar una referencia dentro del cuerpo del constructor (a diferencia de
// un campo normal), tiene que "amarrarse" a su objetivo en este punto.
Menu::Menu(Inventario& inventario) : inventario_(inventario) {}

void Menu::ejecutar() {
    bool salir = false;
    while (!salir) {
        mostrarMenuPrincipal();
        try {
            int opcion = leerEntero("Selecciona una opcion: ");
            switch (opcion) {
                case 1:
                    gestionarProductos();
                    break;
                case 0:
                    salir = true;
                    std::cout << "Hasta luego.\n";
                    break;
                default:
                    std::cout << "Opcion no valida.\n";
            }
        } catch (const FinDeEntrada&) {
            // Ver Excepciones.h: este catch debe ir ANTES que uno generico
            // de std::exception para poder distinguir "se acabo la entrada"
            // de un error de negocio comun.
            std::cout << "\nEntrada finalizada. Cerrando el programa.\n";
            salir = true;
        }
    }
}

void Menu::mostrarMenuPrincipal() const {
    std::cout << "\n=== Sistema de Inventario y Punto de Venta ===\n";
    std::cout << "1. Gestion de productos\n";
    std::cout << "0. Salir\n";
}

void Menu::gestionarProductos() {
    bool volver = false;
    while (!volver) {
        std::cout << "\n--- Gestion de productos ---\n";
        std::cout << "1. Dar de alta un producto\n";
        std::cout << "2. Editar un producto\n";
        std::cout << "3. Eliminar (baja) un producto\n";
        std::cout << "4. Listar productos\n";
        std::cout << "0. Volver al menu principal\n";
        // Cada operacion se envuelve en try/catch: si Producto/Inventario
        // lanzan una excepcion de negocio, la atrapamos aqui, mostramos el
        // mensaje y el programa sigue vivo (Requisito tecnico: manejar
        // errores sin que la app se caiga).
        try {
            int opcion = leerEntero("Selecciona una opcion: ");
            switch (opcion) {
                case 1: alDarAltaProducto(); break;
                case 2: alEditarProducto(); break;
                case 3: alEliminarProducto(); break;
                case 4: alListarProductos(); break;
                case 0: volver = true; break;
                default: std::cout << "Opcion no valida.\n";
            }
        } catch (const FinDeEntrada&) {
            // Se re-lanza para que Menu::ejecutar() tambien detenga su
            // propio bucle en vez de quedar esperando otra opcion.
            throw;
        } catch (const std::exception& e) {
            // std::exception es la clase base de la que heredan
            // std::runtime_error y, por lo tanto, nuestras excepciones de
            // Excepciones.h. Atraparla aqui cubre cualquier error de negocio
            // que definamos a futuro sin tener que listar cada tipo.
            std::cout << "Error: " << e.what() << "\n";
        }
    }
}

void Menu::alDarAltaProducto() {
    std::cout << "\n-- Alta de producto --\n";
    std::string codigo = leerLinea("Codigo (unico): ");
    std::string nombre = leerLinea("Nombre: ");
    double precio = leerDouble("Precio: ");
    int stock = leerEntero("Stock inicial: ");
    std::string categoria = leerLinea("Categoria (opcional, Enter para omitir): ");
    int stockMinimo = leerEntero("Stock minimo para alertas (0 si no aplica): ");

    Producto nuevo(codigo, nombre, precio, stock, categoria, stockMinimo);
    inventario_.agregarProducto(nuevo);
    std::cout << "Producto agregado correctamente.\n";
}

void Menu::alEditarProducto() {
    std::cout << "\n-- Editar producto --\n";
    std::string codigo = leerLinea("Codigo del producto a editar: ");

    // Consultamos primero para mostrar los valores actuales como referencia;
    // si no existe, buscarPorCodigo lanza ProductoNoEncontrado y el catch de
    // gestionarProductos() se encarga de mostrarlo.
    const Producto& actual = inventario_.buscarPorCodigo(codigo);
    std::cout << "Datos actuales -> nombre: " << actual.getNombre()
              << ", precio: " << actual.getPrecio()
              << ", categoria: " << actual.getCategoria()
              << ", stock minimo: " << actual.getStockMinimo() << "\n";

    std::string nombre = leerLinea("Nuevo nombre: ");
    double precio = leerDouble("Nuevo precio: ");
    std::string categoria = leerLinea("Nueva categoria (Enter para dejar vacia): ");
    int stockMinimo = leerEntero("Nuevo stock minimo: ");

    inventario_.editarProducto(codigo, nombre, precio, categoria, stockMinimo);
    std::cout << "Producto actualizado correctamente.\n";
}

void Menu::alEliminarProducto() {
    std::cout << "\n-- Baja de producto --\n";
    std::string codigo = leerLinea("Codigo del producto a eliminar: ");
    const Producto& actual = inventario_.buscarPorCodigo(codigo);
    std::cout << "Vas a eliminar: " << actual.getNombre() << " (codigo " << codigo << ")\n";
    if (confirmar("Confirmas la eliminacion?")) {
        inventario_.eliminarProducto(codigo);
        std::cout << "Producto eliminado.\n";
    } else {
        std::cout << "Operacion cancelada.\n";
    }
}

void Menu::alListarProductos() const {
    std::vector<Producto> productos = inventario_.listarTodos();
    if (productos.empty()) {
        std::cout << "No hay productos registrados.\n";
        return;
    }

    std::cout << "\n"
              << std::left << std::setw(10) << "Codigo"
              << std::setw(25) << "Nombre"
              << std::right << std::setw(10) << "Precio"
              << std::setw(8) << "Stock"
              << "  " << std::left << "Categoria" << "\n";
    std::cout << std::string(65, '-') << "\n";

    for (const Producto& p : productos) {
        std::cout << std::left << std::setw(10) << p.getCodigo()
                   << std::setw(25) << p.getNombre()
                   << std::right << std::setw(10) << std::fixed << std::setprecision(2) << p.getPrecio()
                   << std::setw(8) << p.getStock()
                   << "  " << std::left << p.getCategoria();
        if (p.estaBajoStockMinimo()) {
            std::cout << "  [STOCK BAJO]";
        }
        std::cout << "\n";
    }
}
