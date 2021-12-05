# TFT_ILI9486 over ESP32
Sample of TFT ILI9486 with touch and SD for ESP32

_Este es un ejemplo de como conectar la pantalla TFT ILI9486 a un microprocesador ESP32 y programarla con Arduino, para su utilización con su versión táctil, incorporando texto, botones e imágenes pre-grabadas en una tarjeta SD._

![ILI9486_Ejemplo](https://github.com/OscarCalero/TFT_ILI9486/blob/main/Ejemplo.jpg?raw=true)


## Instrucciones...

Link de YouTube con montaje, programación y funcionamiento :  ---

Necesitamos:

```
TFT ILI9486 o similar
Controlador (librería) de TFT_eSPI de Bodmer: ![libreria TFT_eSPI Bodmer](https://github.com/Bodmer/TFT_eSPI)
Una tarjeta SD, que en mi caso de de 8Gb
Un procesador ESP32, aunque otros pueden funcionar igualmente
```

Recomiendo su montaje en 3 FASES de hardware:

```
1.- conectar TFT
2.- Conectar pantalla táctil (touch) y calibrar grabando en SPIFFS
3.- Conectar la SD con imágenes JPG y su uso como HMI
```
![ILI9486_Montaje](https://github.com/OscarCalero/TFT_ILI9486/blob/main/Montaje.jpg?raw=true)


### Contenidos que podreis ver

```
1.- Conexión de TFT
2.- Varios dispositivos SPI simultaneamente
3.- Uso de SPIFFS (memoria flash del ESP32)
4.- Uso de SD en ESP32
```

### Disfruta...

Oscar
