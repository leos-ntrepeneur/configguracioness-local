// IRepositorioCortes.h
//
// Interfaz para persistir el historial de cortes de caja. A diferencia de
// IRepositorioProductos/IRepositorioVentas (que guardan TODA la coleccion
// de una vez, sobreescribiendo el archivo completo cada vez que algo
// cambia), aqui el metodo es `agregar`: un corte, una vez cerrado, nunca
// se modifica ni se borra -- es un registro contable archivado, asi que
// la implementacion en disco solo necesita AÑADIR filas nuevas, nunca
// reescribir las que ya existen (ver RepositorioCortesCsv).

#ifndef I_REPOSITORIO_CORTES_H
#define I_REPOSITORIO_CORTES_H

#include <vector>

#include "CorteCaja.h"

class IRepositorioCortes {
public:
    virtual ~IRepositorioCortes() = default;

    // Agrega UN corte nuevo al historial persistido (no reemplaza los
    // anteriores).
    virtual void agregar(const CorteCaja& corte) = 0;

    virtual std::vector<CorteCaja> cargarTodos() = 0;
};

#endif // I_REPOSITORIO_CORTES_H
