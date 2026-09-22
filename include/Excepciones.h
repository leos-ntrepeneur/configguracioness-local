// Excepciones.h
//
// NOTA PARA QUIEN VIENE DE OTROS LENGUAJES:
// C++ no tiene una clase "Exception" base obligatoria como Java/C#, pero la
// biblioteca estandar (STL) ofrece <stdexcept> con clases como
// std::runtime_error. Aqui heredamos de ella para crear excepciones propias
// del dominio (errores de negocio), en vez de inventar nuestro propio sistema
// de codigos de error.
//
// Estas clases son "header-only" (todo el codigo vive en el .h) porque son
// muy pequenas; no hace falta un .cpp separado para ellas.

#ifndef EXCEPCIONES_H
#define EXCEPCIONES_H

#include <stdexcept>
#include <string>

// Se lanza cuando se busca un producto por codigo y no existe.
class ProductoNoEncontrado : public std::runtime_error {
public:
    explicit ProductoNoEncontrado(const std::string& codigo)
        : std::runtime_error("Producto no encontrado con codigo: " + codigo) {}
};

// Se lanza al intentar dar de alta un producto cuyo codigo ya existe.
class CodigoDuplicado : public std::runtime_error {
public:
    explicit CodigoDuplicado(const std::string& codigo)
        : std::runtime_error("Ya existe un producto con codigo: " + codigo) {}
};

// Se lanza al intentar vender mas unidades de las que hay en stock.
class StockInsuficiente : public std::runtime_error {
public:
    StockInsuficiente(const std::string& codigo, int disponible, int solicitado)
        : std::runtime_error(
              "Stock insuficiente para '" + codigo + "'. Disponible: " +
              std::to_string(disponible) + ", solicitado: " + std::to_string(solicitado)) {}
};

// Se lanza cuando el usuario captura datos invalidos (precio negativo, etc).
class EntradaInvalida : public std::runtime_error {
public:
    explicit EntradaInvalida(const std::string& mensaje)
        : std::runtime_error(mensaje) {}
};

// Se lanza cuando la entrada estandar llega a su fin (EOF), por ejemplo si
// el programa se ejecuta con datos redirigidos desde un archivo/pipe y este
// se agota. Hereda de std::exception (no de runtime_error) para dejar claro
// que no es un error de negocio sino una señal de "hay que cerrar". En
// Menu.cpp la atrapamos con un `catch (const FinDeEntrada&)` ANTES del
// `catch (const std::exception&)` generico -- en C++ los catch se prueban en
// orden y el primero cuyo tipo coincida (o sea clase base del lanzado) gana,
// asi que el orden de los catch importa tanto como el de un if/else if.
class FinDeEntrada : public std::exception {
public:
    const char* what() const noexcept override {
        return "Fin de la entrada (EOF). Cerrando el programa.";
    }
};

#endif // EXCEPCIONES_H
