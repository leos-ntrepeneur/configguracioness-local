// CorteCaja.h
//
// "Corte de caja" (tambien llamado cierre del dia): una fotografia
// PERMANENTE del resumen de ventas del dia en el momento en que se cierra
// -- folio inicial/final incluidos, numero de transacciones, total vendido
// y el desglose por metodo de pago. A diferencia de ReporteVentasDia (que
// se recalcula cada vez que se consulta y siempre refleja "hoy hasta este
// instante"), un CorteCaja queda archivado tal cual estaba al momento del
// cierre, con su propio numero de corte -- es un registro contable, no una
// consulta en vivo.
//
// Solo se guardan TOTALES (no el detalle por producto/categoria/
// transaccion individual): ventas.csv ya tiene ese detalle completo, folio
// por folio, si algun dia hiciera falta reconstruirlo. Guardar todo dos
// veces en formatos distintos seria duplicar datos sin necesidad.

#ifndef CORTE_CAJA_H
#define CORTE_CAJA_H

#include <chrono>
#include <string>

#include "GestorVentas.h" // ReporteVentasDia, TransaccionDia
#include "MetodoPago.h"

class CorteCaja {
public:
    // Arma un corte "fresco" a partir del ReporteVentasDia recien
    // calculado (ver GestorCortes::cerrarDia): de ahi saca el rango de
    // folios incluidos y suma los totales por metodo de pago transaccion
    // por transaccion.
    CorteCaja(int numeroCorte, const ReporteVentasDia& reporte,
              std::chrono::system_clock::time_point fechaHoraCierre);

    // Reconstruye un corte YA GUARDADO desde el CSV (ver
    // RepositorioCortesCsv) -- recibe los totales directamente porque ahi
    // ya no esta disponible el ReporteVentasDia original con el que se
    // genero.
    CorteCaja(int numeroCorte, std::chrono::system_clock::time_point fechaHoraCierre, int folioInicial,
              int folioFinal, int numeroTransacciones, double totalVendido, double totalEfectivo,
              double totalTarjetaCredito, double totalTarjetaDebito);

    int getNumeroCorte() const;
    std::chrono::system_clock::time_point getFechaHoraCierre() const;
    // Representacion legible, ej. "2026-09-23 21:05:10" (mismo formato que
    // Venta::fechaComoTexto()).
    std::string fechaHoraCierreComoTexto() const;

    // 0 en ambos si el dia se cerro sin ninguna venta (un "corte en cero"
    // sigue siendo un registro valido para algunos negocios: deja
    // constancia de que ese dia no hubo movimiento).
    int getFolioInicial() const;
    int getFolioFinal() const;

    int getNumeroTransacciones() const;
    double getTotalVendido() const;
    double getTotalEfectivo() const;
    double getTotalTarjetaCredito() const;
    double getTotalTarjetaDebito() const;

private:
    int numeroCorte_;
    std::chrono::system_clock::time_point fechaHoraCierre_;
    int folioInicial_;
    int folioFinal_;
    int numeroTransacciones_;
    double totalVendido_;
    double totalEfectivo_;
    double totalTarjetaCredito_;
    double totalTarjetaDebito_;
};

#endif // CORTE_CAJA_H
