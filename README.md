# Nombre: Francisco Hermani

# Padrón: 98223

Link GitHub: https://github.com/franhermani/taller-tp1

# Resolución del TP

### TDA Socket

Mi resolución del TP comenzó con la implementación del TDA Socket en su totalidad.

Para ello, leí las diapositivas, vi la clase grabada nuevamente e hice uso de
documentaciones online y del comando *man*.

Básicamente, lo que hice fue ir testeando las funciones desde el lado del cliente
y del servidor; pero no entre ellos, sino de cada uno contra una instancia de *netcat*.

Una vez que estuve satisfecho con el comportamiento de ambos por separado,
procedí a testearlos en conjunto, hasta afinar los detalles que fueron surgiendo.

En el momento en que mi par <cliente-servidor> se enviaba mensajes correctamente
comencé con la implementación del protocolo D-Bus.

### Protocolo D-Bus

Comencé leyendo atentamente el enunciado e investigando en la Web, pero mi principal
herramienta de trabajo para entender el protocolo fue el video subido por Matías
al grupo de Google.

Pausándolo a cada byte nuevo, empecé creando todos los *structs* que componen el protocolo.
Así, logré una estructura de *structs* anidados que será explicada más adelante.

Luego, procedí a escribir funciones para completar dichos *structs*, de forma
modularizada y ordenada, para un mejor seguimiento.

Para debuggear que la tira de bytes se estuviera escribiendo correctamente,
armé una función que imprimía byte a byte en código hexadecimal, de modo tal
que pude analizar todos los casos borde de padding y demás.

Una vez que la tira de bytes se armaba y enviaba correctamente, procedí a decodificarla
desde el lado del servidor.

Para ello, reutilicé los *structs* creados, pero creé nuevas funcionas para
interpretar la tira de bytes según el protocolo.

### Refactorización

Una vez corroborado el correcto funcionamiento del TP, con los casos de pruebas
provistos por el SERCOM, pasé a una etapa final de refactorización general,
la cual incluyó:
- Modularización de funciones
- Correcta asignación de responsabilidades
- Creación de las entidades *client* y *server* como tales, con sus atributos
  y funciones correspondientes
- Lectura del archivo de entrada en bloques de 32 bytes, haciendo uso
  del TDA Dynamic Buffer
- Redimensionamiento de la tira de bytes creada por el cliente a medida
  que se van agregando elementos a ella
- Creación del Endianness Converter, para transformar enteros de little
  a big endian, y viceversa

# Puntos de interés



# Aclaraciones

