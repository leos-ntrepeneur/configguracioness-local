// Inventario.h
//
// Guarda todos los Productos y ofrece las operaciones de alta/edicion/baja/
// busqueda. Es la unica clase que puede modificar la coleccion directamente;
// el resto del programa pasa siempre por aqui (encapsulamiento).
//
// POR QUE std::map Y NO std::vector O UN ARREGLO:
// - std::map<std::string, Producto> mantiene una entrada por codigo, en
//   orden alfabetico de clave, y nos da:
//     * Unicidad garantizada (no puede haber dos productos con el mismo
//       codigo, porque son la clave del mapa).
//     * Busqueda por codigo en O(log n) con find(), en vez de recorrer todo
//       un arreglo a mano como harias en C.
// - Es una plantilla (template), el equivalente de C++ a los genericos de
//   C#/Java: std::map<K, V> funciona para cualquier tipo de clave/valor.

#ifndef INVENTARIO_H
#define INVENTARIO_H

#include <map>
#include <string>
#include <vector>

#include "Producto.h"

class Inventario {
public:
    // Da de alta un producto nuevo. Lanza CodigoDuplicado si el codigo ya
    // existe. Recibe el Producto ya construido (y validado) por el llamador.
    void agregarProducto(const Producto& producto);

    // Actualiza nombre/precio/categoria/stock minimo de un producto existente.
    // El stock NO se edita aqui a mano; se ajusta via ventas o metodos
    // dedicados, para evitar inconsistencias con el historial de ventas.
    void editarProducto(const std::string& codigo,
                         const std::string& nuevoNombre,
                         double nuevoPrecio,
                         const std::string& nuevaCategoria,
                         int nuevoStockMinimo);

    // Baja definitiva de un producto. Lanza ProductoNoEncontrado si no existe.
    void eliminarProducto(const std::string& codigo);

    // Busquedas. Devuelven referencia const: quien llama puede leer pero no
    // modificar el producto directamente (debe usar editarProducto).
    const Producto& buscarPorCodigo(const std::string& codigo) const;
    std::vector<Producto> buscarPorNombre(const std::string& textoParcial) const;

    bool existeCodigo(const std::string& codigo) const;

    // Devuelve todos los productos ordenados por codigo (orden natural del map).
    std::vector<Producto> listarTodos() const;

    // Requisito 4: productos cuyo stock esta por debajo de su stockMinimo.
    std::vector<Producto> productosConStockBajo() const;

    // Usado por GestorVentas al concretar una venta: descuenta stock real.
    // No se expone como "editar stock libremente" para dejar claro que el
    // camino normal de salida de stock es una venta.
    void descontarStock(const std::string& codigo, int cantidad);

    std::size_t cantidadProductos() const;

    // Reemplaza TODO el inventario con lo leido de persistencia al iniciar
    // el programa (Requisito 6). A diferencia de agregarProducto(), no
    // lanza CodigoDuplicado: si el archivo tuviera un codigo repetido (no
    // deberia, pero un archivo se puede editar a mano), el ultimo que
    // aparece simplemente reemplaza al anterior en vez de tronar el
    // arranque del programa.
    void cargarProductos(std::vector<Producto> productos);

private:
    // La clave es el codigo del producto: evita buscarlo manualmente y hace
    // que "existe codigo duplicado" sea una operacion trivial (map.count).
    std::map<std::string, Producto> productos_;
};

#endif // INVENTARIO_H
