#include "GestorVentas.h"
#include "Excepciones.h"

#include <map>

GestorVentas::GestorVentas(Inventario& inventario) : inventario_(inventario) {}

const Venta& GestorVentas::registrarVenta(const std::vector<DetalleVenta>& detalles) {
    if (detalles.empty()) {
        throw EntradaInvalida("La venta no tiene productos.");
    }

    // Primera pasada: sumar cantidades solicitadas por codigo (por si el
    // mismo producto aparece en mas de un detalle) y validar TODO el pedido
    // contra el inventario antes de modificar nada. Si valida uno por uno
    // mientras descuenta, un producto sin stock a mitad de la venta dejaria
    // el inventario descontado a medias.
    std::map<std::string, int> cantidadPorCodigo;
    for (const DetalleVenta& detalle : detalles) {
        cantidadPorCodigo[detalle.getCodigoProducto()] += detalle.getCantidad();
    }
    for (const auto& [codigo, cantidadSolicitada] : cantidadPorCodigo) {
        const Producto& producto = inventario_.buscarPorCodigo(codigo); // lanza ProductoNoEncontrado
        if (cantidadSolicitada > producto.getStock()) {
            throw StockInsuficiente(codigo, producto.getStock(), cantidadSolicitada);
        }
    }

    // Segunda pasada: ya confirmamos que todo alcanza, ahora si descontamos.
    for (const auto& [codigo, cantidad] : cantidadPorCodigo) {
        inventario_.descontarStock(codigo, cantidad);
    }

    // emplace_back construye la Venta directamente dentro del vector (con
    // los argumentos dados), en vez de crearla aparte y copiarla/moverla
    // adentro como haria push_back. Es una optimizacion tipica de C++.
    ventas_.emplace_back(detalles);
    return ventas_.back();
}

const std::vector<Venta>& GestorVentas::listarVentas() const {
    return ventas_;
}

std::size_t GestorVentas::cantidadVentas() const {
    return ventas_.size();
}
