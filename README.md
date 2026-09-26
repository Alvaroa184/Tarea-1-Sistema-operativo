# Tarea 1 - Sistemas Operativos

Creación de una shell básica en C para Sistemas Operativos.

## Autores
- Felipe Sepúlveda
- Álvaro Anabalon
- Gerhec Parra

## Archivos del proyecto

- `mishell.c`: contiene el funcionamiento principal de la shell.
- `pipes.c` / `pipes.h`: contienen la implementación de pipes.
- `jobs.c` / `jobs.h`: contienen el manejo de los procesos en segundo plano.
- `pmon.c` / `pmon.h`: contienen la implementación del monitor de procesos.
- `signals.c` / `signals.h`: contienen el manejo de señales.
- `Makefile`: permite compilar el proyecto utilizando `make`.
 ```text
Tarea-1-Sistema-operativo/
├── mishell.c
├── pipes.c
├── pipes.h
├── jobs.c
├── jobs.h
├── pmon.c
├── pmon.h
├── signals.c
├── signals.h
├── Makefile
└── .gitignore
```

## Requisitos

Para compilar y ejecutar el programa se necesita:

- Linux o WSL
- GCC
- Make

## Cómo usar la shell
Para poder usar la shell primero necesitamos compilarla.
Ingrese a la carpeta del proyecto desde la terminal y ejecutar el comando:
```bash
make
```
El Makefile compilará los archivos necesarios y generará el ejecutable de la shell.
Una vez compilado el proyecto podrá ser ejecutado con:
```bash
./mishell
```
Al iniciar la shell aparecerá un prompt similar a:
```text
miShell:/ruta/actual$
```
La ruta mostrada corresponde al directorio actual desde donde se está utilizando la shell.
A partir de aquí se pueden escribir los diferentes comandos como por ejemplo:
```bash
ls
```

```bash
pwd
```

```bash
echo Hola
```
Para finalizar la ejecución de la shell se utiliza:
```bash
exit
```
O también se puede usar:
```bash
exit 0
```
## Comandos disponibles

La shell permite ejecutar comandos normales de Linux como:

```bash
ls
pwd
echo Hola
```

Además, cuenta con los siguientes comandos internos:

### cd
Permite cambiar el directorio actual de la shell.

```bash
cd carpeta
```

Si se utiliza sin especificar un directorio, se dirige al directorio HOME.

```bash
cd
```

### exit
Permite finalizar la ejecución de la shell.

```bash
exit
```

También se puede especificar un código de salida.

```bash
exit 0
```

### jobs
Muestra los procesos que se están ejecutando en segundo plano.

```bash
jobs
```

### pmon
Permite monitorear los procesos en segundo plano iniciados desde la shell.

```bash
pmon
```

También se puede especificar cada cuántos segundos se actualiza la información.

```bash
pmon 2
```

Para salir de `pmon` se utiliza `Ctrl+C`.

## Operadores disponibles

### Redirección de entrada y salida

Para redirigir la salida de un comando a un archivo:

```bash
echo Hola > archivo.txt
```

Para agregar la salida al final de un archivo:

```bash
echo Hola >> archivo.txt
```

Para utilizar un archivo como entrada:

```bash
sort < archivo.txt
```

### Pipes

Se pueden conectar varios comandos utilizando `|`.

```bash
ls | grep ".c" | wc -l
```

### Procesos en segundo plano

Para ejecutar un comando en segundo plano se utiliza `&` al final:

```bash
sleep 30 &
```

También se puede ejecutar una pipeline completa en segundo plano:

```bash
ls | grep ".c" &
```
## Limpiar compilación

Para eliminar los archivos generados durante la compilación se puede utilizar:

```bash
make clean
```

Para volver a compilar el programa:

```bash
make
```

