#include "RepositorioProductosCsv.h"
#include "CsvUtil.h"

#include <filesystem>
#include <fstream>
#include <stdexcept>

RepositorioProductosCsv::RepositorioProductosCsv(std::string rutaArchivo)
    : rutaArchivo_(std::move(rutaArchivo)) {}

void RepositorioProductosCsv::guardarTodos(const std::vector<Producto>& productos) {
    // std::filesystem (C++17) es la API estandar para rutas y carpetas, y
    // reemplaza el manejo manual de rutas que en C se hacia con strings a
    // mano. Creamos la carpeta contenedora (ej. "data/") si no existe
    // todavia, para que el programa funcione aunque se ejecute desde un
    // directorio de build distinto al del proyecto.
    std::filesystem::path ruta(rutaArchivo_);
    if (ruta.has_parent_path()) {
        std::filesystem::create_directories(ruta.parent_path());
    }

    std::ofstream archivo(rutaArchivo_, std::ios::trunc);
    if (!archivo.is_open()) {
        throw std::runtime_error("No se pudo abrir '" + rutaArchivo_ + "' para guardar productos.");
    }

    archivo << "codigo,nombre,precio,stock,categoria,stockMinimo\n";
    for (const Producto& p : productos) {
        archivo << p.getCodigo() << ',' << p.getNombre() << ',' << p.getPrecio() << ','
                << p.getStock() << ',' << p.getCategoria() << ',' << p.getStockMinimo() << '\n';
    }
}

std::vector<Producto> RepositorioProductosCsv::cargarTodos() {
    std::vector<Producto> resultado;

    std::ifstream archivo(rutaArchivo_);
    if (!archivo.is_open()) {
        // No es un error: la primera vez que corre el programa el archivo
        // todavia no existe. Devolvemos inventario vacio.
        return resultado;
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
        if (campos.size() != 6) {
            continue; // fila corrupta o incompleta: se ignora en vez de tronar.
        }

        try {
            // std::stod/std::stoi convierten string -> double/int y lanzan
            // std::invalid_argument si el texto no es un numero valido;
            // ese catch de abajo cubre tanto eso como las validaciones
            // propias del constructor de Producto (EntradaInvalida).
            resultado.emplace_back(campos[0], campos[1], std::stod(campos[2]),
                                    std::stoi(campos[3]), campos[4], std::stoi(campos[5]));
        } catch (const std::exception&) {
            continue;
        }
    }
    return resultado;
}
