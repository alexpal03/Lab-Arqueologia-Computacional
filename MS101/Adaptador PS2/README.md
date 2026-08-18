# Interfaz de Teclado PS/2 a Protocolo Paralelo Microsistemas

<img width="500" alt="B2E86E17-FF16-48EB-AA83-2C2A9AF8E818" src="https://github.com/user-attachments/assets/81200773-7a56-4c53-b267-295a9c731830" />


Este proyecto implementa un conversor de hardware y software diseñado para leer eventos de un teclado estándar PS/2, interpretarlos, y traducirlos a un bus de datos paralelo de 8 bits propietario (estándar "Microsistemas"). 

El sistema gestiona la recepción asíncrona por interrupciones, decodifica los *scancodes* (incluyendo el estado de modificadores como Shift y Bloq Mayús), los traduce al estándar ASCII, y finalmente mapea dicho ASCII a la codificación binaria requerida por el hardware de destino.



## Nota:
Las teclas de funciones especiales y los shift no fueron nunca probados. Se presupone que el código funcionará. En todo caso, se debería preparar una disquetera para la MS101, correrle el sistema operativo, y luego probar que todas las funcionalidades se ejecuten correctamente.

---

## 🏗️ Arquitectura del Sistema

El flujo de procesamiento se divide en tres etapas principales:

1. **Capa Física (PS/2):** Lectura del protocolo serie síncrono mediante interrupciones de hardware.
2. **Capa Lógica (Máquina de estados):** Filtrado de *scancodes*, manejo de liberación de teclas (`0xF0`) y modificadores.
3. **Capa de Traducción (Pipeline):** `Scancode -> ASCII -> Binario Microsistemas -> Salida Paralela`.

---

## ⚡ 1. Protocolo PS/2 y Recepción por Hardware

El teclado PS/2 se comunica mediante dos líneas en configuración *Open-Drain* (Reloj y Datos). El microcontrolador actúa como receptor pasivo.

### Rutina de Interrupción (ISR)
La recepción se maneja en la función `PS2Interrupt()`, la cual está anclada al flanco de bajada (`FALLING`) del pin de reloj (`PS2_CLK`). 
El protocolo PS/2 envía tramas de **11 bits** por cada evento de teclado:
*   **1 bit de inicio** (siempre 0).
*   **8 bits de datos** (el *Scancode*, LSB primero).
*   **1 bit de paridad** (impar).
*   **1 bit de parada** (siempre 1).

La ISR ignora los bits de inicio, paridad y parada. Desplaza los 8 bits útiles hacia la variable `scancodeRecibido` usando bitwise (`scancodeRecibido |= (dataBit << (bitCount - 1))`). Al alcanzar los 11 bits, levanta la bandera `teclaLista` para cederle el control al bucle principal sin bloquear el procesador.

---

## 🧠 2. Procesamiento Lógico (Bucle Principal)

El `loop()` evalúa la bandera `teclaLista`. Si hay un scancode listo, pasa por una máquina de estados para determinar la acción correcta:

*   **Key Release (0xF0):** Los teclados PS/2 envían `0xF0` seguido del scancode de la tecla cuando esta se suelta. El código enciende la bandera `ignorarSiguienteByte` para no imprimir la letra dos veces. Si la tecla soltada es *Shift* (`0x12` o `0x59`), apaga la bandera `shiftPresionado`.
*   **Modificadores:** Si se presiona *Shift*, se enciende su bandera. Si se presiona *Bloq Mayús* (`0x58`), se invierte el estado booleano de `bloqMayuscActivado`.
*   **Gestión de Mayúsculas/Minúsculas:** Si se presiona una letra y el Bloq Mayús está activo, el sistema aplica aritmética de caracteres C++ (`ascii - 'a' + 'A'`) para invertir el *case* dinámicamente antes de la traducción final.

---

## 🔄 3. Pipeline de Tablas de Conversión

Una vez que se obtiene una tecla válida, atraviesa dos diccionarios de conversión:

1. **`SCANCODE_TO_ASCII_TABLE` (y versión Shifted):** Utiliza la estructura `std::map` para traducir el valor crudo del teclado a un código ASCII. Las teclas de función (F1-F11) están mapeadas intencionalmente a caracteres de control ASCII (0-31) no imprimibles (ej. F1 mapea a `US` [31]).
2. **`ASCII_TO_MICROSISTEMAS_TABLE`:** Es un *array* de 128 posiciones. Usa el valor ASCII obtenido como índice de acceso directo `[ascii]` (O(1)) para recuperar el byte final. 

El mapeo de Microsistemas tiene una estructura semántica en sus bits más significativos (MSB):
*   Los números inician con `0b10...`
*   Las mayúsculas inician con `0b11...`
*   Las minúsculas inician con `0b01...`

---

## 🔌 4. Salida de Hardware (Bus Paralelo)

El byte resultante (`codigoMicrosistemas`) se escribe en los pines GPIO. 
Se utiliza una máscara de bits `(codigoMicrosistemas & (1 << i))` dentro de un bucle `for` de 8 iteraciones para encender o apagar los pines definidos en el arreglo `DATA_PINS`.

Finalmente, se envía un pulso de validación (*Strobe*):
1.  Se levanta el pin `STB_PIN` a `HIGH`.
2.  Se aplica un retardo de `100 microsegundos` para cumplir con los tiempos de *setup/hold* del receptor.
3.  Se baja a `LOW`.

---

## 📊 Tabla de Mapeo Completa: Scancode a Microsistemas

A continuación se detalla el recorrido exacto de transformación de cada tecla, número y función desde el teclado físico hasta la salida binaria de 8 bits.

### 🔢 Números
| Scancode PS/2 (Hex) | ASCII Base (Char / Valor) | Tecla Microsistemas | Salida Binaria Microsistemas |
| :--- | :--- | :--- | :--- |
| **0x45** | `'0'` (48) | Número 0 | `0b10100000` |
| **0x16** | `'1'` (49) | Número 1 | `0b10100001` |
| **0x1E** | `'2'` (50) | Número 2 | `0b10100010` |
| **0x26** | `'3'` (51) | Número 3 | `0b10100011` |
| **0x25** | `'4'` (52) | Número 4 | `0b10100100` |
| **0x2E** | `'5'` (53) | Número 5 | `0b10100101` |
| **0x36** | `'6'` (54) | Número 6 | `0b10100110` |
| **0x3D** | `'7'` (55) | Número 7 | `0b10100111` |
| **0x3E** | `'8'` (56) | Número 8 | `0b10101000` |
| **0x46** | `'9'` (57) | Número 9 | `0b10101001` |

### 🔤 Letras Minúsculas
| Scancode PS/2 (Hex) | ASCII Base (Char / Valor) | Tecla Microsistemas | Salida Binaria Microsistemas |
| :--- | :--- | :--- | :--- |
| **0x1C** | `'a'` (97) | Letra a | `0b01110100` |
| **0x32** | `'b'` (98) | Letra b | `0b01111011` |
| **0x21** | `'c'` (99) | Letra c | `0b01111100` |
| **0x23** | `'d'` (100)| Letra d | `0b01111101` |
| **0x24** | `'e'` (101)| Letra e | `0b01111110` |
| **0x2B** | `'f'` (102)| Letra f | `0b01111111` |
| **0x34** | `'g'` (103)| Letra g | `0b01010110` |
| **0x33** | `'h'` (104)| Letra h | `0b01010111` |
| **0x43** | `'i'` (105)| Letra i | `0b01100010` |
| **0x3B** | `'j'` (106)| Letra j | `0b01100100` |
| **0x42** | `'k'` (107)| Letra k | `0b01100101` |
| **0x4B** | `'l'` (108)| Letra l | `0b01100110` |
| **0x3A** | `'m'` (109)| Letra m | `0b01100111` |
| **0x31** | `'n'` (110)| Letra n | `0b01011011` |
| **0x44** | `'o'` (111)| Letra o | `0b01100011` |
| **0x4D** | `'p'` (112)| Letra p | `0b01010100` |
| **0x15** | `'q'` (113)| Letra q | `0b01101010` |
| **0x2D** | `'r'` (114)| Letra r | `0b01010001` |
| **0x1B** | `'s'` (115)| Letra s | `0b01010101` |
| **0x2C** | `'t'` (116)| Letra t | `0b01010010` |
| **0x3C** | `'u'` (117)| Letra u | `0b01100001` |
| **0x2A** | `'v'` (118)| Letra v | `0b01011010` |
| **0x1D** | `'w'` (119)| Letra w | `0b01010000` |
| **0x22** | `'x'` (120)| Letra x | `0b01011001` |
| **0x35** | `'y'` (121)| Letra y | `0b01010011` |
| **0x1A** | `'z'` (122)| Letra z | `0b01011000` |

### 🅰️ Letras Mayúsculas (Requieren Modificador Shift / Bloq Mayús)
| Scancode PS/2 (Hex) | ASCII Shifted (Char / Valor) | Tecla Microsistemas | Salida Binaria Microsistemas |
| :--- | :--- | :--- | :--- |
| **0x1C** *(+ Shift)* | `'A'` (65) | Letra A | `0b11110100` |
| **0x32** *(+ Shift)* | `'B'` (66) | Letra B | `0b11111011` |
| **0x21** *(+ Shift)* | `'C'` (67) | Letra C | `0b11111100` |
| **0x23** *(+ Shift)* | `'D'` (68) | Letra D | `0b11111101` |
| **0x24** *(+ Shift)* | `'E'` (69) | Letra E | `0b11111110` |
| **0x2B** *(+ Shift)* | `'F'` (70) | Letra F | `0b11111111` |
| **0x34** *(+ Shift)* | `'G'` (71) | Letra G | `0b11010110` |
| **0x33** *(+ Shift)* | `'H'` (72) | Letra H | `0b11010111` |
| **0x43** *(+ Shift)* | `'I'` (73) | Letra I | `0b11100010` |
| **0x3B** *(+ Shift)* | `'J'` (74) | Letra J | `0b11100100` |
| **0x42** *(+ Shift)* | `'K'` (75) | Letra K | `0b11100101` |
| **0x4B** *(+ Shift)* | `'L'` (76) | Letra L | `0b11100110` |
| **0x3A** *(+ Shift)* | `'M'` (77) | Letra M | `0b11100111` |
| **0x31** *(+ Shift)* | `'N'` (78) | Letra N | `0b11011011` |
| **0x44** *(+ Shift)* | `'O'` (79) | Letra O | `0b11100011` |
| **0x4D** *(+ Shift)* | `'P'` (80) | Letra P | `0b11010100` |
| **0x15** *(+ Shift)* | `'Q'` (81) | Letra Q | `0b11101010` |
| **0x2D** *(+ Shift)* | `'R'` (82) | Letra R | `0b11010001` |
| **0x1B** *(+ Shift)* | `'S'` (83) | Letra S | `0b11010101` |
| **0x2C** *(+ Shift)* | `'T'` (84) | Letra T | `0b11010010` |
| **0x3C** *(+ Shift)* | `'U'` (85) | Letra U | `0b11100001` |
| **0x2A** *(+ Shift)* | `'V'` (86) | Letra V | `0b11011010` |
| **0x1D** *(+ Shift)* | `'W'` (87) | Letra W | `0b11010000` |
| **0x22** *(+ Shift)* | `'X'` (88) | Letra X | `0b11011001` |
| **0x35** *(+ Shift)* | `'Y'` (89) | Letra Y | `0b11010011` |
| **0x1A** *(+ Shift)* | `'Z'` (90) | Letra Z | `0b11011000` |

### 🛠️ Teclas de Control y Sistema
| Scancode PS/2 (Hex) | ASCII Base (Char / Valor) | Tecla / Acción Microsistemas | Salida Binaria Microsistemas |
| :--- | :--- | :--- | :--- |
| **0x29** | `SPACE` (32) | Espacio | `0b11011111` |
| **0x5A** (Enter) | `ETX` (3) | **SKIP** | `0b11011100` |
| **0x66** (Backspace) | `BS` (8) | **RESET** | `0b11110010` |
| **0x0D** (Tab)| `HT` (9) | **RIGHT ADJUST** | `0b11110101` |
| **0x76** (Escape)| `ESC` (27) | **NEW LINE** | `0b11110110` |
| **0x0A** (F8)| `DEL` (127) | **FIELD COR** | `0b11001000` |
| **0x05** (F1)| `US` (31) | **CHR BCK** | `0b11011110` |
| **0x06** (F2)| `GS` (29) | **FIELD BCK** | `0b11110000` |
| **0x04** (F3)| `RS` (30) | **REC BCK** | `0b11111000` |
| **0x0C** (F4)| `DC1` (17) | **CHR ADV** | `0b11001010` |
| **0x03** (F5)| `DC2` (18) | **FIELD ADV** | `0b11001100` |
| **0x0B** (F6)| `DC3` (19) | **REC ADV** | `0b11101100` |
| **0x83** (F7)| `BEL` (7) | **HEX** | `0b11000011` |
| **0x83** (F9)| `LF` (10) | **FUNC SEL** | `0b11000001` |
| **0x83** (F10)| `VT` (11) | **DUP** | `0b11001001` |
| **0x83** (F11)| `FF` (12) | **SEL PROG** | `0b11001101` |

