# Requisitos
* Compilador de C (necesario si quiere compilar el código de FuzzyCLIPS)
* Python 3.11.9

# Crear el ejecutable de FuzzyCLIPS

Como parte de la entrega se provee un archivo `.exe` con el ejecutable de FuzzyCLIPS. Sin embargo si desea compilarlo se entrega también un fork del código original de FuzzyCLIPS con el que se generará el `.exe`, para **Windows** deberá seguir los siguientes pasos:
* Instalar [Visual Studio](https://visualstudio.microsoft.com/es/downloads/).
* Abrir una terminal de **developer Command Prompt for VS 2022**.
* Ejecutar el comando `cl` en el developer Command Prompt para asegurarse de que haya un compilador de C instalado.
* Desde la terminal Command Promp, ir al folder:   `C:\Users\soto2\OneDrive\Escritorio\fuzzyclips\FuzzyCLIPS Fork\source`
* Ejecutar: `cl /Fefz_clips.exe *.c.` para generar el .exe

# Ejecutar ejemplo FuzzyCLIPS

Para ejecutar el ejemplo de FuzzyCLIPS, se deberá, desde un la carpeta raiz, abrir un **PowerShell** y correr `fz_clips.exe`. Esto levantará una terminal de FuzzyCLIPS, donde deberá cargar el archivo `.clp`, y ejecutar los comandos: 
> (load fzbackward.clp)
> (reset)
> (run)

Esto cargará y ejecutará el programa. Que empezará a preguntar por los valores de cada nodo y el factor de certeza. 
> **fzclips:** which node are you interested in? (a-j or q to quit)  
> **usuario:** f  
> **fzclips:** Please enter a value for node: a  
> **usuario:** 12  
> **fzclips:** Enter Certainty Factor for node (0.0 to 1.0): a  
> **usuario:** 0.4  
> **fzclips:** Please enter a value for node: b  
> **usuario:** 64  
> **fzclips:** Enter Certainty Factor for node (0.0 to 1.0): b  
> **usuario:** 0.9  
> **fzclips:** Enter Certainty Factor for node (0.0 to 1.0): c  
> **usuario:** 34   
> **fzclips:** Enter Certainty Factor for node (0.0 to 1.0): c  
> **usuario:** 0.4  
> **fzclips:**  The output value at node f is 108 with certainty of 0.4  

# Ejecutar ejemplo Python
Para ejecutar el programa en Python, deberá instalar scikit-fuzzy: **`pip install scikit-fuzzy`.** Y seguir los siguientes pasos desde una terminal **PowerShell**:
* Desde el directorio raíz, ir a la carpeta `Python`.
* Ejecutar `python fzbackward.py`.

Esto cargará y ejecutará el programa. Que empezará a preguntar por los valores de cada nodo y el factor de certeza. 
> **python:** which node are you interested in? (a-j or q to quit)  
> **usuario:** f  
> **python:** Please enter a value for node: a   
> **usuario:** 12  
> **python:** Enter Certainty Factor for node (0.0 to 1.0): a  
> **usuario:** 0.4  
> **python:** Please enter a value for node: b  
> **usuario:** 64  
> **python:** Enter Certainty Factor for node (0.0 to 1.0): b  
> **usuario:** 0.9  
> **python:** Enter Certainty Factor for node (0.0 to 1.0): c  
> **usuario:** 34   
> **python:** Enter Certainty Factor for node (0.0 to 1.0): c  
> **usuario:** 0.4  
> **python:**  The output value at node f is 108 with certainty of 0.4  

