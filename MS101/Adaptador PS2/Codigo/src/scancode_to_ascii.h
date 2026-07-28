#ifndef SCANCODE_TO_ASCII
#define SCANCODE_TO_ASCII


#include <map>


/**
 * @brief Mapeo estándar de códigos de escaneo (scancodes) PS/2 a caracteres ASCII.
 * 
 * @details Este diccionario relaciona los "make codes" enviados por un teclado PS/2 
 * (configurado en Layout US) con su representación ASCII correspondiente. 
 * Incluye letras en minúscula, números, signos de puntuación base y mapeos 
 * específicos de teclas de función (F1-F11) hacia caracteres de control del 
 * sistema Microsistemas (como US, GS, RS, DC1, etc.).
 */
std::map<uint8_t, char> SCANCODE_TO_ASCII_TABLE = {
  // Letras
  {0x1C, 'a'}, {0x32, 'b'}, {0x21, 'c'}, {0x23, 'd'},
  {0x24, 'e'}, {0x2B, 'f'}, {0x34, 'g'}, {0x33, 'h'},
  {0x43, 'i'}, {0x3B, 'j'}, {0x42, 'k'}, {0x4B, 'l'},
  {0x3A, 'm'}, {0x31, 'n'}, {0x44, 'o'}, {0x4D, 'p'},
  {0x15, 'q'}, {0x2D, 'r'}, {0x1B, 's'}, {0x2C, 't'},
  {0x3C, 'u'}, {0x2A, 'v'}, {0x1D, 'w'}, {0x22, 'x'},
  {0x35, 'y'}, {0x1A, 'z'},
  
  // Números
  {0x45, '0'}, {0x16, '1'}, {0x1E, '2'}, {0x26, '3'},
  {0x25, '4'}, {0x2E, '5'}, {0x36, '6'}, {0x3D, '7'},
  {0x3E, '8'}, {0x46, '9'},

  // Signos (Teclas físicas base directas - Layout US)
  {0x0E, '`'}, {0x4E, '-'}, {0x55, '='}, {0x54, '['}, 
  {0x5B, ']'}, {0x5D, '\\'}, {0x4C, ';'}, {0x52, '\''},
  {0x41, ','}, {0x49, '.'}, {0x4A, '/'},

  // Otras teclas base sumamente importantes
  {0x29, ' '},   // Barra espaciadora
  {0x66, static_cast<char>(8)},   // Backspace --> BS
  {0x5A, static_cast<char>(3)},   // Enter --> ETX
  {0x76, static_cast<char>(27)},  // Escape --> ESC
  {0x0D, static_cast<char>(9)},  // Tabular --> HT
  {0x05, static_cast<char>(31)},  // F1 --> US
  {0x06, static_cast<char>(29)},  // F2 --> GS
  {0x04, static_cast<char>(30)},  // F3 --> RS
  {0x0C, static_cast<char>(17)},  // F4 --> DC1
  {0x03, static_cast<char>(18)},  // F5 --> DC2
  {0x0B, static_cast<char>(19)},  // F6 --> DC3
  {0x83, static_cast<char>(7)},   // F7 --> BEL
  {0x0A, static_cast<char>(127)}, // F8 --> DEL
  {0x83, static_cast<char>(10)},  // F9 --> LF
  {0x83, static_cast<char>(11)},  // F10 --> VT
  {0x83, static_cast<char>(12)},  // F11 --> FF
  
};       


/**
 * @brief Mapeo de scancodes PS/2 a ASCII con la tecla SHIFT presionada (Modificador).
 * 
 * @details Proporciona los caracteres alternativos para los mismos scancodes cuando 
 * el usuario mantiene presionada la tecla Shift. Esto convierte las letras a 
 * mayúsculas y reemplaza la fila de números y teclas de puntuación por sus 
 * respectivos símbolos del Layout US. Las teclas de control y función se 
 * mantienen idénticas a la tabla base.
 */
std::map<uint8_t, char> SCANCODE_SHIFTED_TO_ASCII_TABLE = {

  // Letras (Mayúsculas - Mismos Make Codes)
  {0x1C, 'A'}, {0x32, 'B'}, {0x21, 'C'}, {0x23, 'D'},
  {0x24, 'E'}, {0x2B, 'F'}, {0x34, 'G'}, {0x33, 'H'},
  {0x43, 'I'}, {0x3B, 'J'}, {0x42, 'K'}, {0x4B, 'L'},
  {0x3A, 'M'}, {0x31, 'N'}, {0x44, 'O'}, {0x4D, 'P'},
  {0x15, 'Q'}, {0x2D, 'R'}, {0x1B, 'S'}, {0x2C, 'T'},
  {0x3C, 'U'}, {0x2A, 'V'}, {0x1D, 'W'}, {0x22, 'X'},
  {0x35, 'Y'}, {0x1A, 'Z'},

  // Signos (Reemplazan a los números - Mismos Make Codes)
  {0x45, ')'}, {0x16, '!'}, {0x1E, '@'}, {0x26, '#'},
  {0x25, '$'}, {0x2E, '%'}, {0x36, '^'}, {0x3D, '&'},
  {0x3E, '*'}, {0x46, '('},

  // Signos (Reemplazan a los signos base - Mismos Make Codes)
  {0x0E, '~'}, {0x4E, '_'}, {0x55, '+'}, {0x54, '{'}, 
  {0x5B, '}'}, {0x5D, '|'}, {0x4C, ':'}, {0x52, '"'}, 
  {0x41, '<'}, {0x49, '>'}, {0x4A, '?'}, 
  
  // Otras teclas base sumamente importantes
  {0x29, ' '},   // Barra espaciadora
  {0x66, static_cast<char>(8)},   // Backspace --> BS
  {0x5A, static_cast<char>(3)},   // Enter --> ETX
  {0x76, static_cast<char>(27)},  // Escape --> ESC
  {0x0D, static_cast<char>(9)},  // Tabular --> HT
  {0x05, static_cast<char>(31)},  // F1 --> US
  {0x06, static_cast<char>(29)},  // F2 --> GS
  {0x04, static_cast<char>(30)},  // F3 --> RS
  {0x0C, static_cast<char>(17)},  // F4 --> DC1
  {0x03, static_cast<char>(18)},  // F5 --> DC2
  {0x0B, static_cast<char>(19)},  // F6 --> DC3
  {0x83, static_cast<char>(7)},   // F7 --> BEL
  {0x0A, static_cast<char>(127)}, // F8 --> DEL
  {0x83, static_cast<char>(10)},  // F9 --> LF
  {0x83, static_cast<char>(11)},  // F10 --> VT
  {0x83, static_cast<char>(12)},  // F11 --> FF
};


#endif // SCANCODE_TO_ASCII