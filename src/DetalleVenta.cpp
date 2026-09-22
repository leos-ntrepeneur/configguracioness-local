#include "DetalleVenta.h"
#include "Excepciones.h"

DetalleVenta::DetalleVenta(std::string codigoProducto,
                            std::string nombreProducto,
                            int cantidad,
                            double precioUnitario)
    : codigoProducto_(std::move(codigoProducto)),
      nombreProducto_(std::move(nombreProducto)),
      cantidad_(cantidad),
      precioUnitario_(precioUnitario) {
    if (cantidad_ <= 0) {
        throw EntradaInvalida("La cantidad de un detalle de venta debe ser mayor a cero.");
    }
    if (precioUnitario_ < 0.0) {
        throw EntradaInvalida("El precio unitario no puede ser negativo.");
    }
}

const std::string& DetalleVenta::getCodigoProducto() const { return codigoProducto_; }
const std::string& DetalleVenta::getNombreProducto() const { return nombreProducto_; }
int DetalleVenta::getCantidad() const { return cantidad_; }
double DetalleVenta::getPrecioUnitario() const { return precioUnitario_; }

double DetalleVenta::getSubtotal() const {
    return cantidad_ * precioUnitario_;
}
