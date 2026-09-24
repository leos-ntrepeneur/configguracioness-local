// Cliente.h
//
// Un cliente de confianza al que el negocio le vende "al fiado" (a
// credito, se cobra despues) -- ver MetodoPago::Fiado y GestorCreditos.h.
// Es una entidad simple a proposito: solo lo minimo para poder identificar
// a quien se le fio y como contactarlo. El saldo que debe NO vive aqui (no
// es un campo de Cliente): se calcula sumando sus ventas al fiado y
// restando sus abonos (ver GestorCreditos::saldoPendiente), para que nunca
// se desincronice de la fuente real de la deuda.

#ifndef CLIENTE_H
#define CLIENTE_H

#include <string>

class Cliente {
public:
    static constexpr int NOMBRE_LONGITUD_MAXIMA = 100;
    static constexpr int TELEFONO_LONGITUD_MAXIMA = 20;

    // `id` lo asigna GestorClientes (secuencial), igual que el folio de
    // Venta o el numero de CorteCaja. El telefono es opcional.
    Cliente(int id, std::string nombre, std::string telefono = "");

    int getId() const;
    const std::string& getNombre() const;
    const std::string& getTelefono() const;

private:
    int id_;
    std::string nombre_;
    std::string telefono_;
};

#endif // CLIENTE_H
