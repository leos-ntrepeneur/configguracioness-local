// IRepositorioAbonos.h
//
// Interfaz para persistir el historial de abonos. Mismo patron que
// IRepositorioCortes: un abono, una vez registrado, nunca se modifica ni
// se borra, asi que la interfaz es `agregar` (una fila a la vez) en vez
// de `guardarTodos` (todo el archivo reescrito).

#ifndef I_REPOSITORIO_ABONOS_H
#define I_REPOSITORIO_ABONOS_H

#include <vector>

#include "Abono.h"

class IRepositorioAbonos {
public:
    virtual ~IRepositorioAbonos() = default;

    virtual void agregar(const Abono& abono) = 0;
    virtual std::vector<Abono> cargarTodos() = 0;
};

#endif // I_REPOSITORIO_ABONOS_H
