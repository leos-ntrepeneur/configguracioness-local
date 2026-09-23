#include "GeneradorTicket.h"
#include "InformacionNegocio.h"
#include "Venta.h"

#include <iomanip>
#include <sstream>

namespace {

constexpr int ANCHO_TICKET = 40;

// Centra `texto` dentro de un ancho fijo, rellenando con espacios a los
// lados -- el efecto visual tipico del encabezado de un ticket impreso.
// Si el texto es mas ancho que `ancho`, se devuelve tal cual (mejor que
// cortarlo a la mitad).
std::string centrar(const std::string& texto, int ancho) {
    if (static_cast<int>(texto.size()) >= ancho) {
        return texto;
    }
    int espacioTotal = ancho - static_cast<int>(texto.size());
    int izquierda = espacioTotal / 2;
    return std::string(static_cast<std::size_t>(izquierda), ' ') + texto;
}

std::string linea(char c) {
    return std::string(ANCHO_TICKET, c);
}

} // namespace

std::string generarTextoTicket(const Venta& venta, const InformacionNegocio& info) {
    std::ostringstream salida;
    salida << std::fixed << std::setprecision(2);

    salida << linea('=') << "\n";
    // Los campos de InformacionNegocio pueden estar vacios (negocio recien
    // instalado que aun no captura sus datos, ver PestanaInformacionNegocio
    // / Menu::alConfigurarInformacionNegocio) -- se omiten en vez de
    // imprimir una linea en blanco fea.
    if (!info.getNombre().empty()) {
        salida << centrar(info.getNombre(), ANCHO_TICKET) << "\n";
    }
    if (!info.getDireccion().empty()) {
        salida << centrar(info.getDireccion(), ANCHO_TICKET) << "\n";
    }
    if (!info.getTelefono().empty()) {
        salida << centrar("Tel: " + info.getTelefono(), ANCHO_TICKET) << "\n";
    }
    if (!info.getRfc().empty()) {
        salida << centrar("RFC: " + info.getRfc(), ANCHO_TICKET) << "\n";
    }
    salida << linea('=') << "\n";

    salida << "Folio:  " << venta.getNumeroTransaccion() << "\n";
    salida << "Fecha:  " << venta.fechaComoTexto() << "\n";
    salida << "Pago:   " << metodoPagoATexto(venta.getMetodoPago()) << "\n";
    salida << linea('-') << "\n";

    for (const DetalleVenta& detalle : venta.getDetalles()) {
        salida << detalle.getNombreProducto() << "\n";
        salida << "  " << detalle.getCantidad() << " x $" << detalle.getPrecioUnitario()
               << "  =  $" << detalle.getSubtotal() << "\n";
    }
    salida << linea('-') << "\n";

    std::ostringstream totalTexto;
    totalTexto << std::fixed << std::setprecision(2) << venta.getTotal();
    salida << centrar("TOTAL: $" + totalTexto.str(), ANCHO_TICKET) << "\n";
    salida << linea('=') << "\n";
    salida << centrar("Gracias por su compra", ANCHO_TICKET) << "\n";
    salida << linea('=') << "\n";

    return salida.str();
}
