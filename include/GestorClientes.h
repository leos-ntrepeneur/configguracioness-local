// GestorClientes.h
//
// Dueño de la coleccion de clientes (los que compran al fiado). Mismo
// patron que Inventario: un map por id para busqueda O(log n), asigna el
// siguiente id de forma secuencial (igual que el folio de GestorVentas),
// y no valida nada por su cuenta -- esa responsabilidad ya es de Cliente
// (su constructor lanza EntradaInvalida si algo esta mal).

#ifndef GESTOR_CLIENTES_H
#define GESTOR_CLIENTES_H

#include <map>
#include <string>
#include <vector>

#include "Cliente.h"

class GestorClientes {
public:
    GestorClientes() = default;

    // Crea un Cliente nuevo con el siguiente id disponible, lo agrega a
    // la coleccion y devuelve una referencia const a el (vive dentro de
    // clientes_, valida mientras GestorClientes exista).
    const Cliente& agregarCliente(std::string nombre, std::string telefono);

    // Devuelve `nullptr` si no existe -- "el id no existe" es una
    // situacion normal de UI (ej. el usuario tecleo mal), no un error de
    // programacion.
    const Cliente* buscarPorId(int id) const;

    // Busqueda parcial insensible a mayusculas, mismo criterio que
    // Inventario::buscarPorNombre.
    std::vector<Cliente> buscarPorNombre(const std::string& textoParcial) const;

    std::vector<Cliente> listarTodos() const;

    // Reemplaza la coleccion completa con lo leido de persistencia al
    // iniciar el programa, y recalcula el siguiente id disponible a
    // partir del mayor id cargado (mismo patron que GestorVentas::
    // cargarVentas con los folios).
    void cargarClientes(std::vector<Cliente> clientes);

private:
    std::map<int, Cliente> clientes_;
    int siguienteId_ = 1;
};

#endif // GESTOR_CLIENTES_H
