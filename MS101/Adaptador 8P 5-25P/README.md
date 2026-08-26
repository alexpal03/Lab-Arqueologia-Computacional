# Adaptador 8P 5-25P

<img width="500" alt="adapt" src="https://github.com/user-attachments/assets/882366fe-f1f1-46ee-9036-e41c49de2d2f" />

## Mas información

Este proyecto está basado en la publicación del blog DAVES OLD COMPUTER, esquematicos, diagramas, etc, en este [LINK](http://dunfield.classiccmp.org/img42841/cnct.htm)

<img width="558" height="297" alt="image" src="https://github.com/user-attachments/assets/70bbba42-c12b-4758-8fea-cb9b3c150044" />


Luego buscando con la IA, me citó esta otra [página interesante](https://www.retrocmp.de/fdd/8inch/general.htm?utm_source=chatgpt.com) donde el autor comenta como calcular la velocidad real de la disquetera en RPM de acuerdo a la frecuencia de la linea.


## Posición de los Jumpers de la disquetera.

La disqueterea debe tener los jumpers en una posición específica para que funciones correctamente
La información se obtuvo viendo los esquemáticos del [manual de Microsistemas](https://github.com/alexpal03/Lab-Arqueologia-Computacional/blob/main/MS101/Manuales/AR_ALAC_JS_0016.pdf), y del [manual oficial de la disquetera Shuggart](https://github.com/alexpal03/Lab-Arqueologia-Computacional/blob/main/MS101/Manuales/SA800-801_Diskette_Drive_OEM_Manual_1977_Shugart.pdf)

<img width="1269" height="787" alt="image" src="https://github.com/user-attachments/assets/4ad38c81-f182-447b-9f5e-be186ae8ef10" />

Los jumpers que deben estar conectados se listan:

* Y (maneja el encendido del led)
* A B X (lógica de carga del Head Load Solenoide)
* HL (Habilitación del motor junto con el Head Load)
* 800 (Modo de operación de la disquetera)
* DS1 (Selección del driver 1)
* T2 T3 T4 T5 T6 (Terminaciones Pull-Up necesarias)

Todos los demas no se deberían conectar Es importante que los jumpers T1, C, DS se encuentren desconectados!!!
A continuación se adjuntan fotos modelo.

<img width="500" alt="IMG_6070" src="https://github.com/user-attachments/assets/aba09084-9291-4bde-b239-b822e6ff9895" />
<img width="500" alt="IMG_6068" src="https://github.com/user-attachments/assets/ab60e7cc-1eb8-423f-8142-1cfddde92674" />
<img width="500" alt="IMG_6066" src="https://github.com/user-attachments/assets/37a85235-2b12-4803-8ef8-5050b4476800" />


## Puesta en marcha de la CopyStation para 8''

1) Conectar los +24V. En este momento los +24V se deben conectar con una fuente externa. La disquetera también necesita +5V, pero estos se obtienen desde la fuente interna de la PC.

  <img width="350" alt="IMG_6065" src="https://github.com/user-attachments/assets/79ed377d-73f2-497c-9b58-573b5b1953bb" />

2) Conectar correctamente el adaptador de cables, como se ve en la imagen.

  <img width="500" alt="image" src="https://github.com/user-attachments/assets/7f438592-f1d9-4640-9276-33072d267b6f" />

  
3) Acostar la disquetera, ya que por efectos de la gravedad solo funciona si se pone de forma horizontal. Se recomienda poner un cartón para no dañar la tarjeta PCB. **Nota**: *tener cuidado al mover la disquetera, intentar levantarla y no arrastrarla ya que se esto puede tirar de los pines de los jumpers y es muy facil romperlos*.
   
4) Conectar el motor AC a 220V.

  <img width="350" alt="IMG_6071" src="https://github.com/user-attachments/assets/b2d89dab-1a32-48a4-a229-983ed12eb766" />

   
5) Encender la PC, entrar a la BIOS y seleccionar en **Legacy Deskette A** la opción *[360K , 5.25 in.]*. Guardar las configuraciones. Esto se debe hacer siempre que se encienda la PC ya que no tiene pila.

  <img width="500" alt="IMG_6072" src="https://github.com/user-attachments/assets/ac959612-aae0-4ed4-810f-1360c86294f0" />

   
6) Cargar el sistema operativo FreeDOS seleccionar la distribución "FreeDOS with JEMMEX (more compatible)"

    
7) Entrar a
   ```
   cd lab-arq\imd
   imd
   ```
8) Se abrirá la ventana de IMD. Setear de la siguiente forma el IMD para leer discos de 8''.

  <img width="500" alt="image" src="https://github.com/user-attachments/assets/e5c509f2-1b1f-44f9-a54e-ae521efb8a39" />

    
9) Leer la [documentación de Ricardo](https://docs.google.com/document/d/1I7-CFlRigsyXyWVRShV5vskSVcw7a0GdWd633pnnbB0/edit?tab=t.0#heading=h.8ebktycz53rb) sobre el uso del IMD.

## Puesta en marcha de la CopyStation para 5.25''

1) Se debe abrir la PC y reconectar el cable plano a la disquetera de 5.25''. La disquetera con la que se probóoriginalmente fue con al primera de arriba. **Conectar el lado GND con el plano de masa de la disquetera**.


  <img width="500" alt="image" src="https://github.com/user-attachments/assets/2f2e2db1-13b1-432e-b6af-ddb21a891892" />


2) Leer la [documentación de Ricardo](https://docs.google.com/document/d/1I7-CFlRigsyXyWVRShV5vskSVcw7a0GdWd633pnnbB0/edit?tab=t.0#heading=h.8ebktycz53rb) sobre el uso del IMD con discos de 5.25''.
