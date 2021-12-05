# TFT_ILI9486 over ESP32
Sample of TFT ILI9486 with touch and SD for ESP32


_Este es un ejemplo de como conectar la pantalla TFT ILI9486 a un microprocesador ESP32 y programarla con Arduino, para su utilización con su versión táctil, incorporando texto, botones e imágenes pre-grabadas en una tarjeta SD._

Este ejemplo está basado en los ejemplos de la librería TFT_eSPI con el link de más abajo, por lo que le doy mis más __sinceros agradecimientos a Bodmer__. 

La motivación para este ejemplo es:

 - Fusionar el uso de botones e imágenes para mostrar un HMI más funcional
 - Uso de SD para almacenar la calibración de la pantalla táctil
 - Corrección de un error de inversión de color en mi montaje
 - Incorporación de un conversor de RGB 24 bits a RGB565 (16 bits)
 - Dar sopoporte al esquema de hardware propuesto para el ESP32 
 

![ILI9486_Ejemplo](https://github.com/OscarCalero/TFT_ILI9486/blob/main/Ejemplo.jpg?raw=true)


## Instrucciones...

Link de YouTube con montaje, programación y funcionamiento :  https://www.youtube.com/watch?v=ZIpGoSoBYxE

Necesitamos:

```
TFT ILI9486 o similar
Controlador (librería) de TFT_eSPI de Bodmer
Una tarjeta SD, que en mi caso de de 8Gb
Un procesador ESP32, aunque otros pueden funcionar igualmente
```
[libreria TFT_eSPI Bodmer](https://github.com/Bodmer/TFT_eSPI)

Recomiendo su montaje en 3 FASES de hardware:

```
1.- conectar TFT
2.- Conectar pantalla táctil (touch) y calibrar grabando en SPIFFS
3.- Conectar la SD con imágenes JPG y su uso como HMI
```
![ILI9486_Montaje](https://github.com/OscarCalero/TFT_ILI9486/blob/main/Montaje.jpg?raw=true)
![ILI9486_Circuit](https://github.com/OscarCalero/TFT_ILI9486/blob/main/Circuit/ILI9486_L.jpg?raw=true)


### Contenidos que podreis ver

```
1.- Conexión de TFT
2.- Varios dispositivos SPI simultaneamente
3.- Uso de SPIFFS (memoria flash del ESP32)
4.- Uso de SD en ESP32
```

### Disfruta...

Oscar
