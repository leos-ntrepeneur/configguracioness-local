// GestorVentas.h
//
// Convierte un conjunto de DetalleVenta (lo que el usuario quiere comprar)
// en una Venta valida: verifica que haya stock suficiente de TODO antes de
// tocar el inventario (para no dejarlo a medias si un producto falla) y
// luego descuenta el stock real a traves de Inventario. Tambien guarda el
// historial de ventas para el reporte del dia (Requisito 5).

#ifndef GESTOR_VENTAS_H
#define GESTOR_VENTAS_H

#include <vector>

#include "DetalleVenta.h"
#include "Inventario.h"
#include "Venta.h"

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

private:
    Inventario& inventario_;
    std::vector<Venta> ventas_;
};

#endif // GESTOR_VENTAS_H
