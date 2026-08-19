# Combinador Video Compuesto

<img width="500" alt="IMG_3362" src="https://github.com/user-attachments/assets/778247d4-eb4d-40ad-9f38-be4f234cb07d" />


## Principio de funcionamiento

La placa de video de las computadoras MS101 y MS104 funcionan generando 3 señales:

* Iluminación (3V)
* (VSYNC) Sincronización vertical de 50Hz (3.8V)
* (HSYNC) Sincronización Horizontal de 15.625Khz (3.65V)

La función de la placa combinadora es la de sumar las 3 señales para generar un video compuesto estandar PAL. Para poder lograrlo no solo hay que modificar las amplitudes de las señales, sino que tambien (en el caso de la sincronización horizontal) había que modificar su ancho de pulso. El objetivo era lograr una señal resultante de 1Vpp, que tenga la siguiente forma


<img width="800" alt="videopradao" src="https://github.com/alexpal03/Lab-Arqueologia-Computacional/blob/main/MS104/Combinador%20Video%20Compuesto/Imagenes%20utiles/videopadrao.png" />

Una parte clave del combinador era lograr que el pulso de HSYNC quede invertido, y con un ancho de pulso de 4.7us. Para ello, se utilizó un circuito RC temporizador que mediante regulación con un trimpot, modificaría la constante tau y permitiría ajustar a un ancho de 4.7us dejando la frecuencia intacta. Una vez conseguido esto, los valores de front porch y back porch se acomodaron solos.

Por otro lado, para el control del vertical, en el video compuesto tradicional se sustituye cada 20ms (50Hz) el pulso HSYNC por un pulso más largo, indicando que es momento de sincronizar el vertical. Esta sustitución se puede lograr mediante una operación lógica de AND entre el pulso horizontal y vertical. A esta señal resultante de la operación HSYNC & VSYNC se la conoce como CSYNC (Sincronización compuesta)


<img width="800" alt="videopradao" src="https://github.com/alexpal03/Lab-Arqueologia-Computacional/blob/main/MS104/Combinador%20Video%20Compuesto/Imagenes%20utiles/csync_and.png" />


Una vez conseguido esto, solo es necesario sumar la señal CSYNC con la iluminación para obtener el video compuesto. La iluminación debe quedar con 0.7Vpp y el CSYNC con 0.3Vpp. Cabe aclarar que el transistor que se ocupa de realizar esta última suma debe ser un transistor de alta frecuencia, de lo contrario la señal de iluminación puede verse alterada. En esta placa se utilizó el [KPS10](https://www.alldatasheet.es/datasheet-pdf/pdf/53500/FAIRCHILD/KSP10.html)




