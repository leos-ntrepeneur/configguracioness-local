// DetalleVenta.h
//
// Una linea dentro de una Venta: que producto, cuantas unidades, y a que
// precio se vendio CADA UNIDAD en ese momento. Guardamos el precio "en el
// momento de la venta" (no una referencia al Producto) a proposito: si
// mañana cambia el precio del producto, las ventas ya registradas no deben
// cambiar de total. Es el mismo motivo por el que un ticket de compra real
// nunca cambia de importe aunque el negocio suba precios despues.

#ifndef DETALLE_VENTA_H
#define DETALLE_VENTA_H

#include <string>

class DetalleVenta {
public:
    DetalleVenta(std::string codigoProducto,
                 std::string nombreProducto,
                 int cantidad,
                 double precioUnitario);

    const std::string& getCodigoProducto() const;
    const std::string& getNombreProducto() const;
    int getCantidad() const;
    double getPrecioUnitario() const;

    // No se guarda como campo: se calcula cada vez que se pide, para que
    // nunca pueda quedar "desincronizado" de cantidad/precioUnitario.
    double getSubtotal() const;

private:
    std::string codigoProducto_;
    std::string nombreProducto_;
    int cantidad_;
    double precioUnitario_;
};

#endif // DETALLE_VENTA_H
