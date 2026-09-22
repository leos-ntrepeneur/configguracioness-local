#include "DetalleVenta.h"
#include "Excepciones.h"
#include "Producto.h"

#include <cmath>

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
    // Reutiliza el mismo tope que Producto::STOCK_MAXIMO: ningun producto
    // puede tener mas stock que eso, asi que ninguna venta valida deberia
    // pedir mas unidades que eso tampoco. Ademas de tener sentido de
    // negocio, esto es lo que le pone un techo a las sumas de cantidades
    // en GestorVentas::registrarVenta (ver ese archivo) para que no puedan
    // desbordar un int aunque alguien construyera el vector de detalles a
    // mano en vez de armarlo con el carrito de Menu/PestanaVentas.
    if (cantidad_ > Producto::STOCK_MAXIMO) {
        throw EntradaInvalida("La cantidad de un detalle de venta no puede superar " +
                               std::to_string(Producto::STOCK_MAXIMO) + " unidades.");
    }
    // Igual que en Producto::validarPrecio: std::cin >> double acepta
    // "nan"/"inf" como numeros validos, y una comparacion `< 0.0` sola no
    // los detecta (cualquier comparacion con NaN da falso).
    if (!std::isfinite(precioUnitario_)) {
        throw EntradaInvalida("El precio unitario debe ser un numero valido (no infinito ni NaN).");
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
