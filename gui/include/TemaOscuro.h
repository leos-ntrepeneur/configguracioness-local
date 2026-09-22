// TemaOscuro.h
//
// Aplica el tema oscuro a toda la aplicacion. Se separa en su propia
// funcion (en vez de dejarlo suelto en main_gui.cpp) porque hacen falta
// DOS mecanismos distintos de Qt trabajando juntos, y vale la pena que
// quede documentado en un solo lugar por que se necesitan ambos:
//
// 1. Una hoja de estilo (QSS, ver gui/resources/style.qss): controla el
//    aspecto de casi todo -- fondos, bordes, botones, tablas.
// 2. Una QPalette oscura: varios elementos que Qt dibuja "a mano" con su
//    motor de estilos (las flechitas de un QSpinBox, el estado atenuado
//    de un campo deshabilitado, etc.) no leen la propiedad `color` de la
//    hoja de estilo -- leen roles de QPalette (ButtonText, Text...). Sin
//    ajustar tambien la paleta, esos detalles se quedan con los colores
//    claros por defecto y se vuelven ilegibles sobre un fondo oscuro.

#ifndef TEMA_OSCURO_H
#define TEMA_OSCURO_H

class QApplication;

void aplicarTemaOscuro(QApplication& app);

#endif // TEMA_OSCURO_H
