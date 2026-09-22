#include "Venta.h"
#include "Excepciones.h"

#include <ctime>
#include <iomanip>
#include <sstream>

Venta::Venta(std::vector<DetalleVenta> detalles)
    : detalles_(std::move(detalles)),
      total_(0.0),
      fecha_(std::chrono::system_clock::now()) {
    if (detalles_.empty()) {
        throw EntradaInvalida("Una venta debe tener al menos un producto.");
    }
    for (const DetalleVenta& detalle : detalles_) {
        total_ += detalle.getSubtotal();
    }
}

const std::vector<DetalleVenta>& Venta::getDetalles() const { return detalles_; }
double Venta::getTotal() const { return total_; }
std::chrono::system_clock::time_point Venta::getFecha() const { return fecha_; }

std::string Venta::fechaComoTexto() const {
    // system_clock::to_time_t convierte el time_point (tipo moderno de
    // <chrono>) al std::time_t clasico de C, que es lo que entiende
    // std::localtime. C++ mantiene ambas APIs: chrono para hacer aritmetica
    // de tiempo con seguridad de tipos, y la de C para formatear texto.
    std::time_t comoTimeT = std::chrono::system_clock::to_time_t(fecha_);
    std::tm tm{};
#if defined(_WIN32)
    localtime_s(&tm, &comoTimeT); // version seguro para hilos en Windows.
#else
    localtime_r(&comoTimeT, &tm); // equivalente en Linux/macOS.
#endif
    std::ostringstream salida;
    salida << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return salida.str();
}

bool Venta::esDelDiaActual() const {
    std::time_t ventaTimeT = std::chrono::system_clock::to_time_t(fecha_);
    std::time_t ahoraTimeT = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    std::tm ventaTm{};
    std::tm ahoraTm{};
#if defined(_WIN32)
    localtime_s(&ventaTm, &ventaTimeT);
    localtime_s(&ahoraTm, &ahoraTimeT);
#else
    localtime_r(&ventaTimeT, &ventaTm);
    localtime_r(&ahoraTimeT, &ahoraTm);
#endif
    return ventaTm.tm_year == ahoraTm.tm_year &&
           ventaTm.tm_yday == ahoraTm.tm_yday;
}
