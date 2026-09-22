#include "RepositorioVentasCsv.h"
#include "CsvUtil.h"

#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace {

// Inverso de Venta::fechaComoTexto(): convierte "2026-09-22 14:35:10" de
// vuelta a un time_point. std::get_time (de <iomanip>) es el equivalente
// de lectura de std::put_time; juntos forman el par estandar de C++ para
// formatear/parsear fechas con strftime-como-formato pero con streams.
std::chrono::system_clock::time_point parsearFecha(const std::string& texto) {
    std::tm tm{};
    std::istringstream flujo(texto);
    flujo >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    if (flujo.fail()) {
        throw std::runtime_error("Fecha invalida en archivo de ventas: " + texto);
    }
    // mktime interpreta el std::tm como hora LOCAL, igual que localtime_r/
    // localtime_s en Venta::fechaComoTexto(). Si guardaramos con una
    // convencion y leyeramos con otra (ej. UTC vs local), las horas se
    // recorrerian silenciosamente.
    std::time_t comoTimeT = std::mktime(&tm);
    return std::chrono::system_clock::from_time_t(comoTimeT);
}

} // namespace

RepositorioVentasCsv::RepositorioVentasCsv(std::string rutaArchivo)
    : rutaArchivo_(std::move(rutaArchivo)) {}

void RepositorioVentasCsv::guardarTodas(const std::vector<Venta>& ventas) {
    std::filesystem::path ruta(rutaArchivo_);
    if (ruta.has_parent_path()) {
        std::filesystem::create_directories(ruta.parent_path());
    }

    std::ofstream archivo(rutaArchivo_, std::ios::trunc);
    if (!archivo.is_open()) {
        throw std::runtime_error("No se pudo abrir '" + rutaArchivo_ + "' para guardar ventas.");
    }

    archivo << "ventaId,fecha,codigoProducto,nombreProducto,cantidad,precioUnitario\n";
    int ventaId = 1;
    for (const Venta& venta : ventas) {
        for (const DetalleVenta& detalle : venta.getDetalles()) {
            archivo << ventaId << ',' << venta.fechaComoTexto() << ','
                    << detalle.getCodigoProducto() << ',' << detalle.getNombreProducto() << ','
                    << detalle.getCantidad() << ',' << detalle.getPrecioUnitario() << '\n';
        }
        ++ventaId;
    }
}

std::vector<Venta> RepositorioVentasCsv::cargarTodas() {
    std::vector<Venta> resultado;

    std::ifstream archivo(rutaArchivo_);
    if (!archivo.is_open()) {
        return resultado; // primera ejecucion: todavia no hay historial.
    }

    std::string idActual;
    std::string fechaActual;
    std::vector<DetalleVenta> detallesActuales;

    // Lambda que "cierra" el grupo de filas acumulado hasta ahora y lo
    // convierte en una Venta. Se captura todo por referencia (`[&]`) para
    // poder leer y modificar las variables de arriba sin pasarlas como
    // parametros cada vez.
    auto cerrarVentaActual = [&]() {
        if (detallesActuales.empty()) {
            return;
        }
        try {
            resultado.emplace_back(std::move(detallesActuales), parsearFecha(fechaActual));
        } catch (const std::exception&) {
            // Fecha corrupta o detalles invalidos: se descarta esa venta
            // completa en vez de tronar la carga de todo el historial.
        }
        detallesActuales.clear();
    };

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
            continue;
        }
        const std::string& ventaId = campos[0];

        if (ventaId != idActual) {
            cerrarVentaActual();
            idActual = ventaId;
            fechaActual = campos[1];
        }

        try {
            detallesActuales.emplace_back(campos[2], campos[3], std::stoi(campos[4]), std::stod(campos[5]));
        } catch (const std::exception&) {
            continue; // fila con numero invalido: se ignora esa linea.
        }
    }
    cerrarVentaActual(); // no olvidar la ultima venta del archivo.

    return resultado;
}
