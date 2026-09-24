#include "Menu.h"
#include "Excepciones.h"
#include "GeneradorCorte.h"
#include "GeneradorTicket.h"
#include "Iva.h"
#include "Utilidades.h"

#include <algorithm>
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
           GestorCortes& gestorCortes,
           GestorClientes& gestorClientes,
           GestorCreditos& gestorCreditos,
           IRepositorioProductos& repositorioProductos,
           IRepositorioVentas& repositorioVentas,
           IRepositorioInformacionNegocio& repositorioInformacionNegocio,
           IRepositorioCortes& repositorioCortes,
           IRepositorioClientes& repositorioClientes,
           IRepositorioAbonos& repositorioAbonos)
    : inventario_(inventario),
      gestorVentas_(gestorVentas),
      gestorCortes_(gestorCortes),
      gestorClientes_(gestorClientes),
      gestorCreditos_(gestorCreditos),
      repositorioProductos_(repositorioProductos),
      repositorioVentas_(repositorioVentas),
      repositorioInformacionNegocio_(repositorioInformacionNegocio),
      repositorioCortes_(repositorioCortes),
      repositorioClientes_(repositorioClientes),
      repositorioAbonos_(repositorioAbonos),
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
                case 5:
                    alCerrarDia();
                    break;
                case 6:
                    alVerCortesAnteriores();
                    break;
                case 7:
                    alVerArchivoVentas();
                    break;
                case 8:
                    alGestionarClientes();
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
    std::cout << "5. Cerrar el dia (corte de caja)\n";
    std::cout << "6. Ver cortes anteriores\n";
    std::cout << "7. Ver archivo completo de ventas\n";
    std::cout << "8. Clientes y ventas al fiado\n";
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
    double precio = leerDouble("Precio (IVA incluido): ");
    std::cout << "  (IVA incluido en ese precio: $" << std::fixed << std::setprecision(2)
              << iva::calcularMontoIva(precio) << ")\n";
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
    double precio = leerDouble("Nuevo precio (IVA incluido): ");
    std::cout << "  (IVA incluido en ese precio: $" << std::fixed << std::setprecision(2)
              << iva::calcularMontoIva(precio) << ")\n";
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
    mostrarGraficaVentasPorCategoria(reporte.ventasPorCategoria);

    int numeroTransaccion = leerEntero("\nVer ticket de una venta (folio, 0 para omitir): ");
    if (numeroTransaccion == 0) {
        return;
    }
    const Venta* venta = gestorVentas_.buscarPorNumeroTransaccion(numeroTransaccion);
    if (venta == nullptr) {
        std::cout << "No existe una venta con ese folio.\n";
        return;
    }
    std::cout << "\n" << generarTextoTicket(*venta, informacionNegocio_);
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

void Menu::mostrarGraficaVentasPorCategoria(const std::vector<ResumenCategoria>& ventasPorCategoria) const {
    if (ventasPorCategoria.empty()) {
        return;
    }

    std::cout << "\nVentas por categoria:\n";

    // El ancho de cada barra es PROPORCIONAL al total de la categoria mas
    // vendida (la primera del vector, porque generarReporteDelDia() ya lo
    // ordena de mayor a menor): esa categoria pinta la barra completa
    // (ANCHO_MAXIMO caracteres) y las demas se escalan contra ella. Sin
    // esta normalizacion, una categoria con $50 de ventas se veria con una
    // barra casi tan larga como una de $5,000.
    constexpr int ANCHO_MAXIMO = 30;
    double mayorTotal = ventasPorCategoria.front().totalVendido;

    // El ancho de la columna de nombres se ajusta al nombre mas largo (con
    // un minimo razonable) para que las barras siempre arranquen alineadas
    // sin importar si las categorias son cortas ("Aseo") o largas
    // ("Materiales de construccion").
    std::size_t anchoNombre = 12;
    for (const ResumenCategoria& r : ventasPorCategoria) {
        anchoNombre = std::max(anchoNombre, r.categoria.size());
    }

    for (const ResumenCategoria& r : ventasPorCategoria) {
        int anchoBarra = mayorTotal > 0.0
                              ? static_cast<int>((r.totalVendido / mayorTotal) * ANCHO_MAXIMO)
                              : 0;
        // Una categoria con ventas (aunque sean chicas) siempre pinta al
        // menos 1 caracter -- si no, una diferencia real de $0.01 contra
        // $0.00 se veria identica (barra vacia) a no haber vendido nada.
        if (anchoBarra == 0 && r.totalVendido > 0.0) {
            anchoBarra = 1;
        }

        std::cout << std::left << std::setw(static_cast<int>(anchoNombre)) << r.categoria << " ["
                   << std::string(static_cast<std::size_t>(anchoBarra), '#')
                   << std::string(static_cast<std::size_t>(ANCHO_MAXIMO - anchoBarra), ' ') << "] $"
                   << std::right << std::fixed << std::setprecision(2) << r.totalVendido << "\n";
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

void Menu::alCerrarDia() {
    std::cout << "\n-- Cerrar el dia (corte de caja) --\n";
    std::cout << "Esto genera un registro PERMANENTE con el resumen de las ventas de hoy\n";
    std::cout << "(folios incluidos, total por efectivo/tarjeta). No modifica las ventas\n";
    std::cout << "ya registradas -- es solo un resumen archivado, y no se puede deshacer.\n";
    if (!confirmar("Confirmas el cierre del dia?")) {
        std::cout << "Cierre cancelado.\n";
        return;
    }

    const CorteCaja& corte = gestorCortes_.cerrarDia();
    try {
        repositorioCortes_.agregar(corte);
    } catch (const std::exception& e) {
        std::cout << "Aviso: no se pudo guardar el corte en disco (" << e.what() << ").\n";
    }

    std::cout << "\n" << generarTextoCorte(corte, informacionNegocio_);
}

void Menu::alVerCortesAnteriores() const {
    const std::vector<CorteCaja>& cortes = gestorCortes_.listarCortes();
    std::cout << "\n-- Cortes de caja anteriores --\n";
    if (cortes.empty()) {
        std::cout << "Todavia no se ha cerrado ningun dia.\n";
        return;
    }

    mostrarTablaCortes(cortes);

    int numeroCorte = leerEntero("\nVer detalle de un corte (numero, 0 para omitir): ");
    if (numeroCorte == 0) {
        return;
    }
    const CorteCaja* corte = gestorCortes_.buscarPorNumeroCorte(numeroCorte);
    if (corte == nullptr) {
        std::cout << "No existe un corte con ese numero.\n";
        return;
    }
    std::cout << "\n" << generarTextoCorte(*corte, informacionNegocio_);
}

void Menu::mostrarTablaCortes(const std::vector<CorteCaja>& cortes) const {
    std::cout << std::left << std::setw(10) << "Corte"
               << std::setw(22) << "Cierre"
               << std::setw(14) << "Folios"
               << std::right << std::setw(10) << "Ventas"
               << std::setw(14) << "Total" << "\n";
    std::cout << std::string(70, '-') << "\n";

    for (const CorteCaja& corte : cortes) {
        std::string folios = corte.getNumeroTransacciones() > 0
                                  ? std::to_string(corte.getFolioInicial()) + "-" +
                                        std::to_string(corte.getFolioFinal())
                                  : "-";
        std::cout << std::left << std::setw(10) << corte.getNumeroCorte()
                   << std::setw(22) << corte.fechaHoraCierreComoTexto()
                   << std::setw(14) << folios
                   << std::right << std::setw(10) << corte.getNumeroTransacciones()
                   << std::setw(14) << std::fixed << std::setprecision(2) << corte.getTotalVendido()
                   << "\n";
    }
}

void Menu::alVerArchivoVentas() const {
    std::vector<Venta> ventas = gestorVentas_.listarVentas(); // copia: se ordena/filtra sin tocar el historial real.
    std::cout << "\n-- Archivo de ventas (historial completo) --\n";
    if (ventas.empty()) {
        std::cout << "Todavia no se ha registrado ninguna venta.\n";
        return;
    }

    // Mas reciente primero: es lo que casi siempre se quiere revisar,
    // aunque el archivo interno vaya de mas vieja a mas nueva.
    std::sort(ventas.begin(), ventas.end(),
              [](const Venta& a, const Venta& b) { return a.getNumeroTransaccion() > b.getNumeroTransaccion(); });

    std::string filtro = leerLinea("Filtrar por folio o fecha (Enter para ver todo): ");
    if (!filtro.empty()) {
        std::vector<Venta> filtradas;
        for (const Venta& venta : ventas) {
            std::string folioTexto = std::to_string(venta.getNumeroTransaccion());
            if (folioTexto.find(filtro) != std::string::npos || venta.fechaComoTexto().find(filtro) != std::string::npos) {
                filtradas.push_back(venta);
            }
        }
        ventas = std::move(filtradas);
    }

    if (ventas.empty()) {
        std::cout << "Ninguna venta coincide con ese filtro.\n";
        return;
    }

    mostrarTablaVentas(ventas);

    int numeroTransaccion = leerEntero("\nVer ticket de una venta (folio, 0 para omitir): ");
    if (numeroTransaccion == 0) {
        return;
    }
    const Venta* venta = gestorVentas_.buscarPorNumeroTransaccion(numeroTransaccion);
    if (venta == nullptr) {
        std::cout << "No existe una venta con ese folio.\n";
        return;
    }
    std::cout << "\n" << generarTextoTicket(*venta, informacionNegocio_);
}

void Menu::mostrarTablaVentas(const std::vector<Venta>& ventas) const {
    double totalGeneral = 0.0;
    for (const Venta& venta : ventas) {
        totalGeneral += venta.getTotal();
    }
    std::cout << ventas.size() << " venta(s) -- Total: $" << std::fixed << std::setprecision(2) << totalGeneral
              << "\n\n";

    std::cout << std::left << std::setw(8) << "Folio"
               << std::setw(22) << "Fecha y hora"
               << std::setw(20) << "Metodo de pago"
               << std::right << std::setw(10) << "Unidades"
               << std::setw(12) << "Total" << "\n";
    std::cout << std::string(72, '-') << "\n";

    for (const Venta& venta : ventas) {
        int unidades = 0;
        for (const DetalleVenta& detalle : venta.getDetalles()) {
            unidades += detalle.getCantidad();
        }
        std::cout << std::left << std::setw(8) << venta.getNumeroTransaccion()
                   << std::setw(22) << venta.fechaComoTexto()
                   << std::setw(20) << metodoPagoATexto(venta.getMetodoPago())
                   << std::right << std::setw(10) << unidades
                   << std::setw(12) << std::fixed << std::setprecision(2) << venta.getTotal()
                   << "\n";
    }
}

void Menu::alGestionarClientes() {
    bool volver = false;
    while (!volver) {
        std::cout << "\n--- Clientes y ventas al fiado ---\n";
        std::cout << "1. Dar de alta un cliente\n";
        std::cout << "2. Ver clientes y su saldo pendiente\n";
        std::cout << "3. Registrar un abono (pago)\n";
        std::cout << "0. Volver al menu principal\n";
        try {
            int opcion = leerEntero("Selecciona una opcion: ");
            switch (opcion) {
                case 1: alDarAltaCliente(); break;
                case 2: alListarClientes(); break;
                case 3: alRegistrarAbono(); break;
                case 0: volver = true; break;
                default: std::cout << "Opcion no valida.\n";
            }
        } catch (const FinDeEntrada&) {
            throw; // igual que en gestionarProductos: propaga hasta ejecutar().
        } catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << "\n";
        }
    }
}

void Menu::alDarAltaCliente() {
    std::cout << "\n-- Nuevo cliente --\n";
    std::string nombre = leerLinea("Nombre: ");
    std::string telefono = leerLinea("Telefono (opcional, Enter para omitir): ");
    const Cliente& cliente = gestorClientes_.agregarCliente(nombre, telefono);
    std::cout << "Cliente agregado con id " << cliente.getId() << ".\n";
    guardarDatos();
}

void Menu::alListarClientes() const {
    std::vector<Cliente> clientes = gestorClientes_.listarTodos();
    std::cout << "\n-- Clientes --\n";
    if (clientes.empty()) {
        std::cout << "Todavia no hay clientes registrados.\n";
        return;
    }
    mostrarTablaClientes(clientes);
}

void Menu::mostrarTablaClientes(const std::vector<Cliente>& clientes) const {
    std::cout << std::left << std::setw(6) << "Id"
               << std::setw(25) << "Nombre"
               << std::setw(15) << "Telefono"
               << std::right << std::setw(16) << "Saldo pendiente" << "\n";
    std::cout << std::string(62, '-') << "\n";

    for (const Cliente& cliente : clientes) {
        // El saldo NUNCA se lee de un campo guardado -- se calcula en
        // vivo cada vez (ver GestorCreditos::saldoPendiente), asi que
        // siempre refleja las ventas/abonos mas recientes.
        double saldo = gestorCreditos_.saldoPendiente(cliente.getId());
        std::cout << std::left << std::setw(6) << cliente.getId()
                   << std::setw(25) << cliente.getNombre()
                   << std::setw(15) << cliente.getTelefono()
                   << std::right << std::setw(16) << std::fixed << std::setprecision(2) << saldo
                   << "\n";
    }
}

void Menu::alRegistrarAbono() {
    std::vector<Cliente> clientes = gestorClientes_.listarTodos();
    std::cout << "\n-- Registrar abono --\n";
    if (clientes.empty()) {
        std::cout << "Todavia no hay clientes registrados.\n";
        return;
    }
    mostrarTablaClientes(clientes);

    int id = leerEntero("\nId del cliente que abona: ");
    const Cliente* cliente = gestorClientes_.buscarPorId(id);
    if (cliente == nullptr) {
        std::cout << "No existe un cliente con ese id.\n";
        return;
    }
    double pendiente = gestorCreditos_.saldoPendiente(id);
    if (pendiente <= 0.0) {
        std::cout << cliente->getNombre() << " no tiene saldo pendiente.\n";
        return;
    }
    std::cout << "Saldo pendiente de " << cliente->getNombre() << ": $" << std::fixed << std::setprecision(2)
              << pendiente << "\n";

    double monto = leerDouble("Monto a abonar: ");
    // registrarAbono lanza EntradaInvalida si el monto excede el saldo
    // pendiente (ver GestorCreditos.cpp) -- se deja propagar hasta el
    // catch de alGestionarClientes(), igual que cualquier otro error de
    // negocio en este programa.
    const Abono& abono = gestorCreditos_.registrarAbono(id, monto);
    try {
        repositorioAbonos_.agregar(abono);
    } catch (const std::exception& e) {
        std::cout << "Aviso: no se pudo guardar el abono en disco (" << e.what() << ").\n";
    }

    std::cout << "Abono #" << abono.getNumeroAbono() << " registrado. Nuevo saldo pendiente: $" << std::fixed
              << std::setprecision(2) << gestorCreditos_.saldoPendiente(id) << "\n";
}

const Cliente& Menu::elegirOCrearCliente() {
    while (true) {
        std::vector<Cliente> clientes = gestorClientes_.listarTodos();
        if (!clientes.empty()) {
            std::cout << "\nClientes existentes:\n";
            mostrarTablaClientes(clientes);
        }

        std::cout << "\n-- Cliente para la venta al fiado --\n";
        std::cout << "1. Usar un cliente existente (por id)\n";
        std::cout << "2. Dar de alta un cliente nuevo\n";
        int opcion = leerEntero("Selecciona una opcion: ");

        if (opcion == 1) {
            int id = leerEntero("Id del cliente: ");
            const Cliente* cliente = gestorClientes_.buscarPorId(id);
            if (cliente == nullptr) {
                std::cout << "No existe un cliente con ese id.\n";
                continue;
            }
            return *cliente;
        } else if (opcion == 2) {
            std::string nombre = leerLinea("Nombre del cliente nuevo: ");
            std::string telefono = leerLinea("Telefono (opcional, Enter para omitir): ");
            try {
                return gestorClientes_.agregarCliente(nombre, telefono);
            } catch (const std::exception& e) {
                std::cout << "Error: " << e.what() << "\n";
            }
        } else {
            std::cout << "Opcion invalida, intenta de nuevo.\n";
        }
    }
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
        std::cout << "4. Fiado (a credito, se cobra despues)\n";
        int opcion = leerEntero("Selecciona una opcion: ");
        switch (opcion) {
            case 1:
                return MetodoPago::Efectivo;
            case 2:
                return MetodoPago::TarjetaCredito;
            case 3:
                return MetodoPago::TarjetaDebito;
            case 4:
                return MetodoPago::Fiado;
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

    // Solo una venta al fiado necesita un cliente asociado -- para
    // cualquier otro metodo de pago, clienteId/nombreCliente se quedan en
    // sus valores "no aplica" (0 / cadena vacia, ver Venta.h).
    int clienteId = 0;
    std::string nombreCliente;
    if (metodoPago == MetodoPago::Fiado) {
        const Cliente& cliente = elegirOCrearCliente();
        clienteId = cliente.getId();
        nombreCliente = cliente.getNombre();
    }

    const Venta& venta = gestorVentas_.registrarVenta(detalles, metodoPago, clienteId, nombreCliente);

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
        repositorioClientes_.guardarTodos(gestorClientes_.listarTodos());
    } catch (const std::exception& e) {
        std::cout << "Aviso: no se pudieron guardar los datos en disco (" << e.what() << ").\n";
    }
}
