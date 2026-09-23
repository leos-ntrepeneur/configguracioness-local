// IRepositorioInformacionNegocio.h
//
// Misma idea que IRepositorioProductos/IRepositorioVentas: una interfaz
// para guardar/cargar, hoy implementada en CSV, para poder migrar a
// SQLite despues sin que Menu/MainWindow se enteren del cambio. Aqui es
// un solo registro (no una lista), asi que la forma es mas simple:
// guardar/cargar sin necesidad de codigo ni busqueda.

#ifndef I_REPOSITORIO_INFORMACION_NEGOCIO_H
#define I_REPOSITORIO_INFORMACION_NEGOCIO_H

#include "InformacionNegocio.h"

class IRepositorioInformacionNegocio {
public:
    virtual ~IRepositorioInformacionNegocio() = default;

    virtual void guardar(const InformacionNegocio& informacion) = 0;

    // Si el archivo no existe todavia (primera ejecucion, o el negocio
    // nunca ha llenado sus datos), devuelve una InformacionNegocio vacia
    // en vez de lanzar una excepcion -- no tener datos capturados todavia
    // es un estado normal, no un error.
    virtual InformacionNegocio cargar() = 0;
};

#endif // I_REPOSITORIO_INFORMACION_NEGOCIO_H
