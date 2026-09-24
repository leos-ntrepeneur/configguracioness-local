#include "Cliente.h"
#include "Excepciones.h"
#include "ValidacionTexto.h"

Cliente::Cliente(int id, std::string nombre, std::string telefono)
    : id_(id), nombre_(std::move(nombre)), telefono_(std::move(telefono)) {
    if (id_ <= 0) {
        throw EntradaInvalida("El id del cliente debe ser mayor a cero.");
    }
    if (nombre_.empty()) {
        throw EntradaInvalida("El nombre del cliente no puede estar vacio.");
    }
    validacion::validarTextoSeguro(nombre_, "El nombre del cliente", NOMBRE_LONGITUD_MAXIMA);
    validacion::validarTextoSeguro(telefono_, "El telefono del cliente", TELEFONO_LONGITUD_MAXIMA);
}

int Cliente::getId() const { return id_; }
const std::string& Cliente::getNombre() const { return nombre_; }
const std::string& Cliente::getTelefono() const { return telefono_; }
