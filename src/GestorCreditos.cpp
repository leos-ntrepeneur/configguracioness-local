#include "GestorCreditos.h"
#include "Excepciones.h"

#include <chrono>

GestorCreditos::GestorCreditos(const GestorVentas& gestorVentas) : gestorVentas_(gestorVentas) {}

void GestorCreditos::cargarAbonos(std::vector<Abono> abonos) {
    abonos_ = std::move(abonos);

    siguienteNumeroAbono_ = 1;
    for (const Abono& abono : abonos_) {
        if (abono.getNumeroAbono() >= siguienteNumeroAbono_) {
            siguienteNumeroAbono_ = abono.getNumeroAbono() + 1;
        }
    }
}

const Abono& GestorCreditos::registrarAbono(int clienteId, double monto) {
    double pendiente = saldoPendiente(clienteId);
    if (monto > pendiente) {
        throw EntradaInvalida("El abono no puede ser mayor al saldo pendiente del cliente.");
    }

    int numeroAbono = siguienteNumeroAbono_++;
    abonos_.emplace_back(numeroAbono, clienteId, monto);
    return abonos_.back();
}

double GestorCreditos::totalFiado(int clienteId) const {
    double total = 0.0;
    for (const Venta& venta : gestorVentas_.listarVentas()) {
        if (venta.esFiado() && venta.getClienteId() == clienteId) {
            total += venta.getTotal();
        }
    }
    return total;
}

double GestorCreditos::totalAbonado(int clienteId) const {
    double total = 0.0;
    for (const Abono& abono : abonos_) {
        if (abono.getClienteId() == clienteId) {
            total += abono.getMonto();
        }
    }
    return total;
}

double GestorCreditos::saldoPendiente(int clienteId) const {
    return totalFiado(clienteId) - totalAbonado(clienteId);
}

const std::vector<Abono>& GestorCreditos::listarAbonos() const {
    return abonos_;
}
