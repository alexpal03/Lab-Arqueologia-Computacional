#include <Arduino.h>
#include "ascii_to_microsistemas.h"
#include "scancode_to_ascii.h"

// --- Declaración de funciones (Requisito para C++ en PlatformIO) ---
void PS2Interrupt();

// --- Definición de Pines ---
// Pines de datos (8 bits en paralelo para protocolo Microsistemas)
const int DATA_PINS[8] = {PB9, PB8, PB7, PB6, PB4, PB3, PA15, PA12};

// Pin de STROBE (Señal para avisarle al otro equipo que lea los datos)
const int STB_PIN = PA11;

// Pines del teclado PS/2
const int PS2_CLK = PB10;
const int PS2_DATA = PB11;

// --- Variables Globales para lectura de datos por PS/2 ---
uint8_t bitCount = 0;
uint8_t scancodeRecibido = 0;
bool teclaLista = false;

// Bandera de estado para el bucle principal
bool ignorarSiguienteByte = false;

// Bandera de SHIFT presionado
bool shiftPresionado = false;

// Bandera de bloq Mayusc activado
bool bloqMayuscActivado  = true; // inicia activado

/**
 * @brief Inicializa el hardware y las comunicaciones.
 * 
 * @details Configura los pines GPIO del bus de datos paralelo como salidas, así como 
 * la señal de STROBE. Prepara los pines del bus PS/2 (reloj y datos) en modo open-drain 
 * y gestiona la secuencia de inhibición/liberación del puerto para estabilizar el teclado 
 * al arranque. Finalmente, vincula la interrupción de hardware al pin de reloj del PS/2.
 * 
 * @return void
 */
void setup()
{
    // Configurar pines de datos como salida
    for (int i = 0; i < 8; i++)
    {
        pinMode(DATA_PINS[i], OUTPUT);
        digitalWrite(DATA_PINS[i], LOW);
    }

    // Configurar STROBE
    pinMode(STB_PIN, OUTPUT);
    digitalWrite(STB_PIN, LOW);

    // Configuración de pines para PS/2 (Requieren Pull-Up externa)
    // Se configuran como salida, pero al ser open drain se puede usar 
    // como entrada
    pinMode(PS2_CLK, OUTPUT_OPEN_DRAIN);
    pinMode(PS2_DATA, OUTPUT_OPEN_DRAIN);


    // Se inhibe el puerto
    digitalWrite(PS2_DATA, HIGH);
    digitalWrite(PS2_CLK, LOW);

    // Espero que se estabilize la 101
    delay(3000);

    // Se libera el puerto
    digitalWrite(PS2_CLK, HIGH);

    // Adjuntamos la interrupción al PB10 en el flanco de bajada
    attachInterrupt(digitalPinToInterrupt(PS2_CLK), PS2Interrupt, FALLING);
}



/**
 * @brief Bucle principal de procesamiento de teclado y conversión de protocolos.
 * 
 * @details Procesa de forma asíncrona los scancodes capturados por la interrupción. 
 * Gestiona la lógica del protocolo PS/2 (ignorando los bytes de liberación 0xF0), 
 * actualiza las banderas de estado (Shift, Bloq Mayús) y mapea los códigos a caracteres ASCII. 
 * Finalmente, convierte el ASCII al estándar "Microsistemas", escribiéndolo en el bus 
 * paralelo seguido de un pulso de STROBE.
 * 
 * @return void
 */
void loop()
{
    // Procesamos la tecla fuera de la interrupción para no bloquear el procesador
    if (teclaLista) {
        
        // --- Manejo del protocolo PS/2 ---
        if (scancodeRecibido == 0xF0) {
            // Es el código de tecla soltada. Activamos bandera para procesar el siguiente byte.
            ignorarSiguienteByte = true;
        } 
        else if (ignorarSiguienteByte) {
            // Este es el byte que se está soltando (después del 0xF0)
            
            // Apagamos la bandera del Shift
            if (scancodeRecibido == 0x12 || scancodeRecibido == 0x59) {
                shiftPresionado = false;
            }
            
            // Ya procesamos la liberación de la tecla, bajamos la bandera general
            ignorarSiguienteByte = false;
        } 
        else if (scancodeRecibido < 128) { // Prevención de desbordamiento de índice
            
            // --- Detección de Shift Presionado ---
            if (scancodeRecibido == 0x12 || scancodeRecibido == 0x59) {
                shiftPresionado = true;
            } 
            // --- Detección de Bloq Mayusc ---
            else if (scancodeRecibido == 0x58) {
                bloqMayuscActivado != bloqMayuscActivado;
            }
            else {
                // --- Conversión Lógica para Teclas Comunes ---
                unsigned char ascii = 0;
                
                // Elegimos la tabla de acuerdo a si el Shift está presionado
                if (shiftPresionado) {
                    ascii = SCANCODE_SHIFTED_TO_ASCII_TABLE[scancodeRecibido];
                } else {
                    ascii = SCANCODE_TO_ASCII_TABLE[scancodeRecibido];
                }
                
                if (ascii != 0) { // Si es una tecla imprimible válida


                    // Lógica del bloq mayusc
                    if (bloqMayuscActivado) {
                        // Minusc a mayusc
                        if (ascii >= 'a' && ascii <= 'z'){
                            ascii = ascii - 'a' + 'A' ;
                        }
                        // Mayusc a minuscs
                        else if (ascii >= 'A' && ascii <= 'Z'){
                            ascii = ascii - 'A' + 'a' ;
                        }
                    }
                    
                    unsigned char codigoMicrosistemas = ASCII_TO_MICROSISTEMAS_TABLE[ascii];
                    
                    // --- Salida de Hardware (Escritura Paralela) ---
                    for (int i = 0; i < 8; i++)
                    {
                        // Comprobamos cada bit de la variable codigoMicrosistemas usando máscaras
                        if (codigoMicrosistemas & (1 << i))
                        {
                            digitalWrite(DATA_PINS[i], HIGH);
                        }
                        else
                        {
                            digitalWrite(DATA_PINS[i], LOW);
                        }
                    }

                    // Pulso de STROBE (Flanco de subida)
                    digitalWrite(STB_PIN, HIGH);
                    delayMicroseconds(100); 
                    digitalWrite(STB_PIN, LOW);
                }
            }
        }
        
        // Limpiamos las variables para la siguiente lectura de la interrupción
        scancodeRecibido = 0;
        teclaLista = false;
    }
}
/**
 * @brief Rutina de Servicio de Interrupción (ISR) para el reloj del teclado PS/2.
 * 
 * @details Se ejecuta automáticamente en cada flanco de bajada (FALLING) del pin 
 * asociado al reloj (CLK). Captura la trama de datos bit a bit, desplazando y 
 * ensamblando los 8 bits útiles del scancode mientras ignora el bit de inicio, 
 * paridad y parada. Al completar los 11 bits del frame, activa la bandera `teclaLista`.
 * 
 * @note Esta función está diseñada para ser lo más rápida posible para no bloquear 
 * el procesador y evitar la pérdida de ciclos de reloj del teclado.
 * 
 * @return void
 */
void PS2Interrupt() {
    // Leemos el estado del pin de datos
    int dataBit = digitalRead(PS2_DATA);
    
    // El bit 0 es inicio, 1-8 son datos, 9 es paridad, 10 es parada
    if (bitCount > 0 && bitCount < 9) {
        scancodeRecibido |= (dataBit << (bitCount - 1));
    }
    
    bitCount++;
    
    if (bitCount == 11) {
        teclaLista = true; // Avisamos al loop principal que hay un dato
        bitCount = 0;      // Reiniciamos el contador
    }
}