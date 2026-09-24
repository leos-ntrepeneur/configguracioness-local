cmake_minimum_required(VERSION 3.15)
project(InventarioPOS CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra)
endif()

# Fuentes de NEGOCIO: no dependen de la consola ni de Qt, las usan los dos
# ejecutables (version de consola y version grafica). Separarlas de las
# fuentes de cada interfaz es lo que permite tener dos "caras" distintas
# del mismo programa sin duplicar ni una sola regla de negocio.
set(CORE_SOURCES
    src/Producto.cpp
    src/Inventario.cpp
    src/DetalleVenta.cpp
    src/Venta.cpp
    src/MetodoPago.cpp
    src/GestorVentas.cpp
    src/CsvUtil.cpp
    src/ValidacionTexto.cpp
    src/RepositorioProductosCsv.cpp
    src/RepositorioVentasCsv.cpp
    src/InformacionNegocio.cpp
    src/RepositorioInformacionNegocioCsv.cpp
    src/GeneradorTicket.cpp
    src/CorteCaja.cpp
    src/GestorCortes.cpp
    src/RepositorioCortesCsv.cpp
    src/GeneradorCorte.cpp
    src/Iva.cpp
    src/Cliente.cpp
    src/GestorClientes.cpp
    src/RepositorioClientesCsv.cpp
    src/Abono.cpp
    src/GestorCreditos.cpp
    src/RepositorioAbonosCsv.cpp
)

set(CONSOLE_SOURCES
    src/Menu.cpp
    src/Utilidades.cpp
    src/main.cpp
)

add_executable(inventario_pos ${CORE_SOURCES} ${CONSOLE_SOURCES})
target_include_directories(inventario_pos PRIVATE include)

# La version grafica es OPCIONAL: si quien compila no tiene Qt6 instalado,
# CMake simplemente omite ese target y sigue generando la version de
# consola sin problema (QUIET evita que falle la configuracion si no lo
# encuentra).
find_package(Qt6 COMPONENTS Widgets QUIET)

if(Qt6_FOUND)
    file(GLOB GUI_SOURCES "gui/src/*.cpp")
    # Los .h con Q_OBJECT tambien se listan como "fuente" del target (CMake
    # no los compila como .cpp, solo los usa para rastrear dependencias):
    # es lo que le permite a AUTOMOC encontrarlos de forma confiable,
    # sobre todo aqui donde viven en una carpeta distinta (gui/include) a
    # sus .cpp (gui/src).
    file(GLOB GUI_HEADERS "gui/include/*.h")

    # AUTOMOC debe activarse ANTES de add_executable: CMake lee esta
    # variable en ese momento para decidir si el target necesita el paso
    # extra de `moc` (la herramienta de Qt que genera el codigo C++ real
    # detras de señales/slots, a partir de la macro Q_OBJECT). AUTORCC hace
    # lo mismo para los archivos .qrc: los compila a codigo C++ que
    # empaqueta el contenido de style.qss dentro del propio ejecutable.
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)

    # El icono del .exe (visible en el Explorador de archivos de Windows,
    # no solo dentro del programa) es un mecanismo exclusivo de Windows via
    # un archivo .rc -- no existe equivalente en Linux/macOS, por eso se
    # agrega solo "if(WIN32)". CMake compila un .rc automaticamente con
    # windres (MinGW) o rc.exe (MSVC) con solo listarlo como fuente, igual
    # que un .cpp.
    if(WIN32)
        set(GUI_SOURCES ${GUI_SOURCES} gui/resources/app.rc)
    endif()

    add_executable(inventario_pos_gui ${CORE_SOURCES} ${GUI_SOURCES} ${GUI_HEADERS} gui/resources/resources.qrc)
    target_include_directories(inventario_pos_gui PRIVATE include gui/include)
    target_link_libraries(inventario_pos_gui PRIVATE Qt6::Widgets)

    message(STATUS "Qt6 encontrado: se generara tambien el target 'inventario_pos_gui'.")
else()
    message(STATUS "Qt6 no encontrado: solo se generara la version de consola 'inventario_pos'. "
                    "Instala Qt6 (ver README) y vuelve a configurar para habilitar la version grafica.")
endif()
