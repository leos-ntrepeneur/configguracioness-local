// Venta.h
//
// Representa una transaccion ya cerrada: la fecha/hora en que ocurrio, las
// lineas que la componen (DetalleVenta) y el total. Una vez creada, una
// Venta no se edita (no hay setters): en un punto de venta real, corregir
// una venta se hace con una venta de ajuste/nota de credito, no reescribiendo
// el historial. Por eso el constructor calcula el total una sola vez y no
// se expone forma de modificar detalles_ despues.
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

class Venta {
public:
    // Recibe los detalles ya armados (y validados) por quien la crea
    // (GestorVentas). La fecha se toma automaticamente al construir: es el
    // camino normal para una venta que esta ocurriendo AHORA.
    explicit Venta(std::vector<DetalleVenta> detalles);

    // Igual, pero fijando la fecha manualmente. Solo lo usa
    // RepositorioVentasCsv al reconstruir el historial desde archivo: ahi
    // la venta ya ocurrio en el pasado y hay que respetar su fecha
    // original, no ponerle la hora actual.
    Venta(std::vector<DetalleVenta> detalles, std::chrono::system_clock::time_point fecha);

    const std::vector<DetalleVenta>& getDetalles() const;
    double getTotal() const;
    std::chrono::system_clock::time_point getFecha() const;

    // Representacion legible de la fecha, ej. "2026-09-22 14:35:10".
    std::string fechaComoTexto() const;

    // true si la venta ocurrio en el mismo dia de calendario que "ahora".
    bool esDelDiaActual() const;

private:
    std::vector<DetalleVenta> detalles_;
    double total_;
    std::chrono::system_clock::time_point fecha_;
};

#endif // VENTA_H
