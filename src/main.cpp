// main.cpp
//
// Punto de entrada del programa. Deliberadamente muy corto: crea las
// "piezas" del sistema (por ahora solo Inventario) y le pasa el control al
// Menu. Nada de logica de negocio ni de presentacion vive aqui.

#include "Inventario.h"
#include "Menu.h"

int main() {
    Inventario inventario;
    Menu menu(inventario);
    menu.ejecutar();
    return 0;
}
