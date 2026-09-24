#include "GestorClientes.h"

#include <algorithm>

const Cliente& GestorClientes::agregarCliente(std::string nombre, std::string telefono) {
    int id = siguienteId_++;
    // El constructor de Cliente valida nombre/telefono (vacio, comas,
    // longitud) y lanza EntradaInvalida si algo esta mal -- si eso pasa,
    // siguienteId_ ya avanzo, pero eso es igual de inofensivo que un folio
    // de venta "saltado": nadie depende de que los ids sean consecutivos
    // sin huecos, solo de que nunca se repitan.
    auto resultado = clientes_.emplace(id, Cliente(id, std::move(nombre), std::move(telefono)));
    return resultado.first->second;
}

const Cliente* GestorClientes::buscarPorId(int id) const {
    auto it = clientes_.find(id);
    if (it == clientes_.end()) {
        return nullptr;
    }
    return &it->second;
}

std::vector<Cliente> GestorClientes::buscarPorNombre(const std::string& textoParcial) const {
    std::vector<Cliente> resultado;
    std::string textoBuscado = textoParcial;
    std::transform(textoBuscado.begin(), textoBuscado.end(), textoBuscado.begin(), ::tolower);

    for (const auto& [id, cliente] : clientes_) {
        std::string nombreMinusculas = cliente.getNombre();
        std::transform(nombreMinusculas.begin(), nombreMinusculas.end(), nombreMinusculas.begin(), ::tolower);
        if (nombreMinusculas.find(textoBuscado) != std::string::npos) {
            resultado.push_back(cliente);
        }
    }
    return resultado;
}

std::vector<Cliente> GestorClientes::listarTodos() const {
    std::vector<Cliente> resultado;
    resultado.reserve(clientes_.size());
    for (const auto& [id, cliente] : clientes_) {
        resultado.push_back(cliente);
    }
    return resultado;
}

void GestorClientes::cargarClientes(std::vector<Cliente> clientes) {
    clientes_.clear();
    siguienteId_ = 1;
    for (Cliente& cliente : clientes) {
        int id = cliente.getId();
        if (id >= siguienteId_) {
            siguienteId_ = id + 1;
        }
        clientes_.emplace(id, std::move(cliente));
    }
}
