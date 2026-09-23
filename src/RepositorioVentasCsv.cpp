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

    archivo << "ventaId,fecha,metodoPago,codigoProducto,nombreProducto,cantidad,precioUnitario\n";
    for (const Venta& venta : ventas) {
        // El folio YA es parte de la propia Venta (venta.getNumeroTransaccion(),
        // asignado por GestorVentas al registrarla) -- a diferencia de la
        // version anterior, aqui no se inventa un contador nuevo cada vez
        // que se guarda, asi el folio de un ticket impreso sigue
        // coincidiendo con lo que hay en el archivo despues.
        for (const DetalleVenta& detalle : venta.getDetalles()) {
            archivo << venta.getNumeroTransaccion() << ',' << venta.fechaComoTexto() << ','
                    << metodoPagoATexto(venta.getMetodoPago()) << ','
                    << detalle.getCodigoProducto() << ',' << detalle.getNombreProducto() << ','
                    << detalle.getCantidad() << ',' << detalle.getPrecioUnitario() << '\n';
        }
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
    std::string metodoPagoActual;
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
            int numeroTransaccion = std::stoi(idActual);
            MetodoPago metodoPago = textoAMetodoPago(metodoPagoActual);
            resultado.emplace_back(std::move(detallesActuales), numeroTransaccion, metodoPago,
                                    parsearFecha(fechaActual));
        } catch (const std::exception&) {
            // Fecha/folio/metodo de pago corrupto o detalles invalidos: se
            // descarta esa venta completa en vez de tronar la carga de
            // todo el historial.
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
        if (campos.size() != 7) {
            continue;
        }
        const std::string& ventaId = campos[0];

        if (ventaId != idActual) {
            cerrarVentaActual();
            idActual = ventaId;
            fechaActual = campos[1];
            metodoPagoActual = campos[2];
        }

        try {
            detallesActuales.emplace_back(campos[3], campos[4], std::stoi(campos[5]), std::stod(campos[6]));
        } catch (const std::exception&) {
            continue; // fila con numero invalido: se ignora esa linea.
        }
    }
    cerrarVentaActual(); // no olvidar la ultima venta del archivo.

    return resultado;
}
