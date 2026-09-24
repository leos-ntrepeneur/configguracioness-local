// Abono.h
//
// Un pago (parcial o total) que un cliente hace para reducir su saldo
// pendiente de ventas al fiado. Es inmutable, igual que Venta y CorteCaja:
// una vez registrado un abono, no se edita ni se borra -- si hubo un
// error, se corrige con informacion nueva (un ajuste), no reescribiendo el
// historial. GestorCreditos::saldoPendiente sale de sumar Ventas al fiado
// de un cliente y restarle sus Abonos, nunca de un campo "saldo" editado a
// mano en ningun lado.

#ifndef ABONO_H
#define ABONO_H

#include <chrono>
#include <string>

class Abono {
public:
    // `numeroAbono` lo asigna GestorCreditos (secuencial). La fecha se
    // toma automaticamente al construir -- es el camino normal para un
    // abono que esta ocurriendo AHORA.
    Abono(int numeroAbono, int clienteId, double monto);

    // Igual, pero fijando la fecha manualmente. Solo lo usa
    // RepositorioAbonosCsv al reconstruir el historial desde archivo.
    Abono(int numeroAbono, int clienteId, double monto, std::chrono::system_clock::time_point fecha);

    int getNumeroAbono() const;
    int getClienteId() const;
    double getMonto() const;
    std::chrono::system_clock::time_point getFecha() const;
    std::string fechaComoTexto() const;

private:
    int numeroAbono_;
    int clienteId_;
    double monto_;
    std::chrono::system_clock::time_point fecha_;
};

#endif // ABONO_H
