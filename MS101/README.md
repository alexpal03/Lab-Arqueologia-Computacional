# MS101

<img width="500" alt="ms101" src="https://github.com/user-attachments/assets/5bd9ae7f-4513-4b29-8a37-7b152b2ba4c6" />

# Mañas y soluciones a problemas

### La MS101 muestra rayas erráticas, muestra las letras borrosas, o no muestra NADA?
Generalmente esto no es un problema de la computadora en sí, sino que es un problema de algún retorno de masa que falla (creo yo) en el monitor. A veces se soluciona dándole algún "golpesito" en la tapa, o apagando la máquina, dejandola enfriar unos minutos y volviéndola a encender. Si ninguno de estos sofisticados métodos funciona, se tendrá que recurrir a:
1) Apagar la máquina y abrir la tapa
2) Desatornillar el rack de tarjetas
3) Remover la tarjeta de memoria RAM
4) Encender la máquina, esperar unos segundos, y hacer movimientos sútiles en el rack hasta que la imágen mejore. Debería verse la típica pantalla de caracteres aleatorios debido a la falta de memoria RAM.
5) Volver a atornillar el rack, puede hacerse con mucho cuidado con la máquina encendida para ver que la imagen no vuelva a perderse.
6) Apagar y volver a encender la máquina un par de veces para ver que la imagen se mantenga.
7) Recolocar la tarjeta RAM con la máquina apagada
8) Encender y el problema debería estar resuelto

### La MS101 muestra `ERR.MEM.DIR` / `EXCED.CAPACID.TECLADO`

Esto es posiblemente un problema de falso contacto en la tarjeta RAM / CPU. Se recomienda apagar la compu, y recolocar la tarjeta RAM y la tarjeta de CPU.
Si luego de reiterados intentos el problema no se soluciona, puede que estemos ante una falla en algún integrado de memoria RAM. Se puede probar con la tarjeta RAM moderna diseñada en FAMAF y ver si el problema se soluciona.


### La MS101 muestra caracteres aleatorios estáticos

Apagar la máquina y recolocar RAM y CPU. Si no se soluciona, posiblemente el problema sea de un integrado de memoria RAM.



### La MS101 muestra 2 caracteres repetidos, o un patrón detectable de caracteres en movimiento. 

El peor error posible. Puede significar un problema en alguna ROM 2708. Se puede probar recolocarlas en sus zócalos y ver si se soluciona. 
Tambíen puede significar un cortocircuito en la tarjeta de teclado y disco. Un integrado muy delicado que tira ese tipo de error cuando se rompe es el UPD372.


### Regla de oro
*Cualquier recolocación de tarjetas se debe hacer con la máquina apagada*

