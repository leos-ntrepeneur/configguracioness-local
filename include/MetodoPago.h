// MetodoPago.h
//
// Como se cobro una venta. Es un `enum class` (a diferencia de un enum "a
// secas" de C), lo que significa que sus valores viven DENTRO del nombre
// del enum (MetodoPago::Efectivo, no Efectivo a secas) y no se convierten
// implicitamente a int -- evita el clasico bug de comparar por accidente
// un MetodoPago con un numero que en realidad era de otra cosa.

#ifndef METODO_PAGO_H
#define METODO_PAGO_H

#include <string>

// Fiado: venta a credito, cobrada mas tarde a un cliente de confianza
// (comun en abarrotes/ferreterias mexicanas) -- ver Cliente.h y
// GestorCreditos.h para el seguimiento del saldo pendiente.
enum class MetodoPago { Efectivo, TarjetaCredito, TarjetaDebito, Fiado };

// Texto legible (y a la vez seguro para CSV: sin comas) para mostrar en
// pantalla, imprimir en un ticket, o guardar en el archivo de ventas.
std::string metodoPagoATexto(MetodoPago metodo);

// Inverso: reconstruye el enum a partir del texto guardado en el CSV.
// Lanza EntradaInvalida si el texto no coincide con ninguno de los tres
// validos (por ejemplo, un archivo editado a mano con un valor inventado).
MetodoPago textoAMetodoPago(const std::string& texto);

#endif // METODO_PAGO_H
