// ValidacionTexto.h
//
// Regla de "texto seguro para CSV" compartida por cualquier clase que
// guarde texto libre en un archivo CSV (Producto, InformacionNegocio...).
// Vive en su propio archivo porque ya es la SEGUNDA clase que la necesita
// -- la señal clasica de que algo dejo de ser un detalle interno de una
// sola clase y paso a ser una regla del "formato de almacenamiento" en
// general.

#ifndef VALIDACION_TEXTO_H
#define VALIDACION_TEXTO_H

#include <cstddef>
#include <string>

namespace validacion {

// Lanza EntradaInvalida si `valor` tiene una coma (rompe el separador de
// columnas), un caracter de control como salto de linea o tabulador (rompe
// la separacion de filas, o podria inyectar secuencias raras en la
// consola), o si supera `longitudMaxima` caracteres.
void validarTextoSeguro(const std::string& valor, const std::string& nombreCampo, std::size_t longitudMaxima);

} // namespace validacion

#endif // VALIDACION_TEXTO_H
