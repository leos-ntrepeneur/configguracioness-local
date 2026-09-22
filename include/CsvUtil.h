// CsvUtil.h
//
// Funciones libres para partir una linea de texto CSV en sus campos.
// Simplificacion deliberada: NO soporta campos entre comillas que
// contengan comas (un CSV "de verdad" como el que produce Excel si lo
// soporta). Por eso Producto valida que nombre/categoria/codigo no
// contengan comas (ver Producto.cpp) -- para un negocio pequeno es una
// limitacion razonable; si se necesita mas adelante, este es el punto
// exacto donde se reemplazaria por una libreria de CSV o, mejor aun, por
// migrar a SQLite (ver README).

#ifndef CSV_UTIL_H
#define CSV_UTIL_H

#include <string>
#include <vector>

namespace csv {

std::vector<std::string> dividirLinea(const std::string& linea, char separador = ',');

} // namespace csv

#endif // CSV_UTIL_H
