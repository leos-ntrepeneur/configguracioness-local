// Menu.h
//
// Capa de presentacion: solo imprime texto, lee opciones y llama a los
// metodos de Inventario (y mas adelante GestorVentas). No valida reglas de
// negocio aqui -- esas viven en Producto/Inventario y se manejan con
// try/catch para que un error no tumbe el programa.

#ifndef MENU_H
#define MENU_H

#include "Inventario.h"

class Menu {
public:
    // Recibe una REFERENCIA a Inventario, no una copia: Menu no es dueno del
    // inventario, solo lo usa. `&` es el operador de referencia de C++: un
    // alias sin memoria propia hacia el objeto original (distinto de un
    // puntero, que si ocupa su propia memoria y puede ser nulo).
    explicit Menu(Inventario& inventario);

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

    Inventario& inventario_;
};

#endif // MENU_H
