// IRepositorioVentas.h
//
// Misma idea que IRepositorioProductos, pero para el historial de ventas.
// Ver los comentarios de IRepositorioProductos.h para el porque del
// destructor virtual y de los metodos virtuales puros.

#ifndef I_REPOSITORIO_VENTAS_H
#define I_REPOSITORIO_VENTAS_H

#include <vector>

#include "Venta.h"

class IRepositorioVentas {
public:
    virtual ~IRepositorioVentas() = default;

    virtual void guardarTodas(const std::vector<Venta>& ventas) = 0;
    virtual std::vector<Venta> cargarTodas() = 0;
};

#endif // I_REPOSITORIO_VENTAS_H
