// main.cpp
//
// Punto de entrada del programa. Deliberadamente muy corto: crea las
// "piezas" del sistema (Inventario, GestorVentas) y le pasa el control al
// Menu. Nada de logica de negocio ni de presentacion vive aqui.
//
// Notese el orden: GestorVentas se construye con una referencia a
// inventario, asi que inventario debe existir (y seguir viva) antes y
// durante toda la vida de gestorVentas. En C++ el orden de destruccion de
// variables locales es el inverso al de construccion, asi que esto tambien
// garantiza que gestorVentas se destruye antes que inventario.

#include "GestorVentas.h"
#include "Inventario.h"
#include "Menu.h"

int main() {
    Inventario inventario;
    GestorVentas gestorVentas(inventario);
    Menu menu(inventario, gestorVentas);
    menu.ejecutar();
    return 0;
}
