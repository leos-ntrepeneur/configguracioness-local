// TicketDialog.h
//
// Ventana emergente de solo lectura que muestra un bloque de texto de
// ancho fijo con fuente monoespaciada -- el mismo formato que usan TANTO
// el ticket de una venta (GeneradorTicket.h) COMO el corte de caja
// (GeneradorCorte.h), asi que un solo dialogo sirve para los dos en vez
// de duplicar la logica de "mostrar texto preformateado" dos veces.

#ifndef TICKET_DIALOG_H
#define TICKET_DIALOG_H

#include <QDialog>
#include <QString>
#include <string>

class QTextEdit;

class TicketDialog : public QDialog {
    Q_OBJECT

public:
    // `texto` ya viene armado (generarTextoTicket o generarTextoCorte);
    // este dialogo solo se encarga de mostrarlo con una fuente
    // monoespaciada, para que el alineado de columnas (hecho a punta de
    // espacios) se vea igual que en la consola. `titulo` es el titulo de
    // la ventana -- por defecto "Ticket de venta" para no tener que tocar
    // los llamados existentes, pero PestanaCortes lo cambia a "Corte de
    // caja".
    TicketDialog(const std::string& texto, QWidget* padre = nullptr, QString titulo = "Ticket de venta");
};

#endif // TICKET_DIALOG_H
