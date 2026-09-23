// InformacionNegocio.h
//
// Los datos del negocio que aparecen impresos en el encabezado de cada
// ticket: nombre, direccion, telefono y RFC. Es UN SOLO registro (a
// diferencia de Producto, del que hay muchos) -- por eso no vive dentro de
// un Inventario ni tiene un "codigo" que lo identifique, simplemente hay
// una instancia que se edita y se guarda.
//
// IMPORTANTE sobre el RFC: este campo es solo texto libre que el dueño del
// negocio captura a mano para que aparezca impreso en el ticket (como lo
// haria cualquier ticket de tienda). NO se valida contra el formato real
// del RFC del SAT, ni el programa genera facturas electronicas (CFDI) ni
// se conecta a ningun servicio de facturacion -- eso es un sistema aparte,
// mucho mas grande, que queda fuera del alcance de este proyecto.

#ifndef INFORMACION_NEGOCIO_H
#define INFORMACION_NEGOCIO_H

#include <string>

class InformacionNegocio {
public:
    static constexpr int LONGITUD_MAXIMA = 150;

    // A diferencia de Producto, TODOS los campos pueden empezar vacios
    // (un negocio recien instalado el programa todavia no ha capturado
    // nada) -- por eso hay un constructor sin argumentos ademas del que
    // recibe los 4 valores.
    InformacionNegocio() = default;
    InformacionNegocio(std::string nombre, std::string direccion, std::string telefono, std::string rfc);

    const std::string& getNombre() const;
    const std::string& getDireccion() const;
    const std::string& getTelefono() const;
    const std::string& getRfc() const;

    void setNombre(std::string nombre);
    void setDireccion(std::string direccion);
    void setTelefono(std::string telefono);
    void setRfc(std::string rfc);

private:
    std::string nombre_;
    std::string direccion_;
    std::string telefono_;
    std::string rfc_;
};

#endif // INFORMACION_NEGOCIO_H
