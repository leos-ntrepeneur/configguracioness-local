# Inventario POS (C++)

Sistema de consola en C++17 para control de inventario y punto de venta,
pensado para un pequeño negocio (ferretería, abarrotes, refaccionaria).
Proyecto de portafolio orientado a mostrar buenas prácticas de C++ moderno:
POO, separación en archivos `.h`/`.cpp` por clase, STL, `const` correctness
y manejo de errores con excepciones.

> **Estado actual:** Requisito 1 completo (alta, edición y baja de
> productos, con búsqueda/listado y alerta de stock bajo integradas en el
> listado). Los siguientes requisitos (ventas, reportes, persistencia en
> archivo) se agregan de forma incremental.

## Estructura del proyecto

```
inventario-pos/
├── include/            # Declaraciones (.h) — el "contrato" de cada clase
│   ├── Producto.h
│   ├── Inventario.h
│   ├── Menu.h
│   ├── Utilidades.h
│   └── Excepciones.h
├── src/                # Implementaciones (.cpp) + punto de entrada
│   ├── Producto.cpp
│   ├── Inventario.cpp
│   ├── Menu.cpp
│   ├── Utilidades.cpp
│   └── main.cpp
├── data/               # Archivos de datos (CSV) — se agrega en el
│                        # requisito de persistencia
├── CMakeLists.txt      # Build con CMake (recomendado para Visual Studio)
├── Makefile            # Build directo con g++ (MinGW en Windows)
└── README.md
```

### Clases principales

- **Producto**: entidad con código único, nombre, precio, stock, categoría
  opcional y stock mínimo (para alertas). Valida sus propios datos.
- **Inventario**: dueño de la colección de productos (`std::map` por
  código). Alta/edición/baja, búsquedas, listado y detección de stock bajo.
- **Menu**: capa de presentación (menús de consola). No contiene lógica de
  negocio, solo la invoca y maneja errores con `try/catch`.
- **Excepciones**: `ProductoNoEncontrado`, `CodigoDuplicado`,
  `StockInsuficiente`, `EntradaInvalida`, `FinDeEntrada` — errores de
  negocio como excepciones en vez de códigos de retorno.
- **Utilidades**: lectura segura de enteros/doubles/líneas desde consola.

## Cómo compilar y ejecutar (Windows)

Tienes dos rutas igual de válidas; usa la que ya tengas instalada.

### Opción A: g++ / MinGW (línea de comandos)

1. Instala [MinGW-w64](https://www.mingw-w64.org/) o el toolchain de
   [MSYS2](https://www.msys2.org/) y asegúrate de que `g++` esté en el
   `PATH` (`g++ --version` debe funcionar en una terminal nueva).
2. Desde la carpeta del proyecto, en PowerShell o CMD:

   ```bash
   make
   ```

   Esto genera `bin\inventario_pos.exe`. Ejecútalo con:

   ```bash
   bin\inventario_pos.exe
   ```

   Si no tienes `make` instalado (no viene con MinGW por defecto), compila
   directo con g++:

   ```bash
   g++ -std=c++17 -Wall -Wextra -Iinclude src\*.cpp -o inventario_pos.exe
   inventario_pos.exe
   ```

### Opción B: Visual Studio (con CMake)

1. Abre Visual Studio 2022 con el componente **"Desarrollo para escritorio
   con C++"** instalado (incluye soporte de CMake).
2. `Archivo → Abrir → Carpeta...` y selecciona la carpeta del proyecto
   (la que contiene `CMakeLists.txt`). Visual Studio detecta el proyecto
   CMake automáticamente y genera la configuración.
3. Selecciona `inventario_pos.exe` como elemento de inicio y presiona
   **Ctrl+F5** (ejecutar sin depurar) o **F5** (con depurador).

También puedes generar un `.sln` clásico con CMake desde línea de comandos
si lo prefieres:

```bash
cmake -S . -B build -G "Visual Studio 17 2022"
cmake --build build --config Release
```

El ejecutable queda en `build\Release\inventario_pos.exe`.

### Linux / macOS

```bash
make
./bin/inventario_pos
```

o con CMake:

```bash
cmake -S . -B build
cmake --build build
./build/inventario_pos
```

## Próximos pasos (roadmap del proyecto)

2. Registro de ventas (selección de productos, cálculo de total, descuento
   automático de inventario).
3. Consulta de inventario (listar/buscar por nombre o código) — ya cubierto
   parcialmente por el listado de productos.
4. Alertas de stock bajo — ya integradas en el listado; se ampliarán con
   una vista dedicada.
5. Reporte de ventas del día (total vendido, productos más vendidos,
   número de transacciones).
6. Persistencia en archivo (CSV), dejando el diseño preparado para migrar
   a SQLite más adelante.
