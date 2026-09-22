#include "CsvUtil.h"

#include <sstream>

namespace csv {

std::vector<std::string> dividirLinea(const std::string& linea, char separador) {
    std::vector<std::string> campos;
    // std::istringstream trata al std::string como si fuera un archivo de
    // entrada: permite reutilizar std::getline (que normalmente lee de
    // std::cin o de un ifstream) para partir por un separador que no sea
    // salto de linea.
    std::istringstream flujo(linea);
    std::string campo;
    while (std::getline(flujo, campo, separador)) {
        campos.push_back(campo);
    }
    // Caso especial: si la linea termina justo en el separador (ej.
    // "a,b,"), getline no produce un campo vacio final por si solo.
    if (!linea.empty() && linea.back() == separador) {
        campos.push_back("");
    }
    return campos;
}

} // namespace csv
