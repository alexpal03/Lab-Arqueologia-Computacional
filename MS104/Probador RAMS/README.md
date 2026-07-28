# Testeador de RAM de 64kbytes para MS101 - MS104

El testeador tiene capacidad de verificar hasta 64kbytes de memoria RAM, pero lo hace por sectores conformados por 8kbytes.
En total hay 8 sectores.

La decisión de usar esta delimitación de sectores es debido al mapeo de memoria de la 101-104:

|Sector| Direcciones |
| ---- | --------- |
|  1   |0000 - 1FFF|
|  2   |2000 - 3FFF|
|  3   |4000 - 5FFF|
|  4   |6000 - 7FFF|
|  5   |8000 - 9FFF|
|  6   |A000 - BFFF|
|  7   |C000 - DFFF|
|  8   |E000 - FFFF|


## Modo de uso

1. Conectar el testeador de ram en el algún espacio del rack de tarjetas, preferiblemene en el espacio del CPU.
2. Hacer doble click en el botón para entrar a la configuración, donde se podrá seleccionar el sector a testear. Para moverse por las opciones se debe hacer un solo click, y una vez que se seleccione el sector deseado se vuelve a presionar doble click para volver al inicio.
3. Una vez en el inicio, se comienza el test con un solo click.
4. Al finalizar el test, aparece la primera pantalla de resultado, que indica la cantidad de direcciones donde hubo errores de lectura. Con un click se pasa a la siguiente pantalla, donde se muestra con mayor detalle los bits que tuvieron fallas organizados en bloques de 1kbyte. Ejemplo:

    ```
    7F 00 00 00
    00 50 00 00
    ```
    El `7F` que en binario es `01111111`, significa que en el primer bloque de 1kbyte fallaron todos los bits a excepción del más significativo.
    El `50` que en binario es `01010000` significa que en el sexto bloque de 1kbyte solo hay fallas en el B6 y B4.

    Esta subdivisión es útil porque tanto los integrados de RAM de la 101 y la 104 son de 1024 direcciones. De esta forma se puede localizar mucho más fácil la falla. Ej: si todos los bloques de 1kbyte fallan en el mismo bit, es probable que la falla sea del buffer de datos, y no de las RAM.
