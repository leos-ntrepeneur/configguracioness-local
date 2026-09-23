#include "RepositorioInformacionNegocioCsv.h"
#include "CsvUtil.h"

#include <filesystem>
#include <fstream>
#include <stdexcept>

RepositorioInformacionNegocioCsv::RepositorioInformacionNegocioCsv(std::string rutaArchivo)
    : rutaArchivo_(std::move(rutaArchivo)) {}

void RepositorioInformacionNegocioCsv::guardar(const InformacionNegocio& informacion) {
    std::filesystem::path ruta(rutaArchivo_);
    if (ruta.has_parent_path()) {
        std::filesystem::create_directories(ruta.parent_path());
    }

    std::ofstream archivo(rutaArchivo_, std::ios::trunc);
    if (!archivo.is_open()) {
        throw std::runtime_error("No se pudo abrir '" + rutaArchivo_ + "' para guardar la informacion del negocio.");
    }

    archivo << "nombre,direccion,telefono,rfc\n";
    archivo << informacion.getNombre() << ',' << informacion.getDireccion() << ','
            << informacion.getTelefono() << ',' << informacion.getRfc() << '\n';
}

InformacionNegocio RepositorioInformacionNegocioCsv::cargar() {
    std::ifstream archivo(rutaArchivo_);
    if (!archivo.is_open()) {
        return InformacionNegocio(); // primera ejecucion: aun no hay datos capturados.
    }

    std::string linea;
    bool esEncabezado = true;
    while (std::getline(archivo, linea)) {
        if (esEncabezado) {
            esEncabezado = false;
            continue;
        }
        if (linea.empty()) {
            continue;
        }

        std::vector<std::string> campos = csv::dividirLinea(linea);
        if (campos.size() != 4) {
            continue; // fila corrupta: se ignora, igual que en los demas repositorios.
        }

        try {
            return InformacionNegocio(campos[0], campos[1], campos[2], campos[3]);
        } catch (const std::exception&) {
            return InformacionNegocio(); // fila invalida: mejor vacia que tronar el arranque.
        }
    }

    return InformacionNegocio(); // archivo existe pero no tiene datos todavia.
}
