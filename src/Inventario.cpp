#include "Inventario.h"
#include "Excepciones.h"

#include <algorithm>

void Inventario::agregarProducto(const Producto& producto) {
    // map::count devuelve 0 o 1 (no puede haber claves repetidas en un map).
    if (productos_.count(producto.getCodigo()) > 0) {
        throw CodigoDuplicado(producto.getCodigo());
    }
    // operator[] crearia un Producto "vacio" si la clave no existe y no hay
    // constructor por defecto, asi que usamos emplace/insert explicito.
    productos_.insert({producto.getCodigo(), producto});
}

void Inventario::editarProducto(const std::string& codigo,
                                 const std::string& nuevoNombre,
                                 double nuevoPrecio,
                                 const std::string& nuevaCategoria,
                                 int nuevoStockMinimo) {
    // find() devuelve un iterador; si es end() la clave no existe. Es el
    // patron estandar de la STL para "buscar sin lanzar excepcion".
    auto it = productos_.find(codigo);
    if (it == productos_.end()) {
        throw ProductoNoEncontrado(codigo);
    }
    // it->second es el Producto asociado a esa clave dentro del map.
    Producto& producto = it->second;
    producto.setNombre(nuevoNombre);
    producto.setPrecio(nuevoPrecio);
    producto.setCategoria(nuevaCategoria);
    producto.setStockMinimo(nuevoStockMinimo);
}

void Inventario::eliminarProducto(const std::string& codigo) {
    // erase devuelve cuantos elementos elimino (0 o 1 en un map normal).
    std::size_t eliminados = productos_.erase(codigo);
    if (eliminados == 0) {
        throw ProductoNoEncontrado(codigo);
    }
}

const Producto& Inventario::buscarPorCodigo(const std::string& codigo) const {
    auto it = productos_.find(codigo);
    if (it == productos_.end()) {
        throw ProductoNoEncontrado(codigo);
    }
    return it->second;
}

std::vector<Producto> Inventario::buscarPorNombre(const std::string& textoParcial) const {
    std::vector<Producto> resultado;
    // Comparacion simple, insensible a mayusculas, por coincidencia parcial.
    std::string textoBuscado = textoParcial;
    std::transform(textoBuscado.begin(), textoBuscado.end(), textoBuscado.begin(), ::tolower);

    for (const auto& [codigo, producto] : productos_) {
        // Estructured bindings (C++17): [codigo, producto] desempaqueta el
        // par clave/valor del map sin usar .first/.second. No existe en C++
        // anteriores a 2017.
        std::string nombreMinusculas = producto.getNombre();
        std::transform(nombreMinusculas.begin(), nombreMinusculas.end(),
                        nombreMinusculas.begin(), ::tolower);
        if (nombreMinusculas.find(textoBuscado) != std::string::npos) {
            resultado.push_back(producto);
        }
    }
    return resultado;
}

bool Inventario::existeCodigo(const std::string& codigo) const {
    return productos_.count(codigo) > 0;
}

std::vector<Producto> Inventario::listarTodos() const {
    std::vector<Producto> resultado;
    resultado.reserve(productos_.size()); // evita reasignaciones del vector.
    for (const auto& [codigo, producto] : productos_) {
        resultado.push_back(producto);
    }
    return resultado;
}

std::vector<Producto> Inventario::productosConStockBajo() const {
    std::vector<Producto> resultado;
    for (const auto& [codigo, producto] : productos_) {
        if (producto.estaBajoStockMinimo()) {
            resultado.push_back(producto);
        }
    }
    return resultado;
}

void Inventario::descontarStock(const std::string& codigo, int cantidad) {
    auto it = productos_.find(codigo);
    if (it == productos_.end()) {
        throw ProductoNoEncontrado(codigo);
    }
    it->second.reducirStock(cantidad); // puede lanzar StockInsuficiente.
}

std::size_t Inventario::cantidadProductos() const {
    return productos_.size();
}
