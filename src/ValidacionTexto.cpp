#include "ValidacionTexto.h"
#include "Excepciones.h"

namespace validacion {

void validarTextoSeguro(const std::string& valor, const std::string& nombreCampo, std::size_t longitudMaxima) {
    if (valor.find(',') != std::string::npos) {
        throw EntradaInvalida(nombreCampo + " no puede contener comas.");
    }
    for (unsigned char c : valor) {
        if (c < 0x20) {
            throw EntradaInvalida(nombreCampo + " no puede contener saltos de linea ni caracteres de control.");
        }
    }
    if (valor.size() > longitudMaxima) {
        throw EntradaInvalida(nombreCampo + " no puede tener mas de " +
                               std::to_string(longitudMaxima) + " caracteres.");
    }
}

} // namespace validacion
