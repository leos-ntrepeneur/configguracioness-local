// Producto.h
//
// Representa un articulo del inventario. Es una clase de "valor": se copia,
// se compara por su codigo y no participa en jerarquias de herencia, asi que
// no necesita destructor virtual ni punteros.
//
// DIFERENCIAS CLAVE CON C#/JAVA:
// - Aqui separamos declaracion (.h) de implementacion (.cpp). El .h es el
//   "contrato publico" que otras clases incluyen con #include; el .cpp trae
//   el cuerpo de los metodos. Esto no existe en C#/Java, donde todo vive en
//   un solo archivo por clase.
// - Los miembros son privados por defecto en una `class` (en un `struct` son
//   publicos por defecto). Es la unica diferencia real entre `struct` y
//   `class` en C++; aqui usamos `class` porque vamos a exponer invariantes
//   (precio/stock no negativos) a traves de metodos, no campos sueltos.

#ifndef PRODUCTO_H
#define PRODUCTO_H

#include <string>

class Producto {
public:
    // Constructor: valida los datos de entrada y lanza EntradaInvalida si
    // algo esta mal (precio negativo, stock negativo, codigo/nombre vacios).
    Producto(std::string codigo,
             std::string nombre,
             double precio,
             int stock,
             std::string categoria = "",
             int stockMinimo = 0);

    // Getters marcados `const`: prometen al compilador que no modifican el
    // objeto. Permiten llamarlos sobre referencias `const Producto&`, que es
    // como vamos a pasar productos casi siempre (evita copias innecesarias).
    const std::string& getCodigo() const;
    const std::string& getNombre() const;
    double getPrecio() const;
    int getStock() const;
    const std::string& getCategoria() const;
    int getStockMinimo() const;

    // Setters: cada uno valida su propio dato antes de asignarlo.
    void setNombre(std::string nombre);
    void setPrecio(double precio);
    void setCategoria(std::string categoria);
    void setStockMinimo(int stockMinimo);

    // Operaciones de stock usadas por Inventario/GestorVentas. Devuelven
    // referencia a *this? No: aqui no hace falta encadenado, asi que son void.
    void aumentarStock(int cantidad);
    void reducirStock(int cantidad); // lanza StockInsuficiente si no alcanza.

    bool estaBajoStockMinimo() const;

private:
    std::string codigo_;      // Identificador unico, no editable tras crear.
    std::string nombre_;
    double precio_;
    int stock_;
    std::string categoria_;   // Opcional: cadena vacia == sin categoria.
    int stockMinimo_;         // Umbral para alertas de stock bajo (Requisito 4).
};

#endif // PRODUCTO_H
