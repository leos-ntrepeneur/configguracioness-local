// Menu.h
//
// Capa de presentacion: solo imprime texto, lee opciones y llama a los
// metodos de Inventario/GestorVentas. No valida reglas de negocio aqui --
// esas viven en Producto/Inventario/GestorVentas y se manejan con
// try/catch para que un error no tumbe el programa.

#ifndef MENU_H
#define MENU_H

#include <map>
#include <string>
#include <vector>

#include "GestorVentas.h"
#include "IRepositorioInformacionNegocio.h"
#include "IRepositorioProductos.h"
#include "IRepositorioVentas.h"
#include "InformacionNegocio.h"
#include "Inventario.h"
#include "MetodoPago.h"

class Menu {
public:
    // Recibe REFERENCIAS a Inventario/GestorVentas/repositorios, no copias
    // ni punteros: Menu no es dueno de ninguno, solo los usa. `&` es el
    // operador de referencia de C++: un alias sin memoria propia hacia el
    // objeto original (distinto de un puntero, que si ocupa su propia
    // memoria y puede ser nulo).
    //
    // Notese que los parametros son IRepositorioProductos&/IRepositorioVentas&
    // (la INTERFAZ), no RepositorioProductosCsv&/RepositorioVentasCsv&
    // (la implementacion concreta). Esto es "programar contra una
    // interfaz": Menu puede guardar/cargar datos sin saber ni importarle
    // si por debajo es un CSV o, mas adelante, una base SQLite.
    Menu(Inventario& inventario,
         GestorVentas& gestorVentas,
         IRepositorioProductos& repositorioProductos,
         IRepositorioVentas& repositorioVentas,
         IRepositorioInformacionNegocio& repositorioInformacionNegocio);

    // Punto de entrada: corre el bucle principal hasta que el usuario
    // elige salir.
    void ejecutar();

private:
    void mostrarMenuPrincipal() const;
    void gestionarProductos();

    void alDarAltaProducto();
    void alEditarProducto();
    void alEliminarProducto();
    void alListarProductos() const;
    void alBuscarProducto() const;
    void alVerAlertasStockBajo() const;

    void alVerReporteVentasDelDia() const;
    // Imprime el historial de transacciones del dia (folio, hora, metodo
    // de pago, total), como parte del reporte -- es lo que antes faltaba
    // para poder distinguir una venta de otra en vez de solo ver el total
    // sumado por producto.
    void mostrarHistorialTransacciones(const std::vector<TransaccionDia>& transacciones) const;

    void alConfigurarInformacionNegocio();

    // Compartida por alListarProductos() y alBuscarProducto() para no
    // duplicar el formato de tabla en dos lugares.
    void mostrarTablaProductos(const std::vector<Producto>& productos) const;

    // --- Registro de ventas ---
    void registrarVenta();
    // El "carrito" vive solo mientras se arma la venta: codigo -> cantidad
    // acumulada. Se pasa por referencia para que las funciones auxiliares
    // lo modifiquen sin necesidad de devolverlo y reasignarlo.
    void mostrarCarrito(const std::map<std::string, int>& carrito) const;
    void agregarAlCarrito(std::map<std::string, int>& carrito);
    void quitarDelCarrito(std::map<std::string, int>& carrito);
    void confirmarVenta(const std::map<std::string, int>& carrito);
    // Pide el metodo de pago por teclado (1/2/3); reintenta hasta que la
    // opcion sea valida en vez de lanzar EntradaInvalida por una eleccion
    // de menu -- no es un dato de negocio invalido, solo hay que repetir
    // la pregunta.
    MetodoPago preguntarMetodoPago() const;

    // --- Persistencia (Requisito 6) ---
    // Se llama despues de cada alta/edicion/baja/venta exitosa, para que
    // una salida inesperada (cerrar la ventana, un corte de luz) pierda
    // como maximo la operacion en curso, no todo el historial previo.
    void guardarDatos() const;

    Inventario& inventario_;
    GestorVentas& gestorVentas_;
    IRepositorioProductos& repositorioProductos_;
    IRepositorioVentas& repositorioVentas_;
    IRepositorioInformacionNegocio& repositorioInformacionNegocio_;
    // A diferencia de Inventario/GestorVentas (colecciones grandes que
    // viven fuera de Menu), la InformacionNegocio es un solo registro
    // chico: Menu la mantiene en memoria directamente y la guarda a
    // traves del repositorio cuando el usuario la edita.
    InformacionNegocio informacionNegocio_;
};

#endif // MENU_H
