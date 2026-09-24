#include "Abono.h"
#include "Excepciones.h"

#include <cmath>
#include <ctime>
#include <iomanip>
#include <sstream>

Abono::Abono(int numeroAbono, int clienteId, double monto)
    : Abono(numeroAbono, clienteId, monto, std::chrono::system_clock::now()) {}

Abono::Abono(int numeroAbono, int clienteId, double monto, std::chrono::system_clock::time_point fecha)
    : numeroAbono_(numeroAbono), clienteId_(clienteId), monto_(monto), fecha_(fecha) {
    if (numeroAbono_ <= 0) {
        throw EntradaInvalida("El numero de abono debe ser mayor a cero.");
    }
    if (clienteId_ <= 0) {
        throw EntradaInvalida("Un abono debe tener un cliente asociado.");
    }
    if (!std::isfinite(monto_) || monto_ <= 0.0) {
        throw EntradaInvalida("El monto del abono debe ser mayor a cero.");
    }
}

int Abono::getNumeroAbono() const { return numeroAbono_; }
int Abono::getClienteId() const { return clienteId_; }
double Abono::getMonto() const { return monto_; }
std::chrono::system_clock::time_point Abono::getFecha() const { return fecha_; }

std::string Abono::fechaComoTexto() const {
    std::time_t comoTimeT = std::chrono::system_clock::to_time_t(fecha_);
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
