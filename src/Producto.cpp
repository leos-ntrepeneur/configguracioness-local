#include "Producto.h"
#include "Excepciones.h"

#include <cmath>

namespace {

// El inventario se persiste en CSV (ver RepositorioProductosCsv), donde la
// coma es el separador de campos y el salto de linea separa filas. Un
// caracter de control (codigo ASCII menor a 0x20: salto de linea, retorno
// de carro, tabulador...) que se cuele en nombre/categoria/codigo puede
// romper la estructura del archivo al guardar -- una fila terminaria a la
// mitad y la siguiente carga leeria datos con las columnas desalineadas.
// Tambien evita "inyectar" secuencias de control en la salida de consola.
// Ademas limitamos el largo: nada en el nombre de un articulo de
// ferreteria necesita mas de 100 caracteres, y evita que un dato
// absurdamente largo (por accidente o a proposito) infle el archivo o
// rompa el ancho de columnas de la tabla en la GUI.
void validarTexto(const std::string& valor, const std::string& nombreCampo, std::size_t longitudMaxima) {
    if (valor.find(',') != std::string::npos) {
        throw EntradaInvalida(nombreCampo + " no puede contener comas.");
    }
    for (unsigned char c : valor) {
        if (c < 0x20) {
            throw EntradaInvalida(nombreCampo + " no puede contener saltos de linea ni caracteres de control.");
        }
    }
    if (valor.size() > longitudMaxima) {
        throw EntradaInvalida(nombreCampo + " no puede tener mas de " +
                               std::to_string(longitudMaxima) + " caracteres.");
    }
}

// Valida un precio: no negativo, dentro de un rango de negocio razonable,
// y "finito". IMPORTANTE: std::cin >> double (usado en la consola) acepta
// como validas las cadenas "inf", "infinity" y "nan" -- las interpreta
// como los valores especiales de punto flotante del mismo nombre, no como
// un error de formato. Sin este chequeo, un precio "nan" pasaria de largo
// las comparaciones `< 0.0` y `> PRECIO_MAXIMO` (CUALQUIER comparacion con
// NaN da falso, incluidas esas dos) y quedaria guardado como precio
// valido; despues, cualquier total que lo sumara quedaria "contaminado"
// con NaN de forma silenciosa, sin ningun error visible. std::isfinite
// devuelve false tanto para NaN como para +-infinito, y hay que revisarlo
// ANTES que los rangos normales, no en su lugar.
void validarPrecio(double precio) {
    if (!std::isfinite(precio)) {
        throw EntradaInvalida("El precio debe ser un numero valido (no puede ser infinito ni NaN).");
    }
    if (precio < 0.0) {
        throw EntradaInvalida("El precio no puede ser negativo.");
    }
    if (precio > Producto::PRECIO_MAXIMO) {
        throw EntradaInvalida("El precio no puede superar " + std::to_string(Producto::PRECIO_MAXIMO) + ".");
    }
}

void validarStock(int stock, const std::string& nombreCampo) {
    if (stock < 0) {
        throw EntradaInvalida(nombreCampo + " no puede ser negativo.");
    }
    if (stock > Producto::STOCK_MAXIMO) {
        throw EntradaInvalida(nombreCampo + " no puede superar " +
                               std::to_string(Producto::STOCK_MAXIMO) + " unidades.");
    }
}

} // namespace

// `std::move` mueve el contenido de un std::string en vez de copiarlo.
// Como los parametros del constructor se reciben por valor (std::string
// codigo, no const std::string&), ya tenemos una copia propia; moverla al
// miembro evita duplicarla una vez mas. En C# esto no aplica porque string
// es una referencia inmutable manejada por el garbage collector.
Producto::Producto(std::string codigo,
                    std::string nombre,
                    double precio,
                    int stock,
                    std::string categoria,
                    int stockMinimo)
    : codigo_(std::move(codigo)),
      nombre_(std::move(nombre)),
      precio_(precio),
      stock_(stock),
      categoria_(std::move(categoria)),
      stockMinimo_(stockMinimo) {
    if (codigo_.empty()) {
        throw EntradaInvalida("El codigo del producto no puede estar vacio.");
    }
    if (nombre_.empty()) {
        throw EntradaInvalida("El nombre del producto no puede estar vacio.");
    }
    validarTexto(codigo_, "El codigo", CODIGO_LONGITUD_MAXIMA);
    validarTexto(nombre_, "El nombre", NOMBRE_LONGITUD_MAXIMA);
    validarTexto(categoria_, "La categoria", CATEGORIA_LONGITUD_MAXIMA);
    validarPrecio(precio_);
    validarStock(stock_, "El stock");
    validarStock(stockMinimo_, "El stock minimo");
}

const std::string& Producto::getCodigo() const { return codigo_; }
const std::string& Producto::getNombre() const { return nombre_; }
double Producto::getPrecio() const { return precio_; }
int Producto::getStock() const { return stock_; }
const std::string& Producto::getCategoria() const { return categoria_; }
int Producto::getStockMinimo() const { return stockMinimo_; }

void Producto::setNombre(std::string nombre) {
    if (nombre.empty()) {
        throw EntradaInvalida("El nombre del producto no puede estar vacio.");
    }
    validarTexto(nombre, "El nombre", NOMBRE_LONGITUD_MAXIMA);
    nombre_ = std::move(nombre);
}

void Producto::setPrecio(double precio) {
    validarPrecio(precio);
    precio_ = precio;
}

void Producto::setCategoria(std::string categoria) {
    validarTexto(categoria, "La categoria", CATEGORIA_LONGITUD_MAXIMA);
    categoria_ = std::move(categoria);
}

void Producto::setStockMinimo(int stockMinimo) {
    validarStock(stockMinimo, "El stock minimo");
    stockMinimo_ = stockMinimo;
}

void Producto::setStock(int nuevoStock) {
    validarStock(nuevoStock, "El stock");
    stock_ = nuevoStock;
}

void Producto::aumentarStock(int cantidad) {
    if (cantidad < 0) {
        throw EntradaInvalida("La cantidad a aumentar no puede ser negativa.");
    }
    // Suma con verificacion de desborde: si cantidad fuera enorme (por
    // ejemplo, un valor cercano al maximo de un int), stock_ + cantidad
    // podria desbordar un int ANTES de llegar a compararse con
    // STOCK_MAXIMO -- y un desborde de enteros con signo es comportamiento
    // indefinido en C++, no simplemente "da un numero raro". Se resta en
    // vez de sumar para comparar: matematicamente equivalente a
    // "stock_ + cantidad > STOCK_MAXIMO" pero sin poder desbordar, porque
    // ambos operandos ya estan acotados a STOCK_MAXIMO por separado.
    if (cantidad > Producto::STOCK_MAXIMO - stock_) {
        throw EntradaInvalida("El stock resultante no puede superar " +
                               std::to_string(Producto::STOCK_MAXIMO) + " unidades.");
    }
    stock_ += cantidad;
}

void Producto::reducirStock(int cantidad) {
    if (cantidad < 0) {
        throw EntradaInvalida("La cantidad a reducir no puede ser negativa.");
    }
    if (cantidad > stock_) {
        throw StockInsuficiente(codigo_, stock_, cantidad);
    }
    stock_ -= cantidad;
}

bool Producto::estaBajoStockMinimo() const {
    return stock_ < stockMinimo_;
}
