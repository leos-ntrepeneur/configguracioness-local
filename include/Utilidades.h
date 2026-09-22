// Utilidades.h
//
// Funciones libres (no pertenecen a ninguna clase) para lectura segura de
// consola. En C++, std::cin >> variable NO lanza excepcion si el usuario
// escribe texto donde se espera un numero: simplemente pone el stream en
// "estado de error" (fail bit) y deja la variable sin tocar. Si no se revisa
// ese estado a mano, el programa entra en un ciclo infinito leyendo basura.
// Estas funciones encapsulan esa revision para no repetirla en cada menu.

#ifndef UTILIDADES_H
#define UTILIDADES_H

#include <string>

namespace utilidades {

// Pide un entero por consola hasta que el usuario escriba algo valido.
int leerEntero(const std::string& mensaje);

// Igual, pero para numeros con decimales.
double leerDouble(const std::string& mensaje);

// Lee una linea completa de texto (permite espacios, a diferencia de cin >>).
std::string leerLinea(const std::string& mensaje);

// Muestra el mensaje y espera una confirmacion s/n. Devuelve true si "s".
bool confirmar(const std::string& mensaje);

} // namespace utilidades

#endif // UTILIDADES_H
