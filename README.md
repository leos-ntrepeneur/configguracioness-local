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
│   ├── MetodoPago.h                # Enum Efectivo/TarjetaCredito/TarjetaDebito
│   ├── GestorVentas.h
│   ├── InformacionNegocio.h        # Datos del local para el encabezado del ticket
│   ├── GeneradorTicket.h           # Arma el texto plano del ticket (consola Y GUI)
│   ├── CorteCaja.h                 # Snapshot permanente del resumen de ventas al cerrar el dia
│   ├── GestorCortes.h              # Asigna numero de corte y arma cada CorteCaja
│   ├── GeneradorCorte.h            # Arma el texto plano del corte (consola Y GUI)
│   ├── IRepositorioProductos.h     # Interfaz de persistencia de productos
│   ├── IRepositorioVentas.h        # Interfaz de persistencia de ventas
│   ├── IRepositorioInformacionNegocio.h  # Interfaz de persistencia de InformacionNegocio
│   ├── IRepositorioCortes.h        # Interfaz de persistencia de CorteCaja (solo agrega filas)
│   ├── RepositorioProductosCsv.h   # Implementacion CSV de la interfaz
│   ├── RepositorioVentasCsv.h      # Implementacion CSV de la interfaz
│   ├── RepositorioInformacionNegocioCsv.h  # Implementacion CSV de la interfaz
│   ├── RepositorioCortesCsv.h      # Implementacion CSV append-only de la interfaz
│   ├── Iva.h                       # Calcula subtotal/IVA a partir de un precio YA con IVA incluido
│   ├── Cliente.h                   # Cliente al que se le vende al fiado (a credito)
│   ├── GestorClientes.h            # Dueño de la coleccion de clientes, asigna id secuencial
│   ├── IRepositorioClientes.h      # Interfaz de persistencia de clientes
│   ├── RepositorioClientesCsv.h    # Implementacion CSV de la interfaz
│   ├── Abono.h                     # Pago (parcial o total) de un cliente contra su saldo fiado
│   ├── GestorCreditos.h            # Calcula saldo pendiente por cliente, registra abonos
│   ├── IRepositorioAbonos.h        # Interfaz de persistencia de abonos (solo agrega filas)
│   ├── RepositorioAbonosCsv.h      # Implementacion CSV append-only de la interfaz
│   ├── CsvUtil.h                   # Parseo de lineas CSV (helper compartido)
│   ├── ValidacionTexto.h           # Validacion de texto "seguro para CSV", compartida
│   ├── Menu.h                      # Solo lo usa la consola
│   ├── Utilidades.h                # Solo lo usa la consola
│   └── Excepciones.h
├── src/                         # Implementaciones (.cpp) de lo de arriba + main.cpp (consola)
│   ├── Producto.cpp
│   ├── Inventario.cpp
│   ├── DetalleVenta.cpp
│   ├── Venta.cpp
│   ├── MetodoPago.cpp
│   ├── GestorVentas.cpp
│   ├── InformacionNegocio.cpp
│   ├── GeneradorTicket.cpp
│   ├── CorteCaja.cpp
│   ├── GestorCortes.cpp
│   ├── GeneradorCorte.cpp
│   ├── RepositorioProductosCsv.cpp
│   ├── RepositorioVentasCsv.cpp
│   ├── RepositorioInformacionNegocioCsv.cpp
│   ├── RepositorioCortesCsv.cpp
│   ├── Iva.cpp
│   ├── Cliente.cpp
│   ├── GestorClientes.cpp
│   ├── RepositorioClientesCsv.cpp
│   ├── Abono.cpp
│   ├── GestorCreditos.cpp
│   ├── RepositorioAbonosCsv.cpp
│   ├── CsvUtil.cpp
│   ├── ValidacionTexto.cpp
│   ├── Menu.cpp
│   ├── Utilidades.cpp
│   └── main.cpp
├── gui/                         # Interfaz grafica (Qt Widgets) -- SOLO presentacion,
│   │                             # reutiliza include/src de arriba sin modificarlos
│   ├── include/
│   │   ├── MainWindow.h            # Ventana principal (dueña de Inventario/GestorVentas/repos)
│   │   ├── PestanaProductos.h      # Pestaña "Productos": tabla + alta/edicion/baja
│   │   ├── PestanaVentas.h         # Pestaña "Vender": productos disponibles + carrito
│   │   ├── PestanaReporte.h        # Pestaña "Reporte del dia" (incluye grafica por categoria)
│   │   ├── PestanaInformacionNegocio.h  # Pestaña "Mi negocio": datos para el ticket
│   │   ├── PestanaCortes.h         # Pestaña "Cortes de caja": historial + cerrar el dia
│   │   ├── PestanaArchivoVentas.h  # Pestaña "Archivo de ventas": historial COMPLETO, todos los dias
│   │   ├── PestanaClientes.h       # Pestaña "Clientes": saldo pendiente + registrar abonos
│   │   ├── GraficaBarras.h         # Widget generico de barras horizontales (QPainter, sin QtCharts)
│   │   ├── ProductoDialog.h        # Formulario emergente de alta/edicion (muestra el IVA incluido)
│   │   ├── TicketDialog.h          # Ventana emergente: ticket de venta O corte de caja
│   │   ├── SeleccionarClienteDialog.h  # Elegir/dar de alta un cliente al vender al fiado
│   │   └── TemaOscuro.h            # Aplica paleta + hoja de estilo oscura
│   ├── src/                        # Implementaciones .cpp + main_gui.cpp
│   └── resources/
│       ├── style.qss               # Hoja de estilo (QSS) del tema oscuro
│       ├── resources.qrc           # Empaqueta style.qss/logo/fuentes en el .exe
│       ├── app.rc                  # Icono del .exe en Windows (solo WIN32)
│       ├── icons/                  # logo.svg, logo.png, app.ico
│       ├── fonts/                  # Manrope (.ttf, licencia OFL)
│       └── licenses/               # Licencia de la fuente incrustada
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
- **MetodoPago**: `enum class` con las 4 formas de pago que reconoce el
  sistema (`Efectivo`, `TarjetaCredito`, `TarjetaDebito`, `Fiado`), más las
  funciones libres `metodoPagoATexto`/`textoAMetodoPago` para
  convertir a/desde el texto que se guarda en CSV y se imprime en el
  ticket.
- **Iva**: namespace con `TASA` (16%, la tasa general en México) y las
  funciones `calcularSubtotal`/`calcularMontoIva`, que a partir de un
  precio **ya con IVA incluido** (así se captura siempre — en México el
  precio en el anaquel es el precio final, a diferencia de EE. UU. donde
  el impuesto se suma después) calculan cuánto de ese precio es subtotal y
  cuánto es impuesto. No se guarda ningún campo de IVA en ningún lado: se
  calcula sobre la marcha para mostrarlo como referencia (`ProductoDialog`,
  `Menu`) y para desglosarlo en el ticket/corte (Subtotal + IVA = Total).
- **Venta**: transacción cerrada e inmutable: fecha, **número de
  transacción (folio)**, método de pago, un cliente asociado (`clienteId`/
  `nombreCliente`, solo para ventas `Fiado` — ver `Cliente` más abajo),
  lista de `DetalleVenta` y total calculado. El folio es lo que permite
  distinguir dos ventas del mismo producto entre sí — antes solo existía
  la fecha, así que dos ventas del mismo producto en la misma sesión eran
  indistinguibles en el reporte. El constructor valida que
  `clienteId`/`nombreCliente` sean consistentes con el método de pago
  (una venta al fiado SIEMPRE necesita cliente; cualquier otra NUNCA lo
  tiene) — nunca puede existir una `Venta` con esos datos a medias.
- **GestorVentas**: valida stock suficiente para TODO el pedido antes de
  tocar el inventario, descuenta stock a través de `Inventario`, asigna el
  folio siguiente (correlativo, solo después de que la venta pasa todas
  las validaciones), guarda el historial de ventas y genera el reporte del
  día (`ReporteVentasDia`, que ahora además de los totales por producto
  incluye `transacciones`: una lista con cada venta individual del día —
  folio, hora, método de pago y total).
- **InformacionNegocio**: los datos del local que aparecen en el
  encabezado del ticket (nombre, dirección, teléfono y un RFC que es
  **solo texto de ejemplo para el ticket** — no se valida contra el
  formato real del SAT ni habilita facturación electrónica, ver el
  comentario grande en `InformacionNegocio.h`). Todos los campos pueden
  quedar vacíos (un negocio recién instalado que aún no los ha
  capturado).
- **GeneradorTicket**: función libre `generarTextoTicket(venta, info)` que
  arma el texto plano del ticket (encabezado del negocio, folio, fecha,
  método de pago, cliente si es fiado, detalle, **Subtotal/IVA/Total** y
  total) — vive en el código de NEGOCIO (`include`/`src`, no en `gui/`)
  precisamente para que la consola (`std::cout`) y la GUI (`TicketDialog`)
  impriman **exactamente el mismo texto** sin duplicar ni una línea de
  formato. Un ticket al fiado termina con "PENDIENTE DE PAGO (FIADO)" en
  vez de "Gracias por su compra", para que no se confunda con un
  comprobante de pago.
- **CorteCaja**: una fotografía PERMANENTE del resumen de ventas del día
  en el momento de cerrarlo (también llamado "cierre del día"): número de
  corte, folio inicial/final incluidos, número de transacciones, total
  vendido y el desglose por método de pago (efectivo/tarjeta de
  crédito/tarjeta de débito/**fiado**). A diferencia de `ReporteVentasDia`
  (que se recalcula cada vez y siempre refleja "hoy hasta ahora"), un
  `CorteCaja` queda archivado tal cual estaba al cerrarse — es un registro
  contable, no una consulta en vivo. Solo guarda totales, no el detalle
  por producto/categoría (`ventas.csv` ya tiene eso, folio por folio). El
  monto fiado se muestra aparte y "(pendiente)": todavía no entró a la
  caja, así que el corte también calcula "Cobrado hoy" (total vendido
  menos lo fiado) para no confundirlo con efectivo real.
- **GestorCortes**: le asigna a cada `CorteCaja` su número de corte
  (correlativo, mismo patrón que el folio de `Venta`) y mantiene el
  historial en memoria. `cerrarDia()` arma el corte a partir del
  `ReporteVentasDia` vigente; la persistencia queda a cargo de quien
  llama (`Menu`/`MainWindow`), igual que con `GestorVentas::registrarVenta`.
- **GeneradorCorte**: función libre `generarTextoCorte(corte, info)`,
  misma idea que `GeneradorTicket` pero para el resumen de cierre.
- **Cliente**: un cliente de confianza al que se le vende al fiado —
  id (secuencial, lo asigna `GestorClientes`), nombre y teléfono opcional.
  No guarda ningún saldo: el saldo que debe se calcula siempre a partir de
  sus ventas y sus abonos (ver `GestorCreditos`), nunca de un número
  editado a mano que se pudiera desincronizar.
- **GestorClientes**: dueño de la colección de clientes (mismo patrón que
  `Inventario`, un `std::map` por id).
- **Abono**: un pago (parcial o total) que un cliente hace para reducir su
  saldo fiado — inmutable, igual que `Venta` y `CorteCaja`.
- **GestorCreditos**: calcula `totalFiado`/`totalAbonado`/`saldoPendiente`
  por cliente (sumando `Venta`s al fiado de `GestorVentas` y restando sus
  `Abono`s) y registra abonos nuevos — `registrarAbono` rechaza un monto
  mayor al saldo pendiente, así el saldo nunca queda negativo.
- **IRepositorioProductos / IRepositorioVentas / IRepositorioInformacionNegocio / IRepositorioCortes / IRepositorioClientes / IRepositorioAbonos**:
  interfaces (clases abstractas con métodos virtuales puros) para
  guardar/cargar datos. Implementadas hoy por `RepositorioProductosCsv` /
  `RepositorioVentasCsv` / `RepositorioInformacionNegocioCsv` /
  `RepositorioCortesCsv` / `RepositorioClientesCsv` / `RepositorioAbonosCsv`;
  migrar a SQLite implicaría solo escribir una nueva clase que herede de
  estas interfaces, sin tocar `Menu`, `Inventario` ni `GestorVentas`.
  `IRepositorioCortes`/`IRepositorioAbonos` son distintas de las demás: su
  método es `agregar` (una fila a la vez), no `guardarTodos` (todo el
  archivo reescrito) — un corte o un abono ya registrado nunca se
  modifica, así que la implementación en CSV solo necesita abrir el
  archivo en modo *append*.
- **Menu**: capa de presentación (menús de consola, incluido el flujo de
  carrito para registrar una venta, la pregunta de método de pago, elegir
  o dar de alta un cliente al vender al fiado, la edición de
  `InformacionNegocio`, el cierre/consulta de cortes de caja y la gestión
  de clientes/abonos). No contiene lógica de negocio ni de persistencia,
  solo las invoca y maneja errores con `try/catch`.
- **Excepciones**: `ProductoNoEncontrado`, `CodigoDuplicado`,
  `StockInsuficiente`, `EntradaInvalida`, `FinDeEntrada` — errores de
  negocio como excepciones en vez de códigos de retorno.
- **Utilidades / CsvUtil / ValidacionTexto**: lectura segura de consola,
  parseo de líneas CSV, y validación de texto "seguro para CSV" (sin
  comas ni caracteres de control, dentro de una longitud máxima) —
  compartida entre `Producto` e `InformacionNegocio` para no duplicar la
  misma regla dos veces.

### Clases de la interfaz gráfica (`gui/`)

- **MainWindow**: cumple el mismo papel que `main.cpp` + `Menu` en la
  consola — es dueña de `Inventario`, `GestorVentas`, `GestorCortes`,
  `GestorClientes`, `GestorCreditos`, `InformacionNegocio` y los
  repositorios, arma las 7 pestañas y conecta sus señales
  (`datosModificados()`) para guardar en disco y refrescar automáticamente.
- **PestanaProductos**: tabla de productos + búsqueda + botones
  Nuevo/Editar/Eliminar, con las filas de stock bajo resaltadas.
- **PestanaVentas**: productos disponibles a la izquierda (con un botón
  **"+" por fila** que agrega 1 unidad directo, sin abrir ningún diálogo —
  para el caso más común, "vender 1 de esto", sin tener que seleccionar la
  fila y bajar hasta el botón "Agregar al carrito"), carrito a la derecha,
  con el método de pago como **4 botones tipo interruptor** (Efectivo/
  Tarjeta de crédito/Tarjeta de débito/Fiado, un solo clic para ver y
  cambiar la selección) en vez de una lista desplegable. Si se elige
  Fiado, `SeleccionarClienteDialog` pide un cliente existente o da de alta
  uno nuevo ahí mismo antes de confirmar. Al confirmar, registra la venta
  y muestra el ticket en un `TicketDialog`.
- **SeleccionarClienteDialog**: combo con "-- Nuevo cliente --" + todos
  los clientes existentes; si se elige la opción de nuevo, muestra los
  campos nombre/teléfono y lo da de alta al aceptar. Devuelve el id del
  cliente elegido o recién creado.
- **PestanaReporte**: totales y ranking de productos más vendidos del día,
  una tabla de "Historial de transacciones" (folio, hora, método de pago
  y total de cada venta individual — **doble clic en una fila reabre su
  ticket completo**, para ver exactamente qué se compró) para poder
  distinguir dos ventas del mismo producto entre sí, y una **gráfica de
  ventas por categoría** (`GraficaBarras`) que se repuebla sola cada vez
  que el reporte se actualiza — no hace falta pedirla aparte.
- **PestanaInformacionNegocio** ("Mi negocio"): formulario para capturar
  los datos del local que aparecen en el ticket/corte. Recibe la
  `InformacionNegocio` de `MainWindow` **por referencia** (no una copia):
  al guardar, `PestanaVentas`/`PestanaCortes` ven los datos nuevos de
  inmediato en el siguiente ticket o corte, sin necesidad de reiniciar la
  aplicación.
- **PestanaCortes** ("Cortes de caja"): tabla con el historial de cierres
  ya generados (doble clic en una fila reabre su detalle completo) más un
  botón "Cerrar el día", que pide confirmación (es un registro
  permanente) y muestra el corte recién generado reutilizando
  `TicketDialog`.
- **PestanaArchivoVentas** ("Archivo de ventas"): a diferencia de
  PestanaReporte (solo hoy), lista **todas** las ventas registradas desde
  siempre — ya se guardan completas en `ventas.csv`, esta pestaña es solo
  la vista para revisarlas, con un filtro en vivo por folio o fecha y
  doble clic para reabrir el ticket de cualquiera.
- **PestanaClientes** ("Clientes"): tabla de clientes con su saldo
  pendiente (calculado en vivo, nunca editado a mano), botón "Nuevo
  cliente" y botón "Registrar abono (pago)" sobre el cliente seleccionado.
- **GraficaBarras**: widget genérico de barras horizontales dibujado a
  mano con `QPainter` (degradado teal, igual que los botones) en vez de
  depender del módulo QtCharts, que no siempre viene instalado junto con
  Qt Widgets. No sabe nada de ventas ni categorías — solo recibe pares
  (etiqueta, valor) ya calculados.
- **ProductoDialog**: formulario emergente reutilizado tanto para alta
  como para edición. Junto al campo de precio muestra una etiqueta
  informativa "IVA incluido (16%): $X.XX" (ver `Iva.h`) que se recalcula
  en vivo mientras se escribe — el precio capturado nunca cambia por
  esto, sigue siendo el precio final tal cual se vende.
- **TicketDialog**: ventana emergente de solo lectura que muestra un
  bloque de texto preformateado en fuente monoespaciada, dimensionada con
  `QFontMetrics` para que las 40 columnas siempre quepan sin recortarse.
  Un solo diálogo sirve tanto para el ticket de una venta
  (`GeneradorTicket`) como para el corte de caja (`GeneradorCorte`) — el
  título de la ventana es el único parámetro que cambia.
- **TemaOscuro**: aplica el tema oscuro completo en un solo lugar —
  combina una `QPalette` oscura (para lo que Qt dibuja "a mano", como las
  flechitas de un spinbox o el atenuado de campos deshabilitados) con la
  hoja de estilo `style.qss` (para todo lo demás: colores, bordes,
  esquinas redondeadas). Ver el comentario en `TemaOscuro.h` para el
  porqué de necesitar ambos mecanismos.

Cada pestaña reutiliza `Inventario`/`GestorVentas`/las excepciones de
negocio tal cual, sin ninguna clase nueva de lógica — la única diferencia
con la consola es cómo se piden/muestran los datos.

### Identidad visual

- **Logo**: monograma "IP" en una insignia con degradado (`gui/resources/icons/`),
  diseñado en SVG y exportado a `.ico` multi-resolución para Windows. Se usa
  como icono de ventana/taskbar (`TemaOscuro.cpp`) y como icono del propio
  `.exe` (`gui/resources/app.rc`, solo se compila en Windows).
- **Tipografía**: [Manrope](https://github.com/sharanda/manrope) (licencia
  SIL Open Font License, incrustada como recurso — no hace falta tenerla
  instalada) para títulos y encabezados de pestaña; el texto de datos
  (tablas, formularios) usa la fuente del sistema, más legible a tamaños
  chicos.
- **Color**: un solo acento (teal, `#14b8a6`–`#2dd4bf`) para *todo* lo que
  antes usaba azul — botones de acción primaria (en degradado
  `qlineargradient`, ver `style.qss`), foco de campos, pestaña activa y
  selección de filas en tablas. Antes se usaba un azul para botones y un
  teal distinto para la selección; se unificó a un solo color porque tener
  dos acentos distintos no aportaba nada y el teal es el que mejor contraste
  da sobre el fondo oscuro.

### Ajuste manual de stock

A diferencia de la primera versión, el stock **sí se puede corregir
directamente** al editar un producto (`Producto::setStock`), para casos
como un conteo físico que no coincide con el sistema o mercancía dañada.
Es un ajuste directo, no pasa por `GestorVentas` ni queda registrado como
venta — el código del producto sigue siendo el único campo bloqueado al
editar.

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
- `data/negocio.csv` guarda `InformacionNegocio` en un archivo aparte, de
  una sola fila de datos (más su encabezado) — no tiene sentido un archivo
  con múltiples filas cuando solo existe un registro a la vez.

### Tickets de venta

- Cada venta queda identificada por un **folio** (número de transacción)
  correlativo, asignado por `GestorVentas` solo después de que la venta
  pasa todas las validaciones de stock — así no se "queman" números en
  ventas que fallan. El folio, junto con la fecha/hora exacta y el método
  de pago, es lo que permite separar dos ventas del mismo producto entre
  sí; antes de esto, dos compras seguidas del mismo producto se veían
  mezcladas en el reporte del día.
- Al confirmar una venta (consola o GUI) se pide el **método de pago**
  (Efectivo / Tarjeta de crédito / Tarjeta de débito / Fiado) y se genera
  un **ticket** con `GeneradorTicket::generarTextoTicket`: encabezado con
  los datos del local (tomados de `InformacionNegocio`, capturados en el
  menú "Información del local" de la consola o la pestaña "Mi negocio" de
  la GUI), folio, fecha, método de pago, el detalle de la compra, el
  desglose de IVA y el total. La consola lo imprime con `std::cout`; la
  GUI lo muestra en un `TicketDialog` con fuente monoespaciada.
- El RFC de `InformacionNegocio` es **solo un texto de ejemplo** que se
  imprime en el ticket, igual que en cualquier ticket de tienda física —
  deliberadamente NO se valida contra el formato real del SAT ni habilita
  facturación electrónica (CFDI). Es un campo para que el ticket se vea
  completo, no el punto de partida de un módulo de facturación.
- En la GUI, el método de pago se elige con **4 botones tipo interruptor**
  (`QButtonGroup` exclusivo) en vez de una lista desplegable: la
  selección actual siempre está a la vista y cambia con un solo clic. El
  historial de transacciones (`PestanaReporte`) y el archivo de ventas
  (`PestanaArchivoVentas`) también permiten **doble clic en cualquier
  fila para reabrir el ticket exacto de esa venta**, en vez de solo ver
  folio/hora/total sueltos sin poder revisar qué se compró.

### Ventas por categoría (gráfica automática)

- `GestorVentas::generarReporteDelDia()` agrupa las ventas del día también
  por **categoría** de producto (`ResumenCategoria`), ordenado de mayor a
  menor total vendido. Un producto sin categoría capturada, o que ya se
  eliminó del inventario después de venderse, cae en el cubo "Sin
  categoría".
- Esta agrupación alimenta una **gráfica de barras** que aparece
  automáticamente como parte del reporte, sin que el usuario tenga que
  pedirla aparte: en la consola son barras dibujadas con caracteres de
  texto (`Menu::mostrarGraficaVentasPorCategoria`); en la GUI es el widget
  `GraficaBarras` (dibujado a mano con `QPainter`, con el mismo degradado
  teal que el resto de la interfaz), que se repuebla solo cada vez que
  `PestanaReporte::actualizar()` se ejecuta.

### Cortes de caja (cierre diario)

- Un **corte de caja** (o "cierre del día") es un resumen de ventas
  **permanente**: a diferencia del reporte del día (que se recalcula cada
  vez y siempre refleja "hoy hasta ahora"), un corte queda archivado tal
  cual estaba al momento de cerrarse, con su propio número de corte
  correlativo. Consola: menú "Cerrar el día (corte de caja)". GUI: pestaña
  "Cortes de caja", botón "Cerrar el día".
- Cerrar el día pide **confirmación** primero (es un registro permanente
  que no se puede deshacer y no modifica las ventas ya registradas), y
  después genera el corte a partir del reporte de ventas vigente: folio
  inicial/final incluidos, número de transacciones, total vendido y el
  desglose por método de pago (efectivo / tarjeta de crédito / tarjeta de
  débito). El texto del corte lo arma `GeneradorCorte::generarTextoCorte`
  (mismo formato de 40 columnas que el ticket, reutilizando
  `InformacionNegocio` para el encabezado) y se muestra igual en consola
  (`std::cout`) y en GUI (reutilizando `TicketDialog`).
- Se puede cerrar el día más de una vez en la misma sesión (por ejemplo,
  un corte parcial a medio día y el corte final al terminar) — cada
  llamada genera un registro nuevo e independiente, ninguno modifica ni
  invalida al anterior.
- **Persistencia append-only:** a diferencia de `productos.csv`/
  `ventas.csv` (que se reescriben completos cada vez que algo cambia),
  `data/cortes.csv` solo recibe filas nuevas al final
  (`RepositorioCortesCsv::agregar`, abre el archivo en modo *append*) — un
  corte cerrado es un registro contable, nunca se modifica ni se borra.
  Solo se guardan los totales, no el detalle por producto/categoría/
  transacción individual (`ventas.csv` ya tiene ese detalle completo,
  folio por folio, si algún día hiciera falta reconstruirlo).

### IVA incluido en el precio

- En México el precio que un negocio le pone a un producto **ya incluye
  el IVA** — lo que el cliente ve en el anaquel es lo que paga, sin
  impuesto agregado al cobrar (a diferencia de EE. UU.). Por eso
  `Producto::getPrecio()` nunca cambia ni gana un campo nuevo: sigue
  siendo el precio final tal cual se captura.
- Lo único que se agrega es una forma de **calcular** cuánto de ese precio
  final es impuesto (`Iva::calcularSubtotal`/`calcularMontoIva`, tasa fija
  del 16%): al dar de alta o editar un producto se muestra como
  referencia ("IVA incluido: $X.XX"), y el ticket/corte de caja lo
  desglosan como Subtotal + IVA = Total antes del monto final — mismo
  total de siempre, solo mejor explicado.

### Ventas al fiado y control de clientes

Idea que se les pasa por alto a la mayoría de los sistemas de punto de
venta genéricos, pero que es prácticamente indispensable para una tienda
de abarrotes o ferretería mexicana: vender **al fiado** (a crédito) a
clientes de confianza, cobrando después.

- `MetodoPago::Fiado` es una forma de pago más, con una diferencia clave:
  una venta al fiado necesita un **cliente** asociado (`Cliente`, ver
  `GestorClientes`). Al confirmar una venta al fiado, tanto la consola
  (`Menu::elegirOCrearCliente`) como la GUI (`SeleccionarClienteDialog`)
  piden elegir un cliente ya registrado o dar de alta uno nuevo ahí mismo,
  sin interrumpir el flujo de venta.
- El **saldo pendiente** de un cliente nunca se guarda como número —
  `GestorCreditos::saldoPendiente` lo calcula siempre sumando sus ventas
  al fiado (`GestorVentas`) y restándole sus abonos (`Abono`, pagos
  parciales o totales, inmutables igual que una `Venta`). Así el saldo
  jamás se desincroniza de lo que realmente pasó. `registrarAbono` no
  deja abonar más de lo que se debe, para que el saldo nunca quede en
  negativo.
- El ticket de una venta al fiado muestra el nombre del cliente y termina
  con "PENDIENTE DE PAGO (FIADO)" en vez de "Gracias por su compra" — no
  es un comprobante de que ya se cobró. El corte de caja separa el monto
  fiado del resto ("Fiado (pendiente)") y calcula "Cobrado hoy" (lo que sí
  entró a la caja), para no confundir una venta fiada con dinero en mano.
- Consola: menú "Clientes y ventas al fiado" (dar de alta, ver saldo,
  registrar abono). GUI: pestaña "Clientes".

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

Se abre la ventana con las 7 pestañas (Productos / Vender / Reporte del
día / Mi negocio / Cortes de caja / Archivo de ventas / Clientes). Los
datos se guardan igual que en consola, en `data/productos.csv`,
`data/ventas.csv`, `data/negocio.csv`, `data/cortes.csv`,
`data/clientes.csv` y `data/abonos.csv` junto al ejecutable.

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
- **Costo del producto y margen de utilidad real.** Ahora mismo `Producto`
  solo guarda el precio de VENTA, así que el "Total vendido" del reporte es
  ingreso bruto, no ganancia — no hay forma de saber si el negocio está
  ganando dinero o vendiendo por debajo del costo. Es el hueco más
  importante que le falta a este tipo de software para una PyME real:
  agregar un campo `costo` a `Producto` (con la misma validación que
  `precio`), guardarlo también como snapshot en `DetalleVenta` al momento
  de vender (igual que ya se hace con el precio, para que un cambio de
  costo futuro no reescriba el margen de ventas pasadas), y sumar
  "Utilidad del día" (`total vendido - total costo`) al reporte y al
  ticket interno. Es relativamente chico de implementar con la
  arquitectura actual y es, con diferencia, el dato que más le importa a
  un dueño de negocio día a día.
