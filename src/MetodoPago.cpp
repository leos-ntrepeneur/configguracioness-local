#include "MetodoPago.h"
#include "Excepciones.h"

std::string metodoPagoATexto(MetodoPago metodo) {
    // switch sin `default`: si algun dia se agrega un valor nuevo al enum
    // y se olvida su caso aqui, el compilador avisa con un warning
    // ("enumeration value not handled"). Con un `default` ese aviso se
    // pierde -- una trampa comun al usar enum class para representar un
    // conjunto cerrado de opciones.
    switch (metodo) {
        case MetodoPago::Efectivo:
            return "Efectivo";
        case MetodoPago::TarjetaCredito:
            return "Tarjeta de credito";
        case MetodoPago::TarjetaDebito:
            return "Tarjeta de debito";
    }
    return "Desconocido"; // inalcanzable si el switch cubre todos los casos.
}

MetodoPago textoAMetodoPago(const std::string& texto) {
    if (texto == "Efectivo") return MetodoPago::Efectivo;
    if (texto == "Tarjeta de credito") return MetodoPago::TarjetaCredito;
    if (texto == "Tarjeta de debito") return MetodoPago::TarjetaDebito;
    throw EntradaInvalida("Metodo de pago desconocido: " + texto);
}
