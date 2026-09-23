// TicketDialog.h
//
// Ventana emergente que muestra el ticket de una venta recien confirmada
// (texto generado por GeneradorTicket.h, el mismo que usa la consola).
// Solo lectura: no hay nada que editar aqui, unicamente ver/copiar/leer.

#ifndef TICKET_DIALOG_H
#define TICKET_DIALOG_H

#include <QDialog>
#include <string>

class QTextEdit;

class TicketDialog : public QDialog {
    Q_OBJECT

public:
    // `textoTicket` ya viene armado (ver generarTextoTicket); este dialogo
    // solo se encarga de mostrarlo con una fuente monoespaciada, para que
    // el alineado de columnas del ticket (hecho a punta de espacios) se
    // vea igual que en la consola.
    TicketDialog(const std::string& textoTicket, QWidget* padre = nullptr);
};

#endif // TICKET_DIALOG_H
