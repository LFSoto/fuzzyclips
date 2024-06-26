[![N|Solid](https://images.credly.com/size/400x400/images/0e3e46ee-69d9-46e6-98b7-282f193e2c94/blob.png)](https://nodesource.com/products/nsolid)
# Maestría Profesional en Ingeniería del Software con énfasis en Inteligencia Artificial
## Curso: Sistemas Basados en Conocimiento

**Profesor:**
- Luis Gerardo León Vega

**Estudiantes:**
- Luis Felipe Soto
- Alexander Garro

# Instrucciones de instalación y ejecución
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

Para ejecutar el ejemplo de FuzzyCLIPS, se deberá, desde un la carpeta raiz, abrir un **PowerShell** y correr `cd FuzzyCLIPS`, luego `fz_clips.exe`. Esto levantará una terminal de FuzzyCLIPS, donde deberá cargar el archivo `.clp`, y ejecutar los comandos (respetando paréntesis):  
> (load fzbackward.clp)  
> (reset)  
> (run)  

Esto cargará y ejecutará el programa. Que empezará a preguntar por los valores de cada nodo y el factor de certeza.  
Ejemnplo:  
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
> **fzclips:**  The output value at node f is 110 with certainty of 0.4  

# Ejecutar ejemplo Python
Para ejecutar el programa en Python, deberá instalar scikit-fuzzy: **`pip install scikit-fuzzy`.** Y seguir los siguientes pasos desde una terminal **PowerShell**:
* Desde el directorio raíz, ir a la carpeta `Python`.
* Ejecutar `python fzbackward.py`.

Esto cargará y ejecutará el programa. Que empezará a preguntar por los valores de cada nodo y el factor de certeza.  
Ejemnplo:  
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
> **python:**  The output value at node f is 110 with certainty of 0.4  

# Comparación entre FuzzyCLIPS y Python

|                        | FuzzyCLIPS                                                                                             | skfuzzy (Phython)                                                                                                                                   |
|------------------------|--------------------------------------------------------------------------------------------------------|-------------------------------------------------------------------------------------------------------------------------------------------|
| **Flexibilidad**       | - Muy flexible en cuanto a la definición de conjuntos difusos y reglas.                                 | - Altamente flexible y extensible, gracias a estar construido sobre Python.                                                               |
|                        | - Permite una gran personalización en cómo se definen y aplican las reglas.                             | - Integración con otras bibliotecas de Python para análisis de datos, aprendizaje automático y visualización.                            |
|                        | - Diseñado específicamente para sistemas basados en reglas.                                             | - Permite definir una variedad amplia de funciones y operadores difusos personalizados.                                                  |
| **Intuitividad**       | - Curva de aprendizaje alta para quienes no están familiarizados con los sistemas de producción basados en reglas. | - La sintaxis de Python es generalmente considerada intuitiva y fácil de aprender.                                                        |
|                        | - Requiere un conocimiento previo de CLIPS, lo cual puede no ser intuitivo para todos los usuarios.   | - La documentación y los ejemplos disponibles facilitan el aprendizaje y la implementación.                                               |
| **Complejidad**        | - La implementación de sistemas puede volverse compleja cuando hay muchas reglas y las interacciones entre ellas son complicadas. | - La manera en que se gestionan los datos y las reglas en Python puede simplificar el manejo de la complejidad.                           |
|                        | - Requiere una gestión cuidadosa de la base de conocimientos y la inferencia.                          | - La capacidad de usar herramientas de depuración y análisis de Python puede ayudar a manejar la complejidad inherente en los sistemas difusos. |
| **Rendimiento**        | - Optimizado para sistemas expertos basados en reglas y puede ser muy eficiente en estos contextos.    | - Al estar basado en Python, puede aprovechar las optimizaciones de NumPy y SciPy para manejo de datos y cálculos matemáticos.               |
|                        | - Puede no ser tan rápido como skfuzzy en el manejo de grandes conjuntos de datos debido a limitaciones por su entorno y arquitectura. | - El rendimiento puede variar y puede ser necesario optimizar el código para aplicaciones específicas.                                    |


