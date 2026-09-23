// GeneradorCorte.h
//
// Arma el texto plano de un corte de caja, igual que GeneradorTicket.h
// arma el de un ticket de venta -- misma idea (funcion libre, vive en el
// codigo de NEGOCIO para que consola y GUI impriman exactamente el mismo
// texto) aplicada a un resumen de cierre en vez de a una venta individual.

#ifndef GENERADOR_CORTE_H
#define GENERADOR_CORTE_H

#include <string>

class CorteCaja;
class InformacionNegocio;

std::string generarTextoCorte(const CorteCaja& corte, const InformacionNegocio& informacionNegocio);

#endif // GENERADOR_CORTE_H
