#include "Iva.h"

namespace iva {

double calcularSubtotal(double totalConIva) {
    return totalConIva / (1.0 + TASA);
}

double calcularMontoIva(double totalConIva) {
    return totalConIva - calcularSubtotal(totalConIva);
}

} // namespace iva
