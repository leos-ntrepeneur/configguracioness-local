// RepositorioInformacionNegocioCsv.h
//
// Persiste InformacionNegocio en un CSV de una sola fila de datos (mas su
// encabezado): data/negocio.csv. Es el mismo formato de "encabezado +
// filas" que los demas repositorios CSV, solo que aqui SIEMPRE hay como
// maximo una fila.

#ifndef REPOSITORIO_INFORMACION_NEGOCIO_CSV_H
#define REPOSITORIO_INFORMACION_NEGOCIO_CSV_H

#include <string>

#include "IRepositorioInformacionNegocio.h"

class RepositorioInformacionNegocioCsv : public IRepositorioInformacionNegocio {
public:
    explicit RepositorioInformacionNegocioCsv(std::string rutaArchivo);

    void guardar(const InformacionNegocio& informacion) override;
    InformacionNegocio cargar() override;

private:
    std::string rutaArchivo_;
};

#endif // REPOSITORIO_INFORMACION_NEGOCIO_CSV_H
