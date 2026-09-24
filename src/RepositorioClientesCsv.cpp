#include "RepositorioClientesCsv.h"
#include "CsvUtil.h"

#include <filesystem>
#include <fstream>
#include <stdexcept>

RepositorioClientesCsv::RepositorioClientesCsv(std::string rutaArchivo) : rutaArchivo_(std::move(rutaArchivo)) {}

void RepositorioClientesCsv::guardarTodos(const std::vector<Cliente>& clientes) {
    std::filesystem::path ruta(rutaArchivo_);
    if (ruta.has_parent_path()) {
        std::filesystem::create_directories(ruta.parent_path());
    }

    std::ofstream archivo(rutaArchivo_, std::ios::trunc);
    if (!archivo.is_open()) {
        throw std::runtime_error("No se pudo abrir '" + rutaArchivo_ + "' para guardar clientes.");
    }

    archivo << "id,nombre,telefono\n";
    for (const Cliente& c : clientes) {
        archivo << c.getId() << ',' << c.getNombre() << ',' << c.getTelefono() << '\n';
    }
}

std::vector<Cliente> RepositorioClientesCsv::cargarTodos() {
    std::vector<Cliente> resultado;

    std::ifstream archivo(rutaArchivo_);
    if (!archivo.is_open()) {
        return resultado; // primera ejecucion: todavia no hay clientes.
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
        if (campos.size() != 3) {
            continue; // fila corrupta o incompleta: se ignora.
        }

        try {
            resultado.emplace_back(std::stoi(campos[0]), campos[1], campos[2]);
        } catch (const std::exception&) {
            continue;
        }
    }
    return resultado;
}
