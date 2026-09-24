// RepositorioAbonosCsv.h
//
// Implementacion CSV append-only de IRepositorioAbonos -- ver el
// comentario grande en RepositorioCortesCsv.h, misma idea exacta aplicada
// a abonos en vez de a cortes de caja.

#ifndef REPOSITORIO_ABONOS_CSV_H
#define REPOSITORIO_ABONOS_CSV_H

#include <string>

#include "IRepositorioAbonos.h"

class RepositorioAbonosCsv : public IRepositorioAbonos {
public:
    explicit RepositorioAbonosCsv(std::string rutaArchivo);

    void agregar(const Abono& abono) override;
    std::vector<Abono> cargarTodos() override;

private:
    std::string rutaArchivo_;
};

#endif // REPOSITORIO_ABONOS_CSV_H
