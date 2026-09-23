// GeneradorTicket.h
//
// Arma el texto plano de un ticket de venta a partir de una Venta ya
// registrada y los datos del negocio. Es una funcion libre (no una clase)
// porque no necesita guardar ningun estado propio -- solo toma datos y
// devuelve texto, lo mismo que ya se explico para CsvUtil::dividirLinea.
// Al vivir en las fuentes de NEGOCIO (CORE_SOURCES en CMakeLists.txt), la
// consola y la GUI generan el MISMO texto de ticket sin duplicar ni una
// linea de formato -- la consola lo imprime con std::cout, la GUI lo
// muestra en un QTextEdit monoespaciado (ver gui/include/TicketDialog.h).

#ifndef GENERADOR_TICKET_H
#define GENERADOR_TICKET_H

#include <string>

class Venta;
class InformacionNegocio;

std::string generarTextoTicket(const Venta& venta, const InformacionNegocio& informacionNegocio);

#endif // GENERADOR_TICKET_H
