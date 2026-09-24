// RepositorioClientesCsv.h
//
// Implementacion CSV de IRepositorioClientes. Mismo patron que
// RepositorioProductosCsv: reescribe el archivo completo cada vez
// (std::ios::trunc), un renglon por cliente.

#ifndef REPOSITORIO_CLIENTES_CSV_H
#define REPOSITORIO_CLIENTES_CSV_H

#include <string>

#include "IRepositorioClientes.h"

class RepositorioClientesCsv : public IRepositorioClientes {
public:
    explicit RepositorioClientesCsv(std::string rutaArchivo);

    void guardarTodos(const std::vector<Cliente>& clientes) override;
    std::vector<Cliente> cargarTodos() override;

private:
    std::string rutaArchivo_;
};

#endif // REPOSITORIO_CLIENTES_CSV_H
