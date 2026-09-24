// Iva.h
//
// En Mexico, a diferencia de EE.UU. (donde el impuesto se agrega DESPUES
// del precio mostrado), el precio que el negocio le pone a un producto ya
// TRAE el IVA incluido -- lo que ve el cliente en el anaquel es lo que
// paga, punto. Por eso `Producto::getPrecio()` nunca cambia: sigue siendo
// el precio final tal cual el usuario lo captura. Este archivo solo ofrece
// funciones para, a partir de ese precio final, CALCULAR cuanto de ese
// monto es el subtotal (sin impuesto) y cuanto es el IVA -- util para
// mostrarlo como referencia (ProductoDialog, Menu) y para desglosarlo en
// el ticket/corte de caja (Subtotal + IVA = Total), sin guardar un campo
// nuevo ni tocar la unica fuente de verdad del precio.

#ifndef IVA_H
#define IVA_H

namespace iva {

// Tasa general de IVA en Mexico (16%). Es un solo lugar: si algun dia
// cambiara, o se necesitara la tasa fronteriza reducida, este es el punto
// unico a modificar.
constexpr double TASA = 0.16;

// A partir de un monto que YA INCLUYE el IVA, regresa la parte que
// corresponde al subtotal (sin impuesto): subtotal = total / (1 + TASA).
double calcularSubtotal(double totalConIva);

// A partir de un monto que YA INCLUYE el IVA, regresa solo el monto del
// impuesto: monto = total - subtotal. Se calcula asi (en vez de
// total * TASA directo) para que subtotal + iva sume EXACTAMENTE el total
// original, sin errores de redondeo por separado.
double calcularMontoIva(double totalConIva);

} // namespace iva

#endif // IVA_H
