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

// Resultado completo del reporte de ventas del dia (Requisito 5).
struct ReporteVentasDia {
    int numeroTransacciones = 0;
    double totalVendido = 0.0;
    // Ordenado de mayor a menor cantidad vendida.
    std::vector<ResumenProducto> productosMasVendidos;
};

class GestorVentas {
public:
    explicit GestorVentas(Inventario& inventario);

    // Valida stock disponible para cada detalle, descuenta el inventario y
    // agrega la venta al historial. Devuelve una referencia const a la
    // Venta recien creada (vive dentro de ventas_, por eso la referencia es
    // valida mientras GestorVentas exista y no se borre esa venta).
    const Venta& registrarVenta(const std::vector<DetalleVenta>& detalles);

    const std::vector<Venta>& listarVentas() const;
    std::size_t cantidadVentas() const;

    // Recorre el historial, se queda solo con las ventas de HOY (Venta::
    // esDelDiaActual) y calcula total vendido, numero de transacciones y el
    // ranking de productos mas vendidos.
    ReporteVentasDia generarReporteDelDia() const;

    // Reemplaza el historial completo con lo leido de persistencia al
    // iniciar el programa. A diferencia de registrarVenta(), NO valida ni
    // descuenta stock: el stock que se cargo en Inventario ya es el
    // resultado neto de estas ventas pasadas, asi que volver a descontarlo
    // aqui las contaria dos veces.
    void cargarVentas(std::vector<Venta> ventas);

private:
    Inventario& inventario_;
    std::vector<Venta> ventas_;
};

#endif // GESTOR_VENTAS_H
