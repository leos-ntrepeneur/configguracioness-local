// Venta.h
//
// Representa una transaccion ya cerrada: numero de folio, fecha/hora,
// metodo de pago, las lineas que la componen (DetalleVenta) y el total.
// Una vez creada, una Venta no se edita (no hay setters): en un punto de
// venta real, corregir una venta se hace con una venta de ajuste/nota de
// credito, no reescribiendo el historial. Por eso el constructor calcula
// el total una sola vez y no se expone forma de modificar detalles_
// despues.
//
// std::chrono es la libreria estandar de C++ para fechas/tiempos (equivale
// a DateTime en C#). system_clock::now() da la hora actual; time_point es
// un instante concreto en esa linea de tiempo.

#ifndef VENTA_H
#define VENTA_H

#include <chrono>
#include <string>
#include <vector>

#include "DetalleVenta.h"
#include "MetodoPago.h"

class Venta {
public:
    // Recibe los detalles ya armados (y validados), el numero de folio
    // (lo asigna GestorVentas, secuencial) y el metodo de pago elegido al
    // cobrar. La fecha se toma automaticamente al construir: es el camino
    // normal para una venta que esta ocurriendo AHORA.
    Venta(std::vector<DetalleVenta> detalles, int numeroTransaccion, MetodoPago metodoPago);

    // Igual, pero fijando la fecha manualmente. Solo lo usa
    // RepositorioVentasCsv al reconstruir el historial desde archivo: ahi
    // la venta ya ocurrio en el pasado y hay que respetar su fecha
    // original, no ponerle la hora actual.
    Venta(std::vector<DetalleVenta> detalles, int numeroTransaccion, MetodoPago metodoPago,
          std::chrono::system_clock::time_point fecha);

    // Folio de la transaccion: lo que separa "vendi 3 martillos hoy en
    // total" (una suma sin mas contexto) de "vendi 3 martillos, en DOS
    // ventas distintas, la #4 a las 9am en efectivo y la #7 a las 2pm con
    // tarjeta" -- necesario para generar tickets y para que el reporte del
    // dia muestre las transacciones por separado, no solo agregadas por
    // producto.
    int getNumeroTransaccion() const;
    MetodoPago getMetodoPago() const;

    const std::vector<DetalleVenta>& getDetalles() const;
    double getTotal() const;
    std::chrono::system_clock::time_point getFecha() const;

    // Representacion legible de la fecha, ej. "2026-09-22 14:35:10".
    std::string fechaComoTexto() const;

    // true si la venta ocurrio en el mismo dia de calendario que "ahora".
    bool esDelDiaActual() const;

private:
    int numeroTransaccion_;
    MetodoPago metodoPago_;
    std::vector<DetalleVenta> detalles_;
    double total_;
    std::chrono::system_clock::time_point fecha_;
};

#endif // VENTA_H
