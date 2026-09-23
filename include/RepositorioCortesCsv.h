// RepositorioCortesCsv.h
//
// Implementacion CSV de IRepositorioCortes. `agregar` ABRE EL ARCHIVO EN
// MODO APPEND (a diferencia de RepositorioProductosCsv/RepositorioVentasCsv,
// que reescriben el archivo completo con std::ios::trunc): cada corte es
// un registro contable cerrado, asi que la unica operacion valida es
// sumar filas nuevas al final, nunca reescribir las que ya existen.

#ifndef REPOSITORIO_CORTES_CSV_H
#define REPOSITORIO_CORTES_CSV_H

#include <string>

#include "IRepositorioCortes.h"

class RepositorioCortesCsv : public IRepositorioCortes {
public:
    explicit RepositorioCortesCsv(std::string rutaArchivo);

    void agregar(const CorteCaja& corte) override;
    std::vector<CorteCaja> cargarTodos() override;

private:
    std::string rutaArchivo_;
};

#endif // REPOSITORIO_CORTES_CSV_H
