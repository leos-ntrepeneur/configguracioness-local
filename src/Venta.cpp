#include "Venta.h"
#include "Excepciones.h"

#include <ctime>
#include <iomanip>
#include <sstream>

// Constructor delegante (C++11): en vez de repetir la logica de validacion
// y calculo de total, este constructor simplemente le pasa la fecha actual
// al otro constructor y deja que el haga todo el trabajo. En C++ anterior
// a 2011 (o en C) esto se resolvia con un metodo privado "inicializar()"
// llamado desde ambos constructores; delegar es mas directo.
Venta::Venta(std::vector<DetalleVenta> detalles, int numeroTransaccion, MetodoPago metodoPago, int clienteId,
             std::string nombreCliente)
    : Venta(std::move(detalles), numeroTransaccion, metodoPago, clienteId, std::move(nombreCliente),
            std::chrono::system_clock::now()) {}

Venta::Venta(std::vector<DetalleVenta> detalles, int numeroTransaccion, MetodoPago metodoPago, int clienteId,
             std::string nombreCliente, std::chrono::system_clock::time_point fecha)
    : numeroTransaccion_(numeroTransaccion),
      metodoPago_(metodoPago),
      clienteId_(clienteId),
      nombreCliente_(std::move(nombreCliente)),
      detalles_(std::move(detalles)),
      total_(0.0),
      fecha_(fecha) {
    if (detalles_.empty()) {
        throw EntradaInvalida("Una venta debe tener al menos un producto.");
    }
    if (numeroTransaccion_ <= 0) {
        throw EntradaInvalida("El numero de transaccion debe ser mayor a cero.");
    }
    // Un cliente asociado SOLO tiene sentido para una venta al fiado, y
    // una venta al fiado SIEMPRE necesita un cliente -- lo uno implica lo
    // otro. Validarlo aqui (en vez de confiar en que quien llame lo haga
    // bien) evita que un bug en Menu/PestanaVentas cree una Venta
    // inconsistente que nadie mas detectaria despues.
    if (metodoPago_ == MetodoPago::Fiado) {
        if (clienteId_ <= 0 || nombreCliente_.empty()) {
            throw EntradaInvalida("Una venta al fiado debe tener un cliente asociado.");
        }
    } else if (clienteId_ != 0 || !nombreCliente_.empty()) {
        throw EntradaInvalida("Solo las ventas al fiado pueden tener un cliente asociado.");
    }
    for (const DetalleVenta& detalle : detalles_) {
        total_ += detalle.getSubtotal();
    }
}

int Venta::getNumeroTransaccion() const { return numeroTransaccion_; }
MetodoPago Venta::getMetodoPago() const { return metodoPago_; }
int Venta::getClienteId() const { return clienteId_; }
const std::string& Venta::getNombreCliente() const { return nombreCliente_; }
bool Venta::esFiado() const { return metodoPago_ == MetodoPago::Fiado; }
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
