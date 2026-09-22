// RepositorioVentasCsv.h
//
// Persiste el historial de ventas en un CSV "aplanado": cada DetalleVenta
// es una fila, y todas las filas de una misma Venta comparten el mismo
// ventaId (un numero de fila 1, 2, 3... asignado al guardar, no un campo
// de Venta). Es la forma tipica de representar una relacion "uno a
// muchos" (una venta, varios detalles) en un archivo plano; en SQLite esto
// seria naturalmente dos tablas con una llave foranea.

#ifndef REPOSITORIO_VENTAS_CSV_H
#define REPOSITORIO_VENTAS_CSV_H

#include <string>

#include "IRepositorioVentas.h"

class RepositorioVentasCsv : public IRepositorioVentas {
public:
    explicit RepositorioVentasCsv(std::string rutaArchivo);

    void guardarTodas(const std::vector<Venta>& ventas) override;
    std::vector<Venta> cargarTodas() override;

private:
    std::string rutaArchivo_;
};

#endif // REPOSITORIO_VENTAS_CSV_H
