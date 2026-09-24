#include "CorteCaja.h"

#include <ctime>
#include <iomanip>
#include <sstream>

CorteCaja::CorteCaja(int numeroCorte, const ReporteVentasDia& reporte,
                      std::chrono::system_clock::time_point fechaHoraCierre)
    : numeroCorte_(numeroCorte),
      fechaHoraCierre_(fechaHoraCierre),
      folioInicial_(0),
      folioFinal_(0),
      numeroTransacciones_(reporte.numeroTransacciones),
      totalVendido_(reporte.totalVendido),
      totalEfectivo_(0.0),
      totalTarjetaCredito_(0.0),
      totalTarjetaDebito_(0.0),
      totalFiado_(0.0) {
    for (const TransaccionDia& t : reporte.transacciones) {
        // reporte.transacciones viene ordenado cronologicamente (ver
        // GestorVentas::generarReporteDelDia), pero folioInicial_/Final_ se
        // calculan con min/max explicito en vez de asumir que el primero y
        // el ultimo elemento ya son el minimo y el maximo -- los folios
        // SIEMPRE crecen con el tiempo (GestorVentas los asigna
        // secuencialmente), asi que en la practica coinciden, pero calcular
        // min/max no cuesta nada y no depende de ese supuesto.
        if (folioInicial_ == 0 || t.numeroTransaccion < folioInicial_) {
            folioInicial_ = t.numeroTransaccion;
        }
        if (t.numeroTransaccion > folioFinal_) {
            folioFinal_ = t.numeroTransaccion;
        }

        // switch sin `default` (ver MetodoPago.cpp): si se agrega un metodo
        // de pago nuevo y se olvida sumarlo aqui, el compilador avisa.
        switch (t.metodoPago) {
            case MetodoPago::Efectivo:
                totalEfectivo_ += t.total;
                break;
            case MetodoPago::TarjetaCredito:
                totalTarjetaCredito_ += t.total;
                break;
            case MetodoPago::TarjetaDebito:
                totalTarjetaDebito_ += t.total;
                break;
            case MetodoPago::Fiado:
                totalFiado_ += t.total;
                break;
        }
    }
}

CorteCaja::CorteCaja(int numeroCorte, std::chrono::system_clock::time_point fechaHoraCierre, int folioInicial,
                      int folioFinal, int numeroTransacciones, double totalVendido, double totalEfectivo,
                      double totalTarjetaCredito, double totalTarjetaDebito, double totalFiado)
    : numeroCorte_(numeroCorte),
      fechaHoraCierre_(fechaHoraCierre),
      folioInicial_(folioInicial),
      folioFinal_(folioFinal),
      numeroTransacciones_(numeroTransacciones),
      totalVendido_(totalVendido),
      totalEfectivo_(totalEfectivo),
      totalTarjetaCredito_(totalTarjetaCredito),
      totalTarjetaDebito_(totalTarjetaDebito),
      totalFiado_(totalFiado) {}

int CorteCaja::getNumeroCorte() const { return numeroCorte_; }
std::chrono::system_clock::time_point CorteCaja::getFechaHoraCierre() const { return fechaHoraCierre_; }
int CorteCaja::getFolioInicial() const { return folioInicial_; }
int CorteCaja::getFolioFinal() const { return folioFinal_; }
int CorteCaja::getNumeroTransacciones() const { return numeroTransacciones_; }
double CorteCaja::getTotalVendido() const { return totalVendido_; }
double CorteCaja::getTotalEfectivo() const { return totalEfectivo_; }
double CorteCaja::getTotalTarjetaCredito() const { return totalTarjetaCredito_; }
double CorteCaja::getTotalTarjetaDebito() const { return totalTarjetaDebito_; }
double CorteCaja::getTotalFiado() const { return totalFiado_; }

std::string CorteCaja::fechaHoraCierreComoTexto() const {
    // Misma logica que Venta::fechaComoTexto(): time_point -> time_t ->
    // std::tm en hora LOCAL -> texto con std::put_time.
    std::time_t comoTimeT = std::chrono::system_clock::to_time_t(fechaHoraCierre_);
    std::tm tm{};
#if defined(_WIN32)
    localtime_s(&tm, &comoTimeT);
#else
    localtime_r(&comoTimeT, &tm);
#endif
    std::ostringstream salida;
    salida << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return salida.str();
}
