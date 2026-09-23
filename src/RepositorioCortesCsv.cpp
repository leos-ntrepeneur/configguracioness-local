#include "RepositorioCortesCsv.h"
#include "CsvUtil.h"

#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace {

// Mismo parseo que RepositorioVentasCsv.cpp (inverso de
// CorteCaja::fechaHoraCierreComoTexto()) -- no existe todavia un lugar
// compartido para esto porque cada repositorio CSV lo necesita en un tipo
// de dato distinto (Venta aqui, CorteCaja alla).
std::chrono::system_clock::time_point parsearFecha(const std::string& texto) {
    std::tm tm{};
    std::istringstream flujo(texto);
    flujo >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    if (flujo.fail()) {
        throw std::runtime_error("Fecha invalida en archivo de cortes: " + texto);
    }
    std::time_t comoTimeT = std::mktime(&tm);
    return std::chrono::system_clock::from_time_t(comoTimeT);
}

constexpr int NUMERO_COLUMNAS = 9;

} // namespace

RepositorioCortesCsv::RepositorioCortesCsv(std::string rutaArchivo) : rutaArchivo_(std::move(rutaArchivo)) {}

void RepositorioCortesCsv::agregar(const CorteCaja& corte) {
    std::filesystem::path ruta(rutaArchivo_);
    if (ruta.has_parent_path()) {
        std::filesystem::create_directories(ruta.parent_path());
    }

    // El encabezado solo se escribe si el archivo TODAVIA NO EXISTE -- a
    // diferencia de guardarTodos/guardarTodas (que truncan y reescriben
    // encabezado + todas las filas cada vez), aqui cada llamada a
    // agregar() suma una fila al final de lo que ya habia.
    bool escribirEncabezado = !std::filesystem::exists(rutaArchivo_);

    std::ofstream archivo(rutaArchivo_, std::ios::app);
    if (!archivo.is_open()) {
        throw std::runtime_error("No se pudo abrir '" + rutaArchivo_ + "' para guardar el corte de caja.");
    }

    if (escribirEncabezado) {
        archivo << "numeroCorte,fechaHoraCierre,folioInicial,folioFinal,numeroTransacciones,"
                   "totalVendido,totalEfectivo,totalTarjetaCredito,totalTarjetaDebito\n";
    }

    archivo << corte.getNumeroCorte() << ',' << corte.fechaHoraCierreComoTexto() << ',' << corte.getFolioInicial()
            << ',' << corte.getFolioFinal() << ',' << corte.getNumeroTransacciones() << ','
            << corte.getTotalVendido() << ',' << corte.getTotalEfectivo() << ',' << corte.getTotalTarjetaCredito()
            << ',' << corte.getTotalTarjetaDebito() << '\n';
}

std::vector<CorteCaja> RepositorioCortesCsv::cargarTodos() {
    std::vector<CorteCaja> resultado;

    std::ifstream archivo(rutaArchivo_);
    if (!archivo.is_open()) {
        return resultado; // primera ejecucion: todavia no hay cortes guardados.
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
            continue; // fila corrupta: se ignora, igual que en los demas repositorios.
        }

        try {
            resultado.emplace_back(std::stoi(campos[0]), parsearFecha(campos[1]), std::stoi(campos[2]),
                                    std::stoi(campos[3]), std::stoi(campos[4]), std::stod(campos[5]),
                                    std::stod(campos[6]), std::stod(campos[7]), std::stod(campos[8]));
        } catch (const std::exception&) {
            continue; // numero/fecha invalidos: se descarta esa fila.
        }
    }

    return resultado;
}
