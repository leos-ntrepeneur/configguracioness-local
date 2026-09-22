// RepositorioProductosCsv.h
//
// Implementacion concreta de IRepositorioProductos que persiste en un
// archivo CSV de texto plano. `: public IRepositorioProductos` es
// herencia publica: RepositorioProductosCsv "ES-UN" IRepositorioProductos,
// y puede usarse en cualquier lugar que espere esa interfaz (incluido
// dentro de un std::unique_ptr<IRepositorioProductos>, ver main.cpp).

#ifndef REPOSITORIO_PRODUCTOS_CSV_H
#define REPOSITORIO_PRODUCTOS_CSV_H

#include <string>

#include "IRepositorioProductos.h"

class RepositorioProductosCsv : public IRepositorioProductos {
public:
    explicit RepositorioProductosCsv(std::string rutaArchivo);

    // `override` (C++11) le dice al compilador "esto debe coincidir con un
    // metodo virtual de la clase base": si por error cambiara la firma
    // aqui sin actualizar la interfaz, el compilador lo marca como error
    // en vez de crear silenciosamente un metodo nuevo que nunca se llama.
    void guardarTodos(const std::vector<Producto>& productos) override;
    std::vector<Producto> cargarTodos() override;

private:
    std::string rutaArchivo_;
};

#endif // REPOSITORIO_PRODUCTOS_CSV_H
