#include "GeneradorCorte.h"
#include "CorteCaja.h"
#include "InformacionNegocio.h"

#include <iomanip>
#include <sstream>

namespace {

constexpr int ANCHO_CORTE = 40;

// Mismos helpers que GeneradorTicket.cpp (centrar/linea) -- se duplican
// aqui en vez de compartirse porque son dos lineas cada uno; no vale la
// pena crear un header solo para esto.
std::string centrar(const std::string& texto, int ancho) {
    if (static_cast<int>(texto.size()) >= ancho) {
        return texto;
    }
    int espacioTotal = ancho - static_cast<int>(texto.size());
    int izquierda = espacioTotal / 2;
    return std::string(static_cast<std::size_t>(izquierda), ' ') + texto;
}

std::string linea(char c) {
    return std::string(ANCHO_CORTE, c);
}

// Alinea una etiqueta a la izquierda y un monto en pesos a la derecha
// dentro del ancho del corte, ej. "Efectivo:" ... "$450.00" -- mismo
// truco que una tabla de consola pero en un ancho fijo de 40 caracteres.
std::string filaMonto(const std::string& etiqueta, double monto) {
    std::ostringstream montoTexto;
    montoTexto << '$' << std::fixed << std::setprecision(2) << monto;
    std::string montoStr = montoTexto.str();

    int espacios = ANCHO_CORTE - static_cast<int>(etiqueta.size()) - static_cast<int>(montoStr.size());
    if (espacios < 1) {
        espacios = 1;
    }
    return etiqueta + std::string(static_cast<std::size_t>(espacios), ' ') + montoStr;
}

} // namespace

std::string generarTextoCorte(const CorteCaja& corte, const InformacionNegocio& info) {
    std::ostringstream salida;
    salida << std::fixed << std::setprecision(2);

    salida << linea('=') << "\n";
    // Mismos campos opcionales que el ticket (ver GeneradorTicket.cpp):
    // se omiten si el negocio todavia no los ha capturado.
    if (!info.getNombre().empty()) {
        salida << centrar(info.getNombre(), ANCHO_CORTE) << "\n";
    }
    if (!info.getDireccion().empty()) {
        salida << centrar(info.getDireccion(), ANCHO_CORTE) << "\n";
    }
    if (!info.getTelefono().empty()) {
        salida << centrar("Tel: " + info.getTelefono(), ANCHO_CORTE) << "\n";
    }
    if (!info.getRfc().empty()) {
        salida << centrar("RFC: " + info.getRfc(), ANCHO_CORTE) << "\n";
    }
    salida << linea('=') << "\n";
    salida << centrar("CORTE DE CAJA #" + std::to_string(corte.getNumeroCorte()), ANCHO_CORTE) << "\n";
    salida << linea('=') << "\n";

    salida << "Cierre: " << corte.fechaHoraCierreComoTexto() << "\n";
    if (corte.getNumeroTransacciones() > 0) {
        salida << "Folios: " << corte.getFolioInicial() << " a " << corte.getFolioFinal() << "\n";
    }
    salida << "Transacciones: " << corte.getNumeroTransacciones() << "\n";
    salida << linea('-') << "\n";

    salida << filaMonto("Efectivo:", corte.getTotalEfectivo()) << "\n";
    salida << filaMonto("Tarjeta de credito:", corte.getTotalTarjetaCredito()) << "\n";
    salida << filaMonto("Tarjeta de debito:", corte.getTotalTarjetaDebito()) << "\n";
    salida << linea('-') << "\n";

    std::ostringstream totalTexto;
    totalTexto << std::fixed << std::setprecision(2) << corte.getTotalVendido();
    salida << centrar("TOTAL: $" + totalTexto.str(), ANCHO_CORTE) << "\n";
    salida << linea('=') << "\n";

    return salida.str();
}
