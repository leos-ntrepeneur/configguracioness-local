// GestorCortes.h
//
// Administra el historial de cortes de caja: les asigna un numero de
// corte secuencial (mismo patron que GestorVentas con el folio de venta)
// y los mantiene en memoria. La persistencia en disco es responsabilidad
// de quien llama (Menu/MainWindow, via IRepositorioCortes), exactamente
// igual que GestorVentas::registrarVenta seguido de un guardarDatos()
// aparte -- GestorCortes no sabe nada de CSV ni de archivos.

#ifndef GESTOR_CORTES_H
#define GESTOR_CORTES_H

#include <vector>

#include "CorteCaja.h"
#include "GestorVentas.h"

class GestorCortes {
public:
    // Recibe GestorVentas por referencia CONSTANTE: solo necesita poder
    // pedirle generarReporteDelDia() al momento de cerrar el dia, nunca lo
    // modifica.
    explicit GestorCortes(const GestorVentas& gestorVentas);

    // Reemplaza el historial en memoria con lo leido de persistencia al
    // iniciar el programa, y recalcula el siguiente numero de corte a
    // partir del mayor numeroCorte cargado (mismo patron que
    // GestorVentas::cargarVentas con los folios).
    void cargarCortes(std::vector<CorteCaja> cortes);

    // Genera un nuevo corte a partir del reporte de ventas VIGENTE (llama
    // a gestorVentas_.generarReporteDelDia() internamente; no recibe
    // parametros porque siempre es "ahora"), lo agrega al historial en
    // memoria y devuelve una referencia a el. Se puede llamar mas de una
    // vez el mismo dia (por ejemplo, un corte parcial a medio dia y el
    // corte final al cerrar) -- cada llamada es un registro nuevo e
    // independiente, ninguno modifica ni invalida al anterior.
    const CorteCaja& cerrarDia();

    const std::vector<CorteCaja>& listarCortes() const;

    // Busca un corte ya generado por su numero (para volver a ver su
    // detalle desde el historial). Devuelve `nullptr` si no existe.
    const CorteCaja* buscarPorNumeroCorte(int numeroCorte) const;

private:
    const GestorVentas& gestorVentas_;
    std::vector<CorteCaja> cortes_;
    int siguienteNumeroCorte_ = 1;
};

#endif // GESTOR_CORTES_H
