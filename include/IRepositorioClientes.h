// IRepositorioClientes.h
//
// Interfaz para guardar/cargar la lista completa de clientes. Mismo
// patron que IRepositorioProductos (reescribe el archivo completo cada
// vez, a diferencia de IRepositorioCortes/IRepositorioAbonos que solo
// agregan filas): un cliente si se puede "editar" conceptualmente en el
// futuro (cambiar telefono, por ejemplo), asi que el archivo entero se
// vuelve a escribir cada vez que algo cambia.

#ifndef I_REPOSITORIO_CLIENTES_H
#define I_REPOSITORIO_CLIENTES_H

#include <vector>

#include "Cliente.h"

class IRepositorioClientes {
public:
    virtual ~IRepositorioClientes() = default;

    virtual void guardarTodos(const std::vector<Cliente>& clientes) = 0;
    virtual std::vector<Cliente> cargarTodos() = 0;
};

#endif // I_REPOSITORIO_CLIENTES_H
