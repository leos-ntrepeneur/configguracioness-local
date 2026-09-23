#include "Menu.h"
#include "Excepciones.h"
#include "GeneradorTicket.h"
#include "Utilidades.h"

#include <iomanip>
#include <iostream>
#include <vector>

using utilidades::confirmar;
using utilidades::leerDouble;
using utilidades::leerEntero;
using utilidades::leerLinea;

// Lista de inicializacion de miembros: es la forma idiomatica de
// inicializar referencias y const en C++. No se puede asignar una
// referencia dentro del cuerpo del constructor (a diferencia de un campo
// normal), tiene que "amarrarse" a su objetivo en este punto.
Menu::Menu(Inventario& inventario,
           GestorVentas& gestorVentas,
           IRepositorioProductos& repositorioProductos,
           IRepositorioVentas& repositorioVentas,
           IRepositorioInformacionNegocio& repositorioInformacionNegocio)
    : inventario_(inventario),
      gestorVentas_(gestorVentas),
      repositorioProductos_(repositorioProductos),
      repositorioVentas_(repositorioVentas),
      repositorioInformacionNegocio_(repositorioInformacionNegocio),
      informacionNegocio_(repositorioInformacionNegocio.cargar()) {}

void Menu::ejecutar() {
    bool salir = false;
    while (!salir) {
        mostrarMenuPrincipal();
        try {
            int opcion = leerEntero("Selecciona una opcion: ");
            switch (opcion) {
                case 1:
                    gestionarProductos();
                    break;
                case 2:
                    registrarVenta();
                    break;
                case 3:
                    alVerReporteVentasDelDia();
                    break;
                case 4:
                    alConfigurarInformacionNegocio();
                    break;
                case 0:
                    salir = true;
                    std::cout << "Hasta luego.\n";
                    break;
                default:
                    std::cout << "Opcion no valida.\n";
            }
        } catch (const FinDeEntrada&) {
            // Ver Excepciones.h: este catch debe ir ANTES que uno generico
            // de std::exception para poder distinguir "se acabo la entrada"
            // de un error de negocio comun.
            std::cout << "\nEntrada finalizada. Cerrando el programa.\n";
            salir = true;
        }
    }
}

void Menu::mostrarMenuPrincipal() const {
    std::cout << "\n=== Sistema de Inventario y Punto de Venta ===\n";
    std::cout << "1. Gestion de productos\n";
    std::cout << "2. Registrar venta\n";
    std::cout << "3. Reporte de ventas del dia\n";
    std::cout << "4. Informacion del local (para tickets)\n";
    std::cout << "0. Salir\n";
}

void Menu::gestionarProductos() {
    bool volver = false;
    while (!volver) {
        std::cout << "\n--- Gestion de productos ---\n";
        std::cout << "1. Dar de alta un producto\n";
        std::cout << "2. Editar un producto\n";
        std::cout << "3. Eliminar (baja) un producto\n";
        std::cout << "4. Listar productos\n";
        std::cout << "5. Buscar producto (por nombre o codigo)\n";
        std::cout << "6. Ver alertas de stock bajo\n";
        std::cout << "0. Volver al menu principal\n";
        // Cada operacion se envuelve en try/catch: si Producto/Inventario
        // lanzan una excepcion de negocio, la atrapamos aqui, mostramos el
        // mensaje y el programa sigue vivo (Requisito tecnico: manejar
        // errores sin que la app se caiga).
        try {
            int opcion = leerEntero("Selecciona una opcion: ");
            switch (opcion) {
                case 1: alDarAltaProducto(); break;
                case 2: alEditarProducto(); break;
                case 3: alEliminarProducto(); break;
                case 4: alListarProductos(); break;
                case 5: alBuscarProducto(); break;
                case 6: alVerAlertasStockBajo(); break;
                case 0: volver = true; break;
                default: std::cout << "Opcion no valida.\n";
            }
        } catch (const FinDeEntrada&) {
            // Se re-lanza para que Menu::ejecutar() tambien detenga su
            // propio bucle en vez de quedar esperando otra opcion.
            throw;
        } catch (const std::exception& e) {
            // std::exception es la clase base de la que heredan
            // std::runtime_error y, por lo tanto, nuestras excepciones de
            // Excepciones.h. Atraparla aqui cubre cualquier error de negocio
            // que definamos a futuro sin tener que listar cada tipo.
            std::cout << "Error: " << e.what() << "\n";
        }
    }
}

void Menu::alDarAltaProducto() {
    std::cout << "\n-- Alta de producto --\n";
    std::string codigo = leerLinea("Codigo (unico): ");
    std::string nombre = leerLinea("Nombre: ");
    double precio = leerDouble("Precio: ");
    int stock = leerEntero("Stock inicial: ");
    std::string categoria = leerLinea("Categoria (opcional, Enter para omitir): ");
    int stockMinimo = leerEntero("Alertar cuando el stock baje de (0 si no aplica): ");

    Producto nuevo(codigo, nombre, precio, stock, categoria, stockMinimo);
    inventario_.agregarProducto(nuevo);
    std::cout << "Producto agregado correctamente.\n";
    guardarDatos();
}

void Menu::alEditarProducto() {
    std::cout << "\n-- Editar producto --\n";
    std::string codigo = leerLinea("Codigo del producto a editar: ");

    // Consultamos primero para mostrar los valores actuales como referencia;
    // si no existe, buscarPorCodigo lanza ProductoNoEncontrado y el catch de
    // gestionarProductos() se encarga de mostrarlo.
    const Producto& actual = inventario_.buscarPorCodigo(codigo);
    std::cout << "Datos actuales -> nombre: " << actual.getNombre()
              << ", precio: " << actual.getPrecio()
              << ", categoria: " << actual.getCategoria()
              << ", stock actual: " << actual.getStock()
              << ", alerta si baja de: " << actual.getStockMinimo() << "\n";

    std::string nombre = leerLinea("Nuevo nombre: ");
    double precio = leerDouble("Nuevo precio: ");
    std::string categoria = leerLinea("Nueva categoria (Enter para dejar vacia): ");
    // El stock SI se puede corregir aqui a mano (ej. conteo fisico, mercancia
    // dañada) -- es un ajuste directo, distinto de que baje solo al vender.
    int stock = leerEntero("Stock real en existencia: ");
    int stockMinimo = leerEntero("Alertar cuando el stock baje de: ");

    inventario_.editarProducto(codigo, nombre, precio, stock, categoria, stockMinimo);
    std::cout << "Producto actualizado correctamente.\n";
    guardarDatos();
}

void Menu::alEliminarProducto() {
    std::cout << "\n-- Baja de producto --\n";
    std::string codigo = leerLinea("Codigo del producto a eliminar: ");
    const Producto& actual = inventario_.buscarPorCodigo(codigo);
    std::cout << "Vas a eliminar: " << actual.getNombre() << " (codigo " << codigo << ")\n";
    if (confirmar("Confirmas la eliminacion?")) {
        inventario_.eliminarProducto(codigo);
        std::cout << "Producto eliminado.\n";
        guardarDatos();
    } else {
        std::cout << "Operacion cancelada.\n";
    }
}

void Menu::alListarProductos() const {
    mostrarTablaProductos(inventario_.listarTodos());
}

void Menu::alBuscarProducto() const {
    std::cout << "\n-- Buscar producto --\n";
    std::string texto = leerLinea("Nombre o codigo a buscar: ");
    if (texto.empty()) {
        throw EntradaInvalida("Escribe algo para buscar.");
    }

    // Primero probamos coincidencia EXACTA de codigo (busqueda O(log n) en
    // el map de Inventario); si no hay, caemos a busqueda parcial por
    // nombre. Asi "MART001" encuentra el producto exacto y "mart"
    // encuentra por nombre aunque no sea un codigo valido.
    if (inventario_.existeCodigo(texto)) {
        // `{ ... }` aqui construye un std::vector<Producto> de un solo
        // elemento a partir de una lista de inicializacion (otra novedad de
        // C++11 en adelante, sin equivalente directo en C). Reutilizamos
        // asi la misma funcion de tabla que usa la busqueda por nombre.
        mostrarTablaProductos({inventario_.buscarPorCodigo(texto)});
        return;
    }

    std::vector<Producto> encontrados = inventario_.buscarPorNombre(texto);
    if (encontrados.empty()) {
        std::cout << "No se encontraron productos que coincidan con \"" << texto << "\".\n";
        return;
    }
    mostrarTablaProductos(encontrados);
}

void Menu::alVerAlertasStockBajo() const {
    std::vector<Producto> productos = inventario_.productosConStockBajo();
    std::cout << "\n-- Alertas de stock bajo --\n";
    if (productos.empty()) {
        std::cout << "No hay productos con stock bajo. Todo en orden.\n";
        return;
    }
    mostrarTablaProductos(productos);
}

void Menu::mostrarTablaProductos(const std::vector<Producto>& productos) const {
    if (productos.empty()) {
        std::cout << "No hay productos registrados.\n";
        return;
    }

    std::cout << "\n"
              << std::left << std::setw(10) << "Codigo"
              << std::setw(25) << "Nombre"
              << std::right << std::setw(10) << "Precio"
              << std::setw(8) << "Stock"
              << "  " << std::left << "Categoria" << "\n";
    std::cout << std::string(65, '-') << "\n";

    for (const Producto& p : productos) {
        std::cout << std::left << std::setw(10) << p.getCodigo()
                   << std::setw(25) << p.getNombre()
                   << std::right << std::setw(10) << std::fixed << std::setprecision(2) << p.getPrecio()
                   << std::setw(8) << p.getStock()
                   << "  " << std::left << p.getCategoria();
        if (p.estaBajoStockMinimo()) {
            std::cout << "  [STOCK BAJO]";
        }
        std::cout << "\n";
    }
}

void Menu::alVerReporteVentasDelDia() const {
    ReporteVentasDia reporte = gestorVentas_.generarReporteDelDia();

    std::cout << "\n=== Reporte de ventas del dia ===\n";
    std::cout << "Transacciones: " << reporte.numeroTransacciones << "\n";
    std::cout << "Total vendido: $" << std::fixed << std::setprecision(2)
              << reporte.totalVendido << "\n";

    if (reporte.productosMasVendidos.empty()) {
        std::cout << "Aun no se registran ventas hoy.\n";
        return;
    }

    std::cout << "\nProductos mas vendidos:\n"
              << std::left << std::setw(10) << "Codigo"
              << std::setw(25) << "Nombre"
              << std::right << std::setw(10) << "Unidades"
              << std::setw(12) << "Precio"
              << std::setw(14) << "Total" << "\n";
    std::cout << std::string(71, '-') << "\n";

    for (const ResumenProducto& resumen : reporte.productosMasVendidos) {
        // Precio unitario promedio del dia: totalVendido / cantidadVendida.
        // Se calcula (no se guarda) porque si el precio del producto cambio
        // a medio dia, esto muestra el promedio real de lo cobrado, no un
        // precio "actual" que podria no coincidir con ninguna venta.
        double precioPromedio = resumen.cantidadVendida > 0
                                     ? resumen.totalVendido / resumen.cantidadVendida
                                     : 0.0;
        std::cout << std::left << std::setw(10) << resumen.codigo
                   << std::setw(25) << resumen.nombre
                   << std::right << std::setw(10) << resumen.cantidadVendida
                   << std::setw(12) << std::fixed << std::setprecision(2) << precioPromedio
                   << std::setw(14) << std::fixed << std::setprecision(2) << resumen.totalVendido
                   << "\n";
    }

    mostrarHistorialTransacciones(reporte.transacciones);
}

void Menu::mostrarHistorialTransacciones(const std::vector<TransaccionDia>& transacciones) const {
    // Esta tabla es la razon por la que se agrego el folio: la de arriba
    // (productos mas vendidos) SUMA todas las ventas del dia; esta, en
    // cambio, muestra cada venta por separado -- asi "vendi 5 martillos
    // hoy" deja de ser una sola cifra ciega y se puede ver que fueron,
    // por ejemplo, 2 ventas distintas con su propio folio, hora y forma
    // de pago.
    std::cout << "\nHistorial de transacciones:\n"
              << std::left << std::setw(8) << "Folio"
              << std::setw(22) << "Fecha y hora"
              << std::setw(20) << "Metodo de pago"
              << std::right << std::setw(10) << "Unidades"
              << std::setw(12) << "Total" << "\n";
    std::cout << std::string(72, '-') << "\n";

    for (const TransaccionDia& t : transacciones) {
        std::cout << std::left << std::setw(8) << t.numeroTransaccion
                   << std::setw(22) << t.fechaHoraTexto
                   << std::setw(20) << metodoPagoATexto(t.metodoPago)
                   << std::right << std::setw(10) << t.cantidadProductos
                   << std::setw(12) << std::fixed << std::setprecision(2) << t.total
                   << "\n";
    }
}

void Menu::alConfigurarInformacionNegocio() {
    std::cout << "\n-- Informacion del local --\n";
    std::cout << "Esto se usa para el encabezado de los tickets de venta.\n";
    std::cout << "Datos actuales -> nombre: " << informacionNegocio_.getNombre()
              << ", direccion: " << informacionNegocio_.getDireccion()
              << ", telefono: " << informacionNegocio_.getTelefono()
              << ", RFC: " << informacionNegocio_.getRfc() << "\n\n";

    std::string nombre = leerLinea("Nombre del negocio (Enter para dejar igual): ");
    std::string direccion = leerLinea("Direccion (Enter para dejar igual): ");
    std::string telefono = leerLinea("Telefono (Enter para dejar igual): ");
    // El RFC es SOLO texto libre para que aparezca impreso en el ticket,
    // como en cualquier ticket de tienda -- no se valida contra el
    // formato real del SAT ni se usa para facturar electronicamente (ver
    // el comentario grande en InformacionNegocio.h).
    std::string rfc = leerLinea("RFC (ejemplo/placeholder, Enter para dejar igual): ");

    if (!nombre.empty()) informacionNegocio_.setNombre(nombre);
    if (!direccion.empty()) informacionNegocio_.setDireccion(direccion);
    if (!telefono.empty()) informacionNegocio_.setTelefono(telefono);
    if (!rfc.empty()) informacionNegocio_.setRfc(rfc);

    repositorioInformacionNegocio_.guardar(informacionNegocio_);
    std::cout << "Informacion del local guardada.\n";
}

// --- Registro de ventas ---
//
// El flujo es tipo "carrito de compras": el usuario va agregando productos
// (validando stock disponible en cada paso) y al final confirma para que
// GestorVentas cree la Venta real y descuente el inventario de una sola
// vez. Nada se descuenta mientras se arma el carrito, asi que cancelar no
// deja el inventario en un estado raro.

void Menu::registrarVenta() {
    // codigo -> cantidad acumulada. Vive solo durante este metodo: en
    // cuanto termina (se confirma o se cancela), el carrito desaparece.
    std::map<std::string, int> carrito;
    bool salirCarrito = false;

    while (!salirCarrito) {
        mostrarCarrito(carrito);
        std::cout << "\n--- Registrar venta ---\n";
        std::cout << "1. Agregar producto\n";
        std::cout << "2. Quitar producto\n";
        std::cout << "3. Confirmar venta\n";
        std::cout << "0. Cancelar y volver\n";

        try {
            int opcion = leerEntero("Selecciona una opcion: ");
            switch (opcion) {
                case 1:
                    agregarAlCarrito(carrito);
                    break;
                case 2:
                    quitarDelCarrito(carrito);
                    break;
                case 3:
                    if (carrito.empty()) {
                        std::cout << "El carrito esta vacio, agrega al menos un producto.\n";
                    } else {
                        confirmarVenta(carrito);
                        salirCarrito = true;
                    }
                    break;
                case 0:
                    salirCarrito = true;
                    std::cout << "Venta cancelada. No se modifico el inventario.\n";
                    break;
                default:
                    std::cout << "Opcion no valida.\n";
            }
        } catch (const FinDeEntrada&) {
            throw; // igual que en gestionarProductos: propaga hasta ejecutar().
        } catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << "\n";
        }
    }
}

void Menu::mostrarCarrito(const std::map<std::string, int>& carrito) const {
    if (carrito.empty()) {
        std::cout << "\nCarrito vacio.\n";
        return;
    }

    std::cout << "\n-- Carrito actual --\n";
    double total = 0.0;
    for (const auto& [codigo, cantidad] : carrito) {
        try {
            const Producto& producto = inventario_.buscarPorCodigo(codigo);
            double subtotal = producto.getPrecio() * cantidad;
            total += subtotal;
            std::cout << "  " << codigo << " - " << producto.getNombre()
                       << " x" << cantidad << " = "
                       << std::fixed << std::setprecision(2) << subtotal << "\n";
        } catch (const ProductoNoEncontrado&) {
            // El producto se elimino del inventario mientras estaba en el
            // carrito (en otra parte del menu). Lo avisamos en vez de
            // tronar; confirmarVenta() volvera a fallar mas claro si el
            // usuario intenta cerrar la venta con este producto todavia
            // dentro.
            std::cout << "  " << codigo << " x" << cantidad << " (producto ya no existe)\n";
        }
    }
    std::cout << "  Total estimado: " << std::fixed << std::setprecision(2) << total << "\n";
}

void Menu::agregarAlCarrito(std::map<std::string, int>& carrito) {
    std::string codigo = leerLinea("Codigo del producto: ");
    const Producto& producto = inventario_.buscarPorCodigo(codigo); // lanza si no existe.

    int cantidad = leerEntero("Cantidad: ");
    if (cantidad <= 0) {
        throw EntradaInvalida("La cantidad debe ser mayor a cero.");
    }
    // Sin este limite, alguien podria escribir una cantidad cercana al
    // maximo de un int (leerEntero no tiene techo propio) y, sumada a lo
    // que ya hubiera en el carrito, DESBORDAR el int de abajo -- un
    // desborde de entero con signo es comportamiento indefinido en C++,
    // no simplemente "da un numero raro". Producto::STOCK_MAXIMO ya es el
    // limite real de cualquier stock, asi que pedir mas que eso de una
    // vez nunca puede ser una venta valida de todos modos.
    if (cantidad > Producto::STOCK_MAXIMO) {
        throw EntradaInvalida("La cantidad maxima por movimiento es " +
                               std::to_string(Producto::STOCK_MAXIMO) + " unidades.");
    }

    // Si el producto ya estaba en el carrito, sumamos a lo que ya habia
    // pedido, y validamos el TOTAL acumulado contra el stock real (el
    // stock del inventario todavia no se ha tocado). Se suma en long long
    // como defensa adicional: con el limite de arriba ya no deberia poder
    // desbordar un int, pero sumar en 64 bits y recien despues comparar
    // es gratis y quita cualquier duda.
    int yaEnCarrito = carrito.count(codigo) ? carrito.at(codigo) : 0;
    long long totalSolicitado = static_cast<long long>(yaEnCarrito) + cantidad;
    if (totalSolicitado > producto.getStock()) {
        // static_cast<int> es seguro: totalSolicitado esta acotado por
        // yaEnCarrito (<= STOCK_MAXIMO por invariante del carrito) mas
        // cantidad (<= STOCK_MAXIMO por el chequeo de arriba), asi que
        // como mucho vale 2 * STOCK_MAXIMO -- muy por debajo del limite
        // de un int.
        throw StockInsuficiente(codigo, producto.getStock(), static_cast<int>(totalSolicitado));
    }

    carrito[codigo] = static_cast<int>(totalSolicitado);
    std::cout << producto.getNombre() << " agregado al carrito (cantidad en carrito: "
              << totalSolicitado << ").\n";
}

void Menu::quitarDelCarrito(std::map<std::string, int>& carrito) {
    if (carrito.empty()) {
        std::cout << "El carrito ya esta vacio.\n";
        return;
    }
    std::string codigo = leerLinea("Codigo del producto a quitar: ");
    auto it = carrito.find(codigo);
    if (it == carrito.end()) {
        std::cout << "Ese codigo no esta en el carrito.\n";
        return;
    }
    carrito.erase(it);
    std::cout << "Producto quitado del carrito.\n";
}

MetodoPago Menu::preguntarMetodoPago() const {
    while (true) {
        std::cout << "\nMetodo de pago:\n";
        std::cout << "1. Efectivo\n";
        std::cout << "2. Tarjeta de credito\n";
        std::cout << "3. Tarjeta de debito\n";
        int opcion = leerEntero("Selecciona una opcion: ");
        switch (opcion) {
            case 1:
                return MetodoPago::Efectivo;
            case 2:
                return MetodoPago::TarjetaCredito;
            case 3:
                return MetodoPago::TarjetaDebito;
            default:
                std::cout << "Opcion invalida, intenta de nuevo.\n";
        }
    }
}

void Menu::confirmarVenta(const std::map<std::string, int>& carrito) {
    // Construimos los DetalleVenta con el nombre/precio ACTUALES del
    // producto (snapshot en el momento de vender). GestorVentas hace su
    // propia validacion de stock antes de descontar nada.
    std::vector<DetalleVenta> detalles;
    detalles.reserve(carrito.size());
    for (const auto& [codigo, cantidad] : carrito) {
        const Producto& producto = inventario_.buscarPorCodigo(codigo);
        detalles.emplace_back(codigo, producto.getNombre(), cantidad, producto.getPrecio());
    }

    MetodoPago metodoPago = preguntarMetodoPago();
    const Venta& venta = gestorVentas_.registrarVenta(detalles, metodoPago);

    // El ticket (mismo texto que se mostraria en la GUI, ver GeneradorTicket.h)
    // ya trae folio, fecha, metodo de pago y el desglose completo -- no hace
    // falta repetir un resumen aparte encima.
    std::cout << "\n" << generarTextoTicket(venta, informacionNegocio_);

    // Requisito 4: avisar de inmediato si esta venta dejo algun producto
    // por debajo de su stock minimo, sin que el usuario tenga que ir a
    // buscarlo despues en el listado.
    for (const DetalleVenta& detalle : venta.getDetalles()) {
        const Producto& actualizado = inventario_.buscarPorCodigo(detalle.getCodigoProducto());
        if (actualizado.estaBajoStockMinimo()) {
            std::cout << "AVISO: " << actualizado.getNombre() << " quedo con stock bajo ("
                      << actualizado.getStock() << " unidades, minimo "
                      << actualizado.getStockMinimo() << ").\n";
        }
    }

    guardarDatos();
}

void Menu::guardarDatos() const {
    // Un fallo al guardar (disco lleno, sin permisos de escritura) no debe
    // tumbar el programa ni hacer que el usuario pierda lo que ya hizo en
    // memoria: se avisa y se sigue. Los datos se intentaran guardar de
    // nuevo en la siguiente operacion.
    try {
        repositorioProductos_.guardarTodos(inventario_.listarTodos());
        repositorioVentas_.guardarTodas(gestorVentas_.listarVentas());
    } catch (const std::exception& e) {
        std::cout << "Aviso: no se pudieron guardar los datos en disco (" << e.what() << ").\n";
    }
}
