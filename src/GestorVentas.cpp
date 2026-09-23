#include "GestorVentas.h"
#include "Excepciones.h"

#include <algorithm>
#include <map>

GestorVentas::GestorVentas(Inventario& inventario) : inventario_(inventario) {}

const Venta& GestorVentas::registrarVenta(const std::vector<DetalleVenta>& detalles, MetodoPago metodoPago) {
    if (detalles.empty()) {
        throw EntradaInvalida("La venta no tiene productos.");
    }

    // Primera pasada: sumar cantidades solicitadas por codigo (por si el
    // mismo producto aparece en mas de un detalle) y validar TODO el pedido
    // contra el inventario antes de modificar nada. Si valida uno por uno
    // mientras descuenta, un producto sin stock a mitad de la venta dejaria
    // el inventario descontado a medias.
    //
    // Se acumula en `long long` (64 bits) en vez de `int` (32 bits) a
    // proposito: cada DetalleVenta individual ya viene acotado a
    // Producto::STOCK_MAXIMO (ver DetalleVenta.cpp), pero si el MISMO
    // codigo aparece en muchos detalles dentro de una sola venta, la SUMA
    // de varios de esos maximos si podria desbordar un int (un int se
    // desborda alrededor de 2,147 millones; bastarian ~2,148 detalles de
    // 1,000,000 cada uno). Un long long en una maquina moderna llega a
    // mas de 9 trillones, asi que la suma jamas se acerca a su limite;
    // el resultado se valida contra STOCK_MAXIMO (con margen de sobra)
    // antes de volver a un int mas abajo.
    std::map<std::string, long long> cantidadPorCodigo;
    for (const DetalleVenta& detalle : detalles) {
        long long& acumulado = cantidadPorCodigo[detalle.getCodigoProducto()];
        acumulado += detalle.getCantidad();
        if (acumulado > Producto::STOCK_MAXIMO) {
            throw EntradaInvalida("La cantidad solicitada de '" + detalle.getCodigoProducto() +
                                   "' supera el maximo permitido.");
        }
    }
    for (const auto& [codigo, cantidadSolicitada] : cantidadPorCodigo) {
        const Producto& producto = inventario_.buscarPorCodigo(codigo); // lanza ProductoNoEncontrado
        if (cantidadSolicitada > producto.getStock()) {
            // static_cast<int> es seguro aqui: el bucle de arriba ya
            // garantizo que cantidadSolicitada <= Producto::STOCK_MAXIMO,
            // que cabe de sobra en un int.
            throw StockInsuficiente(codigo, producto.getStock(), static_cast<int>(cantidadSolicitada));
        }
    }

    // Segunda pasada: ya confirmamos que todo alcanza, ahora si descontamos.
    for (const auto& [codigo, cantidad] : cantidadPorCodigo) {
        inventario_.descontarStock(codigo, static_cast<int>(cantidad));
    }

    // emplace_back construye la Venta directamente dentro del vector (con
    // los argumentos dados), en vez de crearla aparte y copiarla/moverla
    // adentro como haria push_back. Es una optimizacion tipica de C++. El
    // folio se toma y se avanza SOLO aqui, una vez que ya sabemos que la
    // venta va a completarse (todas las validaciones de arriba pasaron) --
    // asi un intento fallido (ej. StockInsuficiente) no "quema" un numero
    // de folio que nunca llego a usarse.
    int folio = siguienteNumeroTransaccion_++;
    ventas_.emplace_back(detalles, folio, metodoPago);
    return ventas_.back();
}

const std::vector<Venta>& GestorVentas::listarVentas() const {
    return ventas_;
}

std::size_t GestorVentas::cantidadVentas() const {
    return ventas_.size();
}

const Venta* GestorVentas::buscarPorNumeroTransaccion(int numeroTransaccion) const {
    for (const Venta& venta : ventas_) {
        if (venta.getNumeroTransaccion() == numeroTransaccion) {
            return &venta;
        }
    }
    return nullptr;
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

        // Fila del historial: ESTA venta, sola, sin mezclarse con las
        // demas -- es la parte que antes faltaba (el reporte solo sumaba
        // por producto, nunca mostraba las transacciones por separado).
        TransaccionDia fila;
        fila.numeroTransaccion = venta.getNumeroTransaccion();
        fila.fechaHoraTexto = venta.fechaComoTexto();
        fila.metodoPago = venta.getMetodoPago();
        fila.total = venta.getTotal();
        for (const DetalleVenta& detalle : venta.getDetalles()) {
            fila.cantidadProductos += detalle.getCantidad();
        }
        reporte.transacciones.push_back(fila);

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

void GestorVentas::cargarVentas(std::vector<Venta> ventas) {
    ventas_ = std::move(ventas);

    // El siguiente folio debe quedar por encima de CUALQUIER folio ya
    // usado en el historial cargado -- si no, la primera venta nueva del
    // dia reutilizaria un numero de una venta de ayer.
    siguienteNumeroTransaccion_ = 1;
    for (const Venta& venta : ventas_) {
        if (venta.getNumeroTransaccion() >= siguienteNumeroTransaccion_) {
            siguienteNumeroTransaccion_ = venta.getNumeroTransaccion() + 1;
        }
    }
}
