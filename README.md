# Inventario POS (C++)

Sistema de consola en C++17 para control de inventario y punto de venta,
pensado para un pequeño negocio (ferretería, abarrotes, refaccionaria).
Proyecto de portafolio orientado a mostrar buenas prácticas de C++ moderno:
POO, separación en archivos `.h`/`.cpp` por clase, STL, `const` correctness,
manejo de errores con excepciones y persistencia detrás de una interfaz.

> **Estado:** los 6 requisitos funcionales completos.
> - Alta, edición y baja de productos (código, nombre, precio, stock,
>   categoría opcional, stock mínimo).
> - Registro de ventas con carrito (agregar/quitar productos, validación de
>   stock disponible en tiempo real) que descuenta el inventario al
>   confirmar.
> - Búsqueda de productos por código exacto o por nombre (coincidencia
>   parcial, sin distinguir mayúsculas/minúsculas).
> - Alertas de stock bajo: marcadas en el listado y la búsqueda, con una
>   vista dedicada y un aviso inmediato si una venta deja un producto por
>   debajo de su mínimo.
> - Reporte de ventas del día: total vendido, número de transacciones y
>   ranking de productos más vendidos (por unidades).
> - Persistencia en CSV (`data/productos.csv`, `data/ventas.csv`): carga
>   automática al iniciar y guardado automático tras cada alta, edición,
>   baja o venta, detrás de una interfaz (`IRepositorioProductos` /
>   `IRepositorioVentas`) para poder migrar a SQLite sin tocar el resto
>   del programa.

## Estructura del proyecto

```
inventario-pos/
├── include/                    # Declaraciones (.h) — el "contrato" de cada clase
│   ├── Producto.h
│   ├── Inventario.h
│   ├── DetalleVenta.h
│   ├── Venta.h
│   ├── GestorVentas.h
│   ├── IRepositorioProductos.h     # Interfaz de persistencia de productos
│   ├── IRepositorioVentas.h        # Interfaz de persistencia de ventas
│   ├── RepositorioProductosCsv.h   # Implementacion CSV de la interfaz
│   ├── RepositorioVentasCsv.h      # Implementacion CSV de la interfaz
│   ├── CsvUtil.h                   # Parseo de lineas CSV (helper compartido)
│   ├── Menu.h
│   ├── Utilidades.h
│   └── Excepciones.h
├── src/                         # Implementaciones (.cpp) + punto de entrada
│   ├── Producto.cpp
│   ├── Inventario.cpp
│   ├── DetalleVenta.cpp
│   ├── Venta.cpp
│   ├── GestorVentas.cpp
│   ├── RepositorioProductosCsv.cpp
│   ├── RepositorioVentasCsv.cpp
│   ├── CsvUtil.cpp
│   ├── Menu.cpp
│   ├── Utilidades.cpp
│   └── main.cpp
├── data/                        # Datos persistidos (productos.csv, ventas.csv);
│                                 # se generan solos al usar el programa, no se
│                                 # versionan en git (ver .gitignore)
├── CMakeLists.txt               # Build con CMake (recomendado para Visual Studio)
├── Makefile                     # Build directo con g++ (MinGW en Windows)
└── README.md
```

### Clases principales

- **Producto**: entidad con código único, nombre, precio, stock, categoría
  opcional y stock mínimo (para alertas). Valida sus propios datos, incluido
  que no contengan comas (necesario para el formato CSV, ver más abajo).
- **Inventario**: dueño de la colección de productos (`std::map` por
  código). Alta/edición/baja, búsquedas, listado, detección de stock bajo y
  carga masiva desde persistencia.
- **DetalleVenta**: una línea de venta (producto, cantidad, precio unitario
  "congelado" al momento de vender).
- **Venta**: transacción cerrada e inmutable: fecha, lista de
  `DetalleVenta` y total calculado.
- **GestorVentas**: valida stock suficiente para TODO el pedido antes de
  tocar el inventario, descuenta stock a través de `Inventario`, guarda el
  historial de ventas y genera el reporte del día (`ReporteVentasDia`).
- **IRepositorioProductos / IRepositorioVentas**: interfaces (clases
  abstractas con métodos virtuales puros) para guardar/cargar datos.
  Implementadas hoy por `RepositorioProductosCsv` / `RepositorioVentasCsv`;
  migrar a SQLite implicaría solo escribir una nueva clase que herede de
  estas interfaces, sin tocar `Menu`, `Inventario` ni `GestorVentas`.
- **Menu**: capa de presentación (menús de consola, incluido el flujo de
  carrito para registrar una venta). No contiene lógica de negocio ni de
  persistencia, solo las invoca y maneja errores con `try/catch`.
- **Excepciones**: `ProductoNoEncontrado`, `CodigoDuplicado`,
  `StockInsuficiente`, `EntradaInvalida`, `FinDeEntrada` — errores de
  negocio como excepciones en vez de códigos de retorno.
- **Utilidades / CsvUtil**: lectura segura de consola y parseo de líneas
  CSV, respectivamente.

### Persistencia: cómo funciona

- Al iniciar, `main.cpp` carga `data/productos.csv` y `data/ventas.csv` si
  existen (si es la primera ejecución, simplemente arranca vacío).
- Después de cada alta, edición o baja de producto, y después de cada
  venta confirmada, el programa guarda automáticamente ambos archivos — no
  hay un botón de "Guardar" porque nunca hace falta presionarlo.
- **Limitación conocida:** el formato CSV usado aquí es simple (sin
  comillas ni escapado), así que nombre/categoría no pueden contener comas
  — `Producto` lo valida y rechaza esa entrada con un mensaje claro. Para
  un negocio real con nombres más complejos, o para no reinventar el
  manejo de archivos, el siguiente paso natural es migrar a SQLite
  implementando `RepositorioProductosSqlite` / `RepositorioVentasSqlite`
  sobre las mismas interfaces.

## Cómo compilar y ejecutar (Windows)

Tienes dos rutas igual de válidas; usa la que ya tengas instalada.

### Opción A: g++ / MinGW (línea de comandos)

1. Instala [MinGW-w64](https://www.mingw-w64.org/) o el toolchain de
   [MSYS2](https://www.msys2.org/) y asegúrate de que `g++` esté en el
   `PATH` (`g++ --version` debe funcionar en una terminal nueva).
2. Desde la carpeta del proyecto, en PowerShell, CMD o Git Bash:

   ```bash
   make
   ```

   Esto genera `bin\inventario_pos.exe`. Ejecútalo con:

   ```bash
   bin\inventario_pos.exe
   ```

   Si no tienes `make` instalado (no viene con MinGW por defecto), compila
   directo con g++ (en Git Bash, donde `src/*.cpp` se expande solo; en CMD
   o PowerShell reemplaza el `*` por la lista de archivos de `src\`):

   ```bash
   g++ -std=c++17 -Wall -Wextra -Iinclude src/*.cpp -o inventario_pos.exe
   ./inventario_pos.exe
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

El ejecutable queda en `build\Release\inventario_pos.exe`. Nota: el
programa crea la carpeta `data\` junto a donde se ejecuta el `.exe` (no
necesitas crearla a mano), así que `data\productos.csv` aparecerá dentro
de `build\Release\` en este caso.

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

## Ideas para seguir extendiendo el proyecto

Estas no forman parte de los requisitos originales, pero son pasos
naturales para seguir mostrando profundidad técnica:

- Migrar `RepositorioProductosCsv`/`RepositorioVentasCsv` a SQLite
  (con [SQLiteCpp](https://github.com/SRombauts/SQLiteCpp) o el API C de
  SQLite directo), implementando las mismas interfaces.
- Reportes por rango de fechas (no solo "hoy"), reutilizando
  `Venta::getFecha()`.
- Multiples usuarios/cajeros con registro de quién hizo cada venta.
- Pruebas unitarias (por ejemplo con
  [Catch2](https://github.com/catchorg/Catch2) o
  [GoogleTest](https://github.com/google/googletest)) para
  `Producto`, `Inventario` y `GestorVentas`, que ya están diseñados sin
  dependencias de consola y por lo tanto son fáciles de probar de forma
  aislada.
