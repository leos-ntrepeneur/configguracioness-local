#include "TicketDialog.h"

#include <QDialogButtonBox>
#include <QFont>
#include <QFontMetrics>
#include <QTextEdit>
#include <QVBoxLayout>

namespace {
// GeneradorTicket.cpp arma cada linea a exactamente este ancho (ver
// ANCHO_TICKET en GeneradorTicket.cpp) -- se repite aqui solo para
// calcular cuanto espacio necesita el QTextEdit, no para formatear nada.
constexpr int ANCHO_TICKET_CARACTERES = 40;
} // namespace

TicketDialog::TicketDialog(const std::string& textoTicket, QWidget* padre) : QDialog(padre) {
    setWindowTitle("Ticket de venta");

    auto* texto = new QTextEdit(this);
    texto->setReadOnly(true);
    // Sin ajuste de linea: si el dialogo se hiciera mas angosto igual no
    // queremos que una linea de 40 caracteres se parta a la mitad (se veria
    // "torcida", que es justo lo que NoWrap evita) -- aparece una barra de
    // desplazamiento horizontal en vez de eso.
    texto->setLineWrapMode(QTextEdit::NoWrap);
    texto->setPlainText(QString::fromStdString(textoTicket));
    // Una fuente monoespaciada es indispensable aqui: GeneradorTicket.cpp
    // alinea columnas y centra texto contando caracteres uno a uno (ver
    // `centrar()`), algo que solo se ve derecho si cada caracter ocupa el
    // mismo ancho -- con una fuente proporcional (como Manrope, la del
    // resto de la GUI) el "ticket" saldria torcido.
    QFont fuenteMonoespaciada("Courier New");
    fuenteMonoespaciada.setStyleHint(QFont::Monospace);
    fuenteMonoespaciada.setPointSize(11);
    texto->setFont(fuenteMonoespaciada);

    auto* botones = new QDialogButtonBox(QDialogButtonBox::Close, this);
    connect(botones, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(botones, &QDialogButtonBox::accepted, this, &QDialog::accept);

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(texto);
    layout->addWidget(botones);

    // El ancho se calcula a partir de la fuente REAL (QFontMetrics), no de
    // un numero de pixeles inventado a ojo: asi el dialogo siempre le
    // alcanza exactamente para las 40 columnas del ticket sin que la barra
    // de desplazamiento horizontal aparezca de entrada, sea cual sea la
    // fuente monoespaciada que termine resolviendo el sistema.
    QFontMetrics metricas(fuenteMonoespaciada);
    int anchoTexto = metricas.horizontalAdvance(QString(ANCHO_TICKET_CARACTERES, 'X'));
    int margenes = texto->frameWidth() * 2 + texto->document()->documentMargin() * 2 + 40;
    resize(anchoTexto + margenes, 560);
}
