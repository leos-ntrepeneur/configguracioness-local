// GestorVentas.h
//
// Convierte un conjunto de DetalleVenta (lo que el usuario quiere comprar)
// en una Venta valida: verifica que haya stock suficiente de TODO antes de
// tocar el inventario (para no dejarlo a medias si un producto falla) y
// luego descuenta el stock real a traves de Inventario. Tambien guarda el
// historial de ventas para el reporte del dia (Requisito 5).

#ifndef GESTOR_VENTAS_H
#define GESTOR_VENTAS_H

#include <string>
#include <vector>

#include "DetalleVenta.h"
#include "Inventario.h"
#include "Venta.h"

// Fila del reporte del dia: cuanto se vendio de UN producto, sumando todas
// las ventas de hoy. Los `= 0` / `= 0.0` son inicializadores de miembro
// (C++11 en adelante): si se crea un ResumenProducto sin darle valores
// explicitos, arrancan en cero en vez de quedar con basura de memoria como
// pasaria con un struct de C sin inicializar.
struct ResumenProducto {
    std::string codigo;
    std::string nombre;
    int cantidadVendida = 0;
    double totalVendido = 0.0;
};

// Fila del historial de transacciones del dia: UNA venta, no agregada con
// otras. Es justo lo que le faltaba al reporte original -- antes solo se
// veia "se vendieron 5 martillos hoy" (sumado); esto muestra "folio #4, a
// las 9:03am, pagado con tarjeta de credito, 3 martillos, $449.70" y
// "folio #7, a las 2:15pm, efectivo, 2 martillos, $299.80" por separado.
struct TransaccionDia {
    int numeroTransaccion = 0;
    std::string fechaHoraTexto;
    MetodoPago metodoPago = MetodoPago::Efectivo;
    int cantidadProductos = 0; // unidades totales en esa venta (todas las lineas).
    double total = 0.0;
};

// Fila de la grafica de ventas por categoria: cuanto se vendio de UNA
// categoria, sumando todos los productos que pertenecen a ella. Un
// producto sin categoria capturada (Producto::getCategoria() vacio), o
// que ya se elimino del inventario despues de venderse, cae en el mismo
// cubo "Sin categoria" -- no vale la pena distinguir ambos casos aqui.
struct ResumenCategoria {
    std::string categoria;
    int cantidadVendida = 0;
    double totalVendido = 0.0;
};

// Resultado completo del reporte de ventas del dia (Requisito 5).
struct ReporteVentasDia {
    int numeroTransacciones = 0;
    double totalVendido = 0.0;
    // Ordenado de mayor a menor cantidad vendida.
    std::vector<ResumenProducto> productosMasVendidos;
    // Ordenado cronologicamente, una fila por venta (ver TransaccionDia).
    std::vector<TransaccionDia> transacciones;
    // Ordenado de mayor a menor total vendido -- es lo que alimenta la
    // grafica de barras de ventas por categoria (consola: barras ASCII;
    // GUI: GraficaBarras).
    std::vector<ResumenCategoria> ventasPorCategoria;
};

class GestorVentas {
public:
    explicit GestorVentas(Inventario& inventario);

    // Valida stock disponible para cada detalle, descuenta el inventario y
    // agrega la venta al historial con un folio nuevo (secuencial, ver
    // siguienteNumeroTransaccion_). Devuelve una referencia const a la
    // Venta recien creada (vive dentro de ventas_, por eso la referencia es
    // valida mientras GestorVentas exista y no se borre esa venta).
    //
    // `clienteId`/`nombreCliente` solo aplican cuando metodoPago es
    // MetodoPago::Fiado (0 / cadena vacia en cualquier otro caso) -- ver
    // el comentario grande en Venta.h. Quien llame ya debio resolver el
    // cliente (crearlo o buscarlo en GestorClientes) antes de llegar aqui:
    // GestorVentas no conoce GestorClientes a proposito, para no acoplar
    // "registrar una venta" con "administrar clientes".
    const Venta& registrarVenta(const std::vector<DetalleVenta>& detalles, MetodoPago metodoPago,
                                 int clienteId = 0, std::string nombreCliente = "");

    const std::vector<Venta>& listarVentas() const;
    std::size_t cantidadVentas() const;

    // Busca una venta ya registrada por su folio (para reimprimir su
    // ticket desde el historial, ver PestanaReporte/Menu). Devuelve
    // `nullptr` si no existe -- no lanza excepcion porque "el folio que
    // tecleaste no existe" es una situacion normal de UI, no un error de
    // programacion.
    const Venta* buscarPorNumeroTransaccion(int numeroTransaccion) const;

    // Recorre el historial, se queda solo con las ventas de HOY (Venta::
    // esDelDiaActual) y calcula total vendido, numero de transacciones, el
    // ranking de productos mas vendidos, y el historial de transacciones
    // por separado.
    ReporteVentasDia generarReporteDelDia() const;

    // Reemplaza el historial completo con lo leido de persistencia al
    // iniciar el programa. A diferencia de registrarVenta(), NO valida ni
    // descuenta stock: el stock que se cargo en Inventario ya es el
    // resultado neto de estas ventas pasadas, asi que volver a descontarlo
    // aqui las contaria dos veces. Tambien recalcula el siguiente folio
    // disponible a partir del mayor numero de transaccion cargado, para
    // que las ventas nuevas de hoy no repitan un folio ya usado ayer.
    void cargarVentas(std::vector<Venta> ventas);

private:
    Inventario& inventario_;
    std::vector<Venta> ventas_;
    int siguienteNumeroTransaccion_ = 1;
};

#endif // GESTOR_VENTAS_H
