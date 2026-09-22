#include "Utilidades.h"
#include "Excepciones.h"

#include <iostream>
#include <limits>

namespace utilidades {

int leerEntero(const std::string& mensaje) {
    int valor;
    while (true) {
        std::cout << mensaje;
        std::cin >> valor;
        if (std::cin.fail()) {
            // IMPORTANTE: cin.clear() borra el "fail bit" pero, si la razon
            // del fallo fue que la entrada se acabo (EOF, por ejemplo al
            // redirigir un archivo con `programa < datos.txt`), no hay mas
            // caracteres que leer y la siguiente iteracion volveria a fallar
            // exactamente igual -> ciclo infinito. Por eso revisamos eof()
            // ANTES de limpiar el estado y salimos con una excepcion.
            if (std::cin.eof()) {
                throw FinDeEntrada();
            }
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Entrada invalida. Escribe un numero entero.\n";
            continue;
        }
        // Descarta el resto de la linea (por ejemplo el '\n' pendiente)
        // para que una lectura posterior con leerLinea() no se lo encuentre.
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return valor;
    }
}

double leerDouble(const std::string& mensaje) {
    double valor;
    while (true) {
        std::cout << mensaje;
        std::cin >> valor;
        if (std::cin.fail()) {
            if (std::cin.eof()) {
                throw FinDeEntrada();
            }
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Entrada invalida. Escribe un numero (puede tener decimales).\n";
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return valor;
    }
}

std::string leerLinea(const std::string& mensaje) {
    std::cout << mensaje;
    std::string linea;
    std::getline(std::cin, linea);
    if (std::cin.eof() && linea.empty()) {
        throw FinDeEntrada();
    }
    return linea;
}

bool confirmar(const std::string& mensaje) {
    std::string respuesta = leerLinea(mensaje + " (s/n): ");
    return !respuesta.empty() && (respuesta[0] == 's' || respuesta[0] == 'S');
}

} // namespace utilidades
