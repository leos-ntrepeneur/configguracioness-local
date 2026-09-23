#include "InformacionNegocio.h"
#include "ValidacionTexto.h"

InformacionNegocio::InformacionNegocio(std::string nombre, std::string direccion,
                                        std::string telefono, std::string rfc)
    : nombre_(std::move(nombre)),
      direccion_(std::move(direccion)),
      telefono_(std::move(telefono)),
      rfc_(std::move(rfc)) {
    validacion::validarTextoSeguro(nombre_, "El nombre del negocio", LONGITUD_MAXIMA);
    validacion::validarTextoSeguro(direccion_, "La direccion", LONGITUD_MAXIMA);
    validacion::validarTextoSeguro(telefono_, "El telefono", LONGITUD_MAXIMA);
    validacion::validarTextoSeguro(rfc_, "El RFC", LONGITUD_MAXIMA);
}

const std::string& InformacionNegocio::getNombre() const { return nombre_; }
const std::string& InformacionNegocio::getDireccion() const { return direccion_; }
const std::string& InformacionNegocio::getTelefono() const { return telefono_; }
const std::string& InformacionNegocio::getRfc() const { return rfc_; }

void InformacionNegocio::setNombre(std::string nombre) {
    validacion::validarTextoSeguro(nombre, "El nombre del negocio", LONGITUD_MAXIMA);
    nombre_ = std::move(nombre);
}

void InformacionNegocio::setDireccion(std::string direccion) {
    validacion::validarTextoSeguro(direccion, "La direccion", LONGITUD_MAXIMA);
    direccion_ = std::move(direccion);
}

void InformacionNegocio::setTelefono(std::string telefono) {
    validacion::validarTextoSeguro(telefono, "El telefono", LONGITUD_MAXIMA);
    telefono_ = std::move(telefono);
}

void InformacionNegocio::setRfc(std::string rfc) {
    validacion::validarTextoSeguro(rfc, "El RFC", LONGITUD_MAXIMA);
    rfc_ = std::move(rfc);
}
