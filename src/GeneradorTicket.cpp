#include "GeneradorTicket.h"
#include "InformacionNegocio.h"
#include "Iva.h"
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

// Alinea una etiqueta a la izquierda y un monto en pesos a la derecha,
// ej. "Subtotal:" ... "$430.00" -- mismo truco que GeneradorCorte.cpp
// (se duplica aqui, no vale la pena compartir dos lineas via un header).
std::string filaMonto(const std::string& etiqueta, double monto) {
    std::ostringstream montoTexto;
    montoTexto << '$' << std::fixed << std::setprecision(2) << monto;
    std::string montoStr = montoTexto.str();

    int espacios = ANCHO_TICKET - static_cast<int>(etiqueta.size()) - static_cast<int>(montoStr.size());
    if (espacios < 1) {
        espacios = 1;
    }
    return etiqueta + std::string(static_cast<std::size_t>(espacios), ' ') + montoStr;
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
    // Cliente solo aparece en ventas al fiado (ver Venta::esFiado) -- el
    // nombre es un snapshot guardado en la propia Venta, no una consulta
    // en vivo a GestorClientes (ver el comentario grande en Venta.h).
    if (venta.esFiado()) {
        salida << "Cliente: " << venta.getNombreCliente() << "\n";
    }
    salida << linea('-') << "\n";

    for (const DetalleVenta& detalle : venta.getDetalles()) {
        salida << detalle.getNombreProducto() << "\n";
        salida << "  " << detalle.getCantidad() << " x $" << detalle.getPrecioUnitario()
               << "  =  $" << detalle.getSubtotal() << "\n";
    }
    salida << linea('-') << "\n";

    // Los precios de los productos YA TRAEN el IVA incluido (asi se
    // capturan, ver Producto/ProductoDialog) -- el total de la venta no
    // cambia por este desglose, solo se muestra de cuanto de ese total ya
    // pagado es la parte del impuesto, como en cualquier ticket mexicano.
    double subtotal = iva::calcularSubtotal(venta.getTotal());
    double montoIva = iva::calcularMontoIva(venta.getTotal());
    salida << filaMonto("Subtotal:", subtotal) << "\n";
    salida << filaMonto("IVA (16%):", montoIva) << "\n";
    salida << linea('-') << "\n";

    std::ostringstream totalTexto;
    totalTexto << std::fixed << std::setprecision(2) << venta.getTotal();
    salida << centrar("TOTAL: $" + totalTexto.str(), ANCHO_TICKET) << "\n";
    salida << linea('=') << "\n";
    // Un ticket al fiado NO es un comprobante de pago -- decir "gracias
    // por su compra" ahi confundiria al cliente sobre si ya liquido o no.
    if (venta.esFiado()) {
        salida << centrar("** PENDIENTE DE PAGO (FIADO) **", ANCHO_TICKET) << "\n";
    } else {
        salida << centrar("Gracias por su compra", ANCHO_TICKET) << "\n";
    }
    salida << linea('=') << "\n";

    return salida.str();
}
