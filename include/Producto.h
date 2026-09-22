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
    // Limites de negocio, PUBLICOS a proposito: tanto el menu de consola
    // (Menu.cpp) como los widgets de la GUI (ProductoDialog, PestanaVentas)
    // los usan para configurar sus propios controles (setRange, setMaxLength)
    // y para el limite de "cantidad" al vender. Que todos lean del MISMO
    // lugar evita que console/GUI se desincronicen con limites distintos, y
    // es ademas la defensa real contra un desbordamiento de enteros: al
    // garantizar aqui que ningun Producto puede tener mas de STOCK_MAXIMO
    // unidades, ninguna suma "cantidad en carrito + cantidad nueva" en
    // ningun otro archivo puede acercarse jamas al limite de un int (ver
    // Menu::agregarAlCarrito y PestanaVentas::alAgregarAlCarrito).
    static constexpr double PRECIO_MAXIMO = 1'000'000.0;
    static constexpr int STOCK_MAXIMO = 1'000'000;
    static constexpr int CODIGO_LONGITUD_MAXIMA = 30;
    static constexpr int NOMBRE_LONGITUD_MAXIMA = 100;
    static constexpr int CATEGORIA_LONGITUD_MAXIMA = 50;

    // Constructor: valida los datos de entrada y lanza EntradaInvalida si
    // algo esta mal (precio negativo/fuera de rango/no finito, stock
    // negativo o excesivo, codigo/nombre vacios o con caracteres invalidos).
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
