#include "GestorVentas.h"
#include "Excepciones.h"

#include <algorithm>
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

ReporteVentasDia GestorVentas::generarReporteDelDia() const {
    ReporteVentasDia reporte;

    // Acumulamos por codigo de producto usando un map (igual que en
    // Inventario): la clave garantiza que cada producto aparezca una sola
    // vez en el resumen aunque se haya vendido en varias transacciones.
    std::map<std::string, ResumenProducto> resumenPorCodigo;

    for (const Venta& venta : ventas_) {
        if (!venta.esDelDiaActual()) {
            continue;
        }
        reporte.numeroTransacciones++;
        reporte.totalVendido += venta.getTotal();

        for (const DetalleVenta& detalle : venta.getDetalles()) {
            // operator[] crea la entrada con el ResumenProducto por
            // defecto (codigo/nombre vacios, cantidad y total en 0) la
            // primera vez que se ve ese codigo, y la reutiliza despues.
            ResumenProducto& resumen = resumenPorCodigo[detalle.getCodigoProducto()];
            resumen.codigo = detalle.getCodigoProducto();
            resumen.nombre = detalle.getNombreProducto();
            resumen.cantidadVendida += detalle.getCantidad();
            resumen.totalVendido += detalle.getSubtotal();
        }
    }

    reporte.productosMasVendidos.reserve(resumenPorCodigo.size());
    for (const auto& [codigo, resumen] : resumenPorCodigo) {
        reporte.productosMasVendidos.push_back(resumen);
    }

    // std::sort con una lambda como criterio de orden: la lambda
    // `[](const ResumenProducto& a, const ResumenProducto& b) { ... }` es
    // una funcion anonima (sin nombre) definida en el momento de usarla,
    // muy comun en C++ moderno para reemplazar comparadores de una linea.
    std::sort(reporte.productosMasVendidos.begin(), reporte.productosMasVendidos.end(),
              [](const ResumenProducto& a, const ResumenProducto& b) {
                  return a.cantidadVendida > b.cantidadVendida;
              });

    return reporte;
}
