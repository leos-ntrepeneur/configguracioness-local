// GestorCreditos.h
//
// Lleva el seguimiento de cuanto debe cada cliente por ventas al fiado y
// registra sus abonos (pagos). No guarda un "saldo" como campo propio en
// ningun lado: totalFiado/totalAbonado/saldoPendiente se CALCULAN cada vez
// a partir de GestorVentas (ventas con MetodoPago::Fiado de ese cliente) y
// de los Abono ya registrados -- la unica fuente de verdad son esas dos
// listas, nunca un numero editado aparte que se pudiera desincronizar.

#ifndef GESTOR_CREDITOS_H
#define GESTOR_CREDITOS_H

#include <vector>

#include "Abono.h"
#include "GestorVentas.h"

class GestorCreditos {
public:
    // Recibe GestorVentas por referencia CONSTANTE: solo necesita leer su
    // historial (listarVentas()) para sumar lo fiado, nunca lo modifica.
    explicit GestorCreditos(const GestorVentas& gestorVentas);

    // Reemplaza el historial de abonos en memoria con lo leido de
    // persistencia al iniciar el programa, y recalcula el siguiente
    // numero de abono (mismo patron que GestorVentas::cargarVentas).
    void cargarAbonos(std::vector<Abono> abonos);

    // Registra un abono nuevo para `clienteId`. Valida que el monto no
    // exceda el saldo pendiente actual (evita un saldo negativo confuso,
    // "el cliente debe -$50" no tiene sentido de negocio) y lanza
    // EntradaInvalida si lo excede. Quien llame (Menu/PestanaClientes) es
    // responsable de persistirlo despues via IRepositorioAbonos, igual
    // que con GestorVentas::registrarVenta.
    const Abono& registrarAbono(int clienteId, double monto);

    // Suma el total de TODAS las ventas al fiado de `clienteId` (sin
    // importar si ya se abonaron o no).
    double totalFiado(int clienteId) const;
    // Suma de todos los abonos ya registrados de `clienteId`.
    double totalAbonado(int clienteId) const;
    // totalFiado - totalAbonado; nunca queda negativo porque
    // registrarAbono no deja abonar mas de lo que se debe.
    double saldoPendiente(int clienteId) const;

    const std::vector<Abono>& listarAbonos() const;

private:
    const GestorVentas& gestorVentas_;
    std::vector<Abono> abonos_;
    int siguienteNumeroAbono_ = 1;
};

#endif // GESTOR_CREDITOS_H
