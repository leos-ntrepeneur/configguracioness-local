#include "GestorCortes.h"

#include <chrono>

GestorCortes::GestorCortes(const GestorVentas& gestorVentas) : gestorVentas_(gestorVentas) {}

void GestorCortes::cargarCortes(std::vector<CorteCaja> cortes) {
    cortes_ = std::move(cortes);

    siguienteNumeroCorte_ = 1;
    for (const CorteCaja& corte : cortes_) {
        if (corte.getNumeroCorte() >= siguienteNumeroCorte_) {
            siguienteNumeroCorte_ = corte.getNumeroCorte() + 1;
        }
    }
}

const CorteCaja& GestorCortes::cerrarDia() {
    ReporteVentasDia reporte = gestorVentas_.generarReporteDelDia();
    int numeroCorte = siguienteNumeroCorte_++;
    cortes_.emplace_back(numeroCorte, reporte, std::chrono::system_clock::now());
    return cortes_.back();
}

const std::vector<CorteCaja>& GestorCortes::listarCortes() const {
    return cortes_;
}

const CorteCaja* GestorCortes::buscarPorNumeroCorte(int numeroCorte) const {
    for (const CorteCaja& corte : cortes_) {
        if (corte.getNumeroCorte() == numeroCorte) {
            return &corte;
        }
    }
    return nullptr;
}
