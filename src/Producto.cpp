#include "Producto.h"
#include "Excepciones.h"

namespace {

// El inventario se persiste en CSV (ver RepositorioProductosCsv), donde la
// coma es el separador de campos. Si un nombre/categoria pudiera contener
// una coma, romperia el formato del archivo al guardar. En vez de
// implementar comillas/escapado (como hace un CSV "de verdad"), optamos
// por la solucion mas simple para un negocio pequeno: no permitirlas.
void validarSinComas(const std::string& valor, const std::string& nombreCampo) {
    if (valor.find(',') != std::string::npos) {
        throw EntradaInvalida(nombreCampo + " no puede contener comas.");
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
    validarSinComas(codigo_, "El codigo");
    validarSinComas(nombre_, "El nombre");
    validarSinComas(categoria_, "La categoria");
    if (precio_ < 0.0) {
        throw EntradaInvalida("El precio no puede ser negativo.");
    }
    if (stock_ < 0) {
        throw EntradaInvalida("El stock no puede ser negativo.");
    }
    if (stockMinimo_ < 0) {
        throw EntradaInvalida("El stock minimo no puede ser negativo.");
    }
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
    validarSinComas(nombre, "El nombre");
    nombre_ = std::move(nombre);
}

void Producto::setPrecio(double precio) {
    if (precio < 0.0) {
        throw EntradaInvalida("El precio no puede ser negativo.");
    }
    precio_ = precio;
}

void Producto::setCategoria(std::string categoria) {
    validarSinComas(categoria, "La categoria");
    categoria_ = std::move(categoria);
}

void Producto::setStockMinimo(int stockMinimo) {
    if (stockMinimo < 0) {
        throw EntradaInvalida("El stock minimo no puede ser negativo.");
    }
    stockMinimo_ = stockMinimo;
}

void Producto::aumentarStock(int cantidad) {
    if (cantidad < 0) {
        throw EntradaInvalida("La cantidad a aumentar no puede ser negativa.");
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
