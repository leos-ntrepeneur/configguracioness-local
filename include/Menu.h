// Menu.h
//
// Capa de presentacion: solo imprime texto, lee opciones y llama a los
// metodos de Inventario/GestorVentas. No valida reglas de negocio aqui --
// esas viven en Producto/Inventario/GestorVentas y se manejan con
// try/catch para que un error no tumbe el programa.

#ifndef MENU_H
#define MENU_H

#include <map>
#include <string>
#include <vector>

#include "CorteCaja.h"
#include "GestorClientes.h"
#include "GestorCortes.h"
#include "GestorCreditos.h"
#include "GestorVentas.h"
#include "IRepositorioAbonos.h"
#include "IRepositorioClientes.h"
#include "IRepositorioCortes.h"
#include "IRepositorioInformacionNegocio.h"
#include "IRepositorioProductos.h"
#include "IRepositorioVentas.h"
#include "InformacionNegocio.h"
#include "Inventario.h"
#include "MetodoPago.h"

class Menu {
public:
    // Recibe REFERENCIAS a Inventario/GestorVentas/repositorios, no copias
    // ni punteros: Menu no es dueno de ninguno, solo los usa. `&` es el
    // operador de referencia de C++: un alias sin memoria propia hacia el
    // objeto original (distinto de un puntero, que si ocupa su propia
    // memoria y puede ser nulo).
    //
    // Notese que los parametros son IRepositorioProductos&/IRepositorioVentas&
    // (la INTERFAZ), no RepositorioProductosCsv&/RepositorioVentasCsv&
    // (la implementacion concreta). Esto es "programar contra una
    // interfaz": Menu puede guardar/cargar datos sin saber ni importarle
    // si por debajo es un CSV o, mas adelante, una base SQLite.
    Menu(Inventario& inventario,
         GestorVentas& gestorVentas,
         GestorCortes& gestorCortes,
         GestorClientes& gestorClientes,
         GestorCreditos& gestorCreditos,
         IRepositorioProductos& repositorioProductos,
         IRepositorioVentas& repositorioVentas,
         IRepositorioInformacionNegocio& repositorioInformacionNegocio,
         IRepositorioCortes& repositorioCortes,
         IRepositorioClientes& repositorioClientes,
         IRepositorioAbonos& repositorioAbonos);

    // Punto de entrada: corre el bucle principal hasta que el usuario
    // elige salir.
    void ejecutar();

private:
    void mostrarMenuPrincipal() const;
    void gestionarProductos();

    void alDarAltaProducto();
    void alEditarProducto();
    void alEliminarProducto();
    void alListarProductos() const;
    void alBuscarProducto() const;
    void alVerAlertasStockBajo() const;

    void alVerReporteVentasDelDia() const;
    // Imprime el historial de transacciones del dia (folio, hora, metodo
    // de pago, total), como parte del reporte -- es lo que antes faltaba
    // para poder distinguir una venta de otra en vez de solo ver el total
    // sumado por producto.
    void mostrarHistorialTransacciones(const std::vector<TransaccionDia>& transacciones) const;
    // Grafica de barras (con caracteres de texto) de ventas por categoria,
    // como parte del reporte -- se imprime siempre que se ve el reporte,
    // sin pedirla aparte (ver GraficaBarras.h en la GUI para el
    // equivalente grafico).
    void mostrarGraficaVentasPorCategoria(const std::vector<ResumenCategoria>& ventasPorCategoria) const;

    void alConfigurarInformacionNegocio();

    // --- Cortes de caja (cierre diario) ---
    // Pide confirmacion (es un registro permanente, no se puede deshacer),
    // genera el corte a partir del reporte de ventas vigente, lo persiste
    // y muestra su detalle -- mismo flujo que confirmarVenta()+ticket, pero
    // para el resumen del dia completo en vez de una sola venta.
    void alCerrarDia();
    // Lista los cortes ya generados (tabla resumen) y opcionalmente
    // muestra el detalle completo de uno de ellos.
    void alVerCortesAnteriores() const;
    void mostrarTablaCortes(const std::vector<CorteCaja>& cortes) const;

    // --- Archivo de ventas (historial completo, todos los dias) ---
    // A diferencia de alVerReporteVentasDelDia() (SOLO hoy), esto lista
    // TODAS las ventas registradas desde siempre -- ya se guardan
    // completas en ventas.csv, esto es solo la vista para revisarlas.
    void alVerArchivoVentas() const;
    void mostrarTablaVentas(const std::vector<Venta>& ventas) const;

    // --- Clientes y ventas al fiado ---
    // Idea de negocio pensada para abarrotes/ferreterias mexicanas: vender
    // "al fiado" a clientes de confianza, cobrando despues. El saldo que
    // debe cada cliente NUNCA se guarda como numero aparte -- se calcula
    // en vivo (ver GestorCreditos) sumando sus ventas al fiado y
    // restandole sus abonos.
    void alGestionarClientes();
    void alDarAltaCliente();
    void alListarClientes() const;
    void mostrarTablaClientes(const std::vector<Cliente>& clientes) const;
    void alRegistrarAbono();
    // Usado por confirmarVenta() cuando el metodo de pago elegido es
    // Fiado: pide un id de cliente existente o da de alta uno nuevo en el
    // momento. Reintenta hasta obtener un cliente valido (igual criterio
    // que preguntarMetodoPago(): una eleccion de menu invalida no es un
    // error de negocio, solo hay que repetir la pregunta).
    const Cliente& elegirOCrearCliente();

    // Compartida por alListarProductos() y alBuscarProducto() para no
    // duplicar el formato de tabla en dos lugares.
    void mostrarTablaProductos(const std::vector<Producto>& productos) const;

    // --- Registro de ventas ---
    void registrarVenta();
    // El "carrito" vive solo mientras se arma la venta: codigo -> cantidad
    // acumulada. Se pasa por referencia para que las funciones auxiliares
    // lo modifiquen sin necesidad de devolverlo y reasignarlo.
    void mostrarCarrito(const std::map<std::string, int>& carrito) const;
    void agregarAlCarrito(std::map<std::string, int>& carrito);
    void quitarDelCarrito(std::map<std::string, int>& carrito);
    void confirmarVenta(const std::map<std::string, int>& carrito);
    // Pide el metodo de pago por teclado (1/2/3); reintenta hasta que la
    // opcion sea valida en vez de lanzar EntradaInvalida por una eleccion
    // de menu -- no es un dato de negocio invalido, solo hay que repetir
    // la pregunta.
    MetodoPago preguntarMetodoPago() const;

    // --- Persistencia (Requisito 6) ---
    // Se llama despues de cada alta/edicion/baja/venta exitosa, para que
    // una salida inesperada (cerrar la ventana, un corte de luz) pierda
    // como maximo la operacion en curso, no todo el historial previo.
    void guardarDatos() const;

    Inventario& inventario_;
    GestorVentas& gestorVentas_;
    GestorCortes& gestorCortes_;
    GestorClientes& gestorClientes_;
    GestorCreditos& gestorCreditos_;
    IRepositorioProductos& repositorioProductos_;
    IRepositorioVentas& repositorioVentas_;
    IRepositorioInformacionNegocio& repositorioInformacionNegocio_;
    IRepositorioCortes& repositorioCortes_;
    IRepositorioClientes& repositorioClientes_;
    IRepositorioAbonos& repositorioAbonos_;
    // A diferencia de Inventario/GestorVentas (colecciones grandes que
    // viven fuera de Menu), la InformacionNegocio es un solo registro
    // chico: Menu la mantiene en memoria directamente y la guarda a
    // traves del repositorio cuando el usuario la edita.
    InformacionNegocio informacionNegocio_;
};

#endif // MENU_H
