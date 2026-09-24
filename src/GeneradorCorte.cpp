#include "GeneradorCorte.h"
#include "CorteCaja.h"
#include "InformacionNegocio.h"
#include "Iva.h"

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
    // Fiado se muestra aparte y "(pendiente)" a proposito: a diferencia de
    // los otros tres, ese dinero TODAVIA NO entro a la caja -- confundirlo
    // con efectivo real haria que el corte cuadrara mal contra lo que
    // fisicamente hay en la caja.
    salida << filaMonto("Fiado (pendiente):", corte.getTotalFiado()) << "\n";
    salida << linea('-') << "\n";

    double cobradoHoy = corte.getTotalVendido() - corte.getTotalFiado();
    salida << filaMonto("Cobrado hoy:", cobradoHoy) << "\n";
    salida << linea('-') << "\n";

    // Mismo desglose que en el ticket individual (ver GeneradorTicket.cpp):
    // los precios ya incluyen IVA, esto solo muestra cuanto del total del
    // dia corresponde a impuesto -- util para la declaracion del negocio.
    // Se calcula sobre el total VENDIDO (incluye fiado): el IVA se causa
    // al vender, no al cobrar.
    double subtotal = iva::calcularSubtotal(corte.getTotalVendido());
    double montoIva = iva::calcularMontoIva(corte.getTotalVendido());
    salida << filaMonto("Subtotal:", subtotal) << "\n";
    salida << filaMonto("IVA (16%):", montoIva) << "\n";
    salida << linea('-') << "\n";

    std::ostringstream totalTexto;
    totalTexto << std::fixed << std::setprecision(2) << corte.getTotalVendido();
    salida << centrar("TOTAL VENDIDO: $" + totalTexto.str(), ANCHO_CORTE) << "\n";
    salida << linea('=') << "\n";

    return salida.str();
}
