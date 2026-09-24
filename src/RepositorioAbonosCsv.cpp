#include "RepositorioAbonosCsv.h"
#include "CsvUtil.h"

#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace {

// Inverso de Abono::fechaComoTexto() -- mismo parseo que en
// RepositorioVentasCsv.cpp/RepositorioCortesCsv.cpp.
std::chrono::system_clock::time_point parsearFecha(const std::string& texto) {
    std::tm tm{};
    std::istringstream flujo(texto);
    flujo >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    if (flujo.fail()) {
        throw std::runtime_error("Fecha invalida en archivo de abonos: " + texto);
    }
    std::time_t comoTimeT = std::mktime(&tm);
    return std::chrono::system_clock::from_time_t(comoTimeT);
}

constexpr int NUMERO_COLUMNAS = 4;

} // namespace

RepositorioAbonosCsv::RepositorioAbonosCsv(std::string rutaArchivo) : rutaArchivo_(std::move(rutaArchivo)) {}

void RepositorioAbonosCsv::agregar(const Abono& abono) {
    std::filesystem::path ruta(rutaArchivo_);
    if (ruta.has_parent_path()) {
        std::filesystem::create_directories(ruta.parent_path());
    }

    bool escribirEncabezado = !std::filesystem::exists(rutaArchivo_);

    std::ofstream archivo(rutaArchivo_, std::ios::app);
    if (!archivo.is_open()) {
        throw std::runtime_error("No se pudo abrir '" + rutaArchivo_ + "' para guardar el abono.");
    }

    if (escribirEncabezado) {
        archivo << "numeroAbono,clienteId,monto,fecha\n";
    }

    archivo << abono.getNumeroAbono() << ',' << abono.getClienteId() << ',' << abono.getMonto() << ','
            << abono.fechaComoTexto() << '\n';
}

std::vector<Abono> RepositorioAbonosCsv::cargarTodos() {
    std::vector<Abono> resultado;

    std::ifstream archivo(rutaArchivo_);
    if (!archivo.is_open()) {
        return resultado; // primera ejecucion: todavia no hay abonos guardados.
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
        if (static_cast<int>(campos.size()) != NUMERO_COLUMNAS) {
            continue;
        }

        try {
            resultado.emplace_back(std::stoi(campos[0]), std::stoi(campos[1]), std::stod(campos[2]),
                                    parsearFecha(campos[3]));
        } catch (const std::exception&) {
            continue;
        }
    }

    return resultado;
}
