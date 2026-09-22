// IRepositorioProductos.h
//
// Interfaz para guardar/cargar la lista completa de productos. Hoy la
// implementa RepositorioProductosCsv; el dia de manana, para migrar a
// SQLite, bastaria con escribir una RepositorioProductosSqlite que
// implemente esta misma interfaz -- Menu y main no se enteran del cambio
// porque solo dependen de IRepositorioProductos, nunca de la clase
// concreta.
//
// EN C++, una "interfaz" no es una palabra reservada (como `interface` en
// C# o una clase abstracta especial en Java): se simula con una clase que
// solo tiene metodos virtuales PUROS (el `= 0` al final). "Puro" quiere
// decir que la clase no da implementacion, solo el contrato; no se puede
// crear un IRepositorioProductos directamente, solo clases que hereden de
// el e implementen los metodos.
//
// El destructor virtual es obligatorio en cualquier clase pensada para
// usarse de forma polimorfica (a traves de un puntero/referencia a la
// clase base): sin `virtual` aqui, borrar un objeto derivado a traves de
// un `IRepositorioProductos*` (por ejemplo, dentro de un
// std::unique_ptr<IRepositorioProductos>) solo llamaria al destructor de
// la base y dejaria sin liberar los recursos propios de la clase derivada
// -- un error clasico de C++ que no existe en lenguajes con recolector de
// basura.

#ifndef I_REPOSITORIO_PRODUCTOS_H
#define I_REPOSITORIO_PRODUCTOS_H

#include <vector>

#include "Producto.h"

class IRepositorioProductos {
public:
    virtual ~IRepositorioProductos() = default;

    virtual void guardarTodos(const std::vector<Producto>& productos) = 0;
    virtual std::vector<Producto> cargarTodos() = 0;
};

#endif // I_REPOSITORIO_PRODUCTOS_H
