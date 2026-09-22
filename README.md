# Inventario POS (C++)

Sistema en C++17 para control de inventario y punto de venta, pensado para
un pequeño negocio (ferretería, abarrotes, refaccionaria). Proyecto de
portafolio orientado a mostrar buenas prácticas de C++ moderno: POO,
separación en archivos `.h`/`.cpp` por clase, STL, `const` correctness,
manejo de errores con excepciones y persistencia detrás de una interfaz.

**Tiene dos interfaces que comparten exactamente la misma lógica de
negocio** (`Producto`, `Inventario`, `Venta`, `GestorVentas`, persistencia
en CSV — ni una línea de esas clases sabe si el usuario está en una
consola o una ventana):

- **`inventario_pos`** — versión de consola (siempre se puede compilar,
  no requiere nada más que un compilador de C++17).
- **`inventario_pos_gui`** — versión gráfica con Qt Widgets (requiere
  tener Qt6 instalado; si no lo tienes, CMake simplemente omite este
  target y compila solo la consola).

> **Estado:** los 6 requisitos funcionales completos, en ambas interfaces.
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
├── include/                    # NEGOCIO: declaraciones (.h) que usan las DOS interfaces
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
│   ├── Menu.h                      # Solo lo usa la consola
│   ├── Utilidades.h                # Solo lo usa la consola
│   └── Excepciones.h
├── src/                         # Implementaciones (.cpp) de lo de arriba + main.cpp (consola)
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
├── gui/                         # Interfaz grafica (Qt Widgets) -- SOLO presentacion,
│   │                             # reutiliza include/src de arriba sin modificarlos
│   ├── include/
│   │   ├── MainWindow.h            # Ventana principal (dueña de Inventario/GestorVentas/repos)
│   │   ├── PestanaProductos.h      # Pestaña "Productos": tabla + alta/edicion/baja
│   │   ├── PestanaVentas.h         # Pestaña "Vender": productos disponibles + carrito
│   │   ├── PestanaReporte.h        # Pestaña "Reporte del dia"
│   │   ├── ProductoDialog.h        # Formulario emergente de alta/edicion
│   │   └── TemaOscuro.h            # Aplica paleta + hoja de estilo oscura
│   ├── src/                        # Implementaciones .cpp + main_gui.cpp
│   └── resources/
│       ├── style.qss               # Hoja de estilo (QSS) del tema oscuro
│       └── resources.qrc           # Empaqueta style.qss dentro del .exe
├── data/                        # Datos persistidos (productos.csv, ventas.csv);
│                                 # se generan solos al usar el programa, no se
│                                 # versionan en git (ver .gitignore)
├── CMakeLists.txt               # Build con CMake: genera inventario_pos siempre,
│                                 # e inventario_pos_gui si detecta Qt6 instalado
├── Makefile                     # Build directo con g++ de SOLO la consola (MinGW en Windows)
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

### Clases de la interfaz gráfica (`gui/`)

- **MainWindow**: cumple el mismo papel que `main.cpp` + `Menu` en la
  consola — es dueña de `Inventario`, `GestorVentas` y los repositorios,
  arma las 3 pestañas y conecta sus señales (`datosModificados()`) para
  guardar en disco y refrescar automáticamente.
- **PestanaProductos**: tabla de productos + búsqueda + botones
  Nuevo/Editar/Eliminar, con las filas de stock bajo resaltadas.
- **PestanaVentas**: productos disponibles a la izquierda, carrito a la
  derecha (mismo diseño que el flujo de consola, pero con clics).
- **PestanaReporte**: totales y ranking de productos más vendidos del día.
- **ProductoDialog**: formulario emergente reutilizado tanto para alta
  como para edición.
- **TemaOscuro**: aplica el tema oscuro completo en un solo lugar —
  combina una `QPalette` oscura (para lo que Qt dibuja "a mano", como las
  flechitas de un spinbox o el atenuado de campos deshabilitados) con la
  hoja de estilo `style.qss` (para todo lo demás: colores, bordes,
  esquinas redondeadas). Ver el comentario en `TemaOscuro.h` para el
  porqué de necesitar ambos mecanismos.

Cada pestaña reutiliza `Inventario`/`GestorVentas`/las excepciones de
negocio tal cual, sin ninguna clase nueva de lógica — la única diferencia
con la consola es cómo se piden/muestran los datos.

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

## Cómo compilar y ejecutar la versión gráfica (Windows)

La versión gráfica necesita Qt6 instalado. La ruta más confiable es usar
**Qt Creator** (el IDE oficial de Qt) en vez de pelear con la integración
de CMake de Visual Studio.

### 1. Instala Qt

1. Ve a [qt.io/download-qt-installer](https://www.qt.io/download-qt-installer-oss)
   y descarga el **Qt Online Installer** (necesitas crear una cuenta
   gratuita de Qt, es solo un formulario).
2. Al ejecutarlo, en la pantalla de selección de componentes marca:
   - Bajo tu versión de Qt (ej. **Qt 6.8.x**) → **MinGW 64-bit** (el
     compilador viene empaquetado junto con Qt, ya calibrado para
     funcionar entre sí — no hace falta usar el MinGW de MSYS2 aquí).
   - **Qt Creator** normalmente ya viene marcado por defecto bajo
     "Developer and Designer Tools".
3. Instala (va a tardar, son varios GB).

### 2. Abre el proyecto en Qt Creator

1. Abre Qt Creator → **Archivo → Abrir archivo o proyecto...**
2. Selecciona el `CMakeLists.txt` en la raíz de la carpeta del proyecto.
3. Qt Creator va a detectar automáticamente un "Kit" (la combinación de
   compilador + Qt que acabas de instalar) y configurar el proyecto. Dale
   **Configurar proyecto**.

### 3. Ejecuta

1. Abajo a la izquierda, en el selector de "target" activo, elige
   **`inventario_pos_gui`** (no `inventario_pos`, ese es la consola).
2. Presiona el botón verde ▷ (o `Ctrl+R`).

Se abre la ventana con las 3 pestañas (Productos / Vender / Reporte del
día). Los datos se guardan igual que en consola, en `data/productos.csv`
y `data/ventas.csv` junto al ejecutable.

### Alternativa: Visual Studio

También puedes abrir la carpeta del proyecto en Visual Studio (como con
la consola) y seleccionar `inventario_pos_gui.exe` en el desplegable de
elemento de inicio — pero Visual Studio necesita saber dónde quedó
instalado Qt (variable `CMAKE_PREFIX_PATH` apuntando a la carpeta
`.../Qt/6.8.x/mingw_64/lib/cmake`, configurable en el archivo
`CMakePresets.json` o en la configuración de CMake de VS). Si tuviste
problemas para que Visual Studio detectara CMake correctamente (ver el
historial de este proyecto), **Qt Creator es el camino recomendado** — es
el IDE que Qt mantiene específicamente para que esto funcione sin fricción.

## Validación de datos y "hardening" (QA de seguridad)

Además de las validaciones obvias (campos vacíos, precios negativos), el
proyecto pasó por una revisión enfocada en **qué podría romper el sistema
o corromper datos**, no solo en qué se ve mal. Todo vive en un solo punto
de control — el constructor y los setters de `Producto` — para que sea
imposible crear un producto inválido sin importar si la entrada viene de
la consola, la GUI, o un archivo `productos.csv` editado a mano:

- **Límites de negocio explícitos** (`Producto::PRECIO_MAXIMO`,
  `Producto::STOCK_MAXIMO`): sin un tope superior, una cantidad vendida
  muy grande sumada al carrito podía **desbordar un `int`** (comportamiento
  indefinido en C++, no solo "un número raro") y, en el peor caso teórico,
  terminar *aumentando* el stock en vez de descontarlo. Se cerró en dos
  capas: un tope de negocio real (ningún producto tiene más de un millón
  de unidades) y aritmética en `long long` en los puntos donde se suman
  cantidades (`Menu::agregarAlCarrito`, `PestanaVentas::alAgregarAlCarrito`,
  `GestorVentas::registrarVenta`).
- **`NaN`/`Infinity` como precio**: `std::stod("nan")` y `std::stod("inf")`
  son válidos según el estándar de C++ — un `productos.csv` editado a mano
  con un precio así se cargaba sin error y contaminaba silenciosamente
  cualquier total que lo incluyera (una comparación con `NaN` siempre da
  falso, así que ni el chequeo de "no negativo" lo atajaba). Ahora
  `Producto` exige `std::isfinite(precio)`.
- **Inyección en el archivo CSV**: un nombre/categoría con una coma o un
  salto de línea rompía la estructura de filas y columnas del archivo.
  Se valida que ningún campo de texto contenga comas ni caracteres de
  control.
- **Límite de longitud** en código/nombre/categoría, para que un dato
  absurdamente largo no infle el archivo ni rompa el layout de la tabla.

Verificado con ataques reales, no solo revisión de código: craftié un
`productos.csv` a mano con filas de precio `nan`/`inf`, stock de 99
millones y nombres con bytes de control, y confirmé que el programa las
descarta silenciosamente sin corromper el resto del inventario; y
reproduje el escenario exacto de desbordamiento (crear un producto al
límite de stock y pedir una cantidad de 2,000,000,000 en una sola
operación) para confirmar que ahora se rechaza con un mensaje claro en
vez de comportamiento indefinido.

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
