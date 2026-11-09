# Simulación inicial con Helios

Ejemplo didáctico en C++17. Construye 16 plantas simplificadas de ocho hojas rectangulares cada una sobre un suelo de 2.2 × 2.2 m. No representa una especie calibrada ni simula crecimiento. Los experimentos cambian la elevación solar, manteniendo la geometría fija.

## Archivos

| Archivo | Función |
| --- | --- |
| `src/escena.h` | Parámetros y construcción de hojas y suelo en el Context de Helios. |
| `src/main.cpp` | Exportación de geometría, simulación de radiación y vista 3D opcional. |
| `CMakeLists.txt` | Compilación en modo `geometria` o `radiacion`. |
| `graficar.py` | Lectura del CSV real y exportación de una gráfica PNG. |

## Requisitos

- Git, CMake y compilador C++17. En Windows, instalar Visual Studio 2022 con la carga **Desarrollo para el escritorio con C++** y usar **Developer PowerShell for VS 2022**. Git Bash por sí solo no instala un compilador.
- Una copia completa de [Helios](https://github.com/PlantSimulationLab/Helios). La comprobación de API se hizo con el commit `39f7f0d6fece051495d5b60413bce599adfdfb67`, consultado el 9 de noviembre de 2026. Consultar también sus requisitos actualizados.
- Para la ventana 3D: dependencias de Visualizer y una sesión gráfica con OpenGL, según la instalación oficial.
- Para radiación: un backend compatible de Helios, Vulkan u OptiX. CUDA se usa con OptiX/NVIDIA; no es necesaria para la ejecución con Vulkan. Poder generar geometría sin ventana no comprueba que el equipo soporte radiación.
- Python y Matplotlib solo para la gráfica, no para ejecutar los programas C++.

## Windows

Desde una carpeta de trabajo, descargar Helios y este proyecto como carpetas hermanas:

```powershell
git clone https://github.com/PlantSimulationLab/Helios.git
git clone --branch feat/simulacion-helios https://github.com/UlisesACC/SS-Escom.git
cd SS-Escom
```

Los comandos siguientes suponen que Helios está en `D:/Progra/Helios`. **Cambiar esa ruta por la ruta real.** Si ya tienes las carpetas, no vuelvas a clonarlas.

Si ya clonaste este repositorio, descarga y selecciona la rama del ejemplo antes de compilar:

```powershell
git fetch origin
git switch feat/simulacion-helios
```

Primero compilar y ejecutar la geometría sin ventana, para comprobar el entorno:

```powershell
cmake -S simulacion_helios -B build-geometria -DHELIOS_ROOT="D:/Progra/Helios" -DMODO=geometria -DCON_VISUALIZADOR=OFF
cmake --build build-geometria --config Release --parallel 2
./build-geometria/simulacion_geometria.exe resultados-geometria
```

Para abrir la escena 3D, reconfigurar ese mismo proyecto con `-DCON_VISUALIZADOR=ON`, volver a compilar y ejecutar. Cerrar la ventana para terminar el programa.

Para calcular radiación (sin ventana):

```powershell
cmake -S simulacion_helios -B build-radiacion -DHELIOS_ROOT="D:/Progra/Helios" -DMODO=radiacion -DCON_VISUALIZADOR=OFF
cmake --build build-radiacion --config Release --parallel 2
./build-radiacion/simulacion_radiacion.exe resultados-radiacion
```

Helios selecciona el backend disponible. Para solicitar Vulkan explícitamente, añadir `-DFORCE_VULKAN_BACKEND=ON` al comando de configuración de radiación. Para mostrar el mapa de radiación en 3D, usar `-DCON_VISUALIZADOR=ON`; se muestra el último caso, a 70°. Los CSV se guardan antes de abrir la ventana.

## Linux

Instalar previamente las dependencias oficiales de Helios. Desde la raíz de este repositorio, con Helios en la carpeta hermana:

```bash
cmake -S simulacion_helios -B build-geometria -DHELIOS_ROOT="$PWD/../Helios" -DMODO=geometria -DCON_VISUALIZADOR=OFF -DCMAKE_BUILD_TYPE=Release
cmake --build build-geometria --parallel 2
./build-geometria/simulacion_geometria resultados-geometria

cmake -S simulacion_helios -B build-radiacion -DHELIOS_ROOT="$PWD/../Helios" -DMODO=radiacion -DCON_VISUALIZADOR=OFF -DCMAKE_BUILD_TYPE=Release
cmake --build build-radiacion --parallel 2
./build-radiacion/simulacion_radiacion resultados-radiacion
```

## Experimento de radiación

Se comparan elevaciones solares de 30°, 50° y 70°, con azimut fijo de 45°. Se usa una fuente colimada de 500 W/m², medidos en un plano perpendicular a la dirección solar. El flujo incidente sobre una superficie horizontal cambia con la elevación; por tanto, la comparación mezcla proyección solar y sombras.

Se desactiva la emisión térmica y no se incluye radiación difusa. Hojas y suelo tienen reflectividad y transmisividad cero: son absorbedores negros idealizados. No se han asignado propiedades ópticas medidas de una especie. El suelo absorbe únicamente por su cara superior. La parcela es finita y no tiene fronteras periódicas.

`PAR` identifica aquí una banda de energía suministrada manualmente; no se integra un espectro solar ni se convierten los resultados a flujo de fotones. No son predicciones de fotosíntesis, rendimiento agrícola ni clima de una ubicación. Las tres posiciones del sol no equivalen a tres horas de un día calculadas astronómicamente.

La fracción exportada se define como `potencia_hojas / (potencia_hojas + potencia_suelo)`. Es la proporción de absorción dentro de los dos grupos modelados, no la fracción de toda la energía incidente sobre la parcela. El trazado usa muestreo numérico y puede variar ligeramente entre ejecuciones; para estudios de convergencia aumentar `setDirectRayCount`.

## Resultados

| Archivo | Contenido |
| --- | --- |
| `geometria.csv` | Cantidades, área foliar de una cara, área de suelo e índice de área foliar LAI. |
| `escena.obj` y archivos auxiliares | Geometría para inspección en un visor 3D, incluido Blender. |
| `escena.xml` | Geometría y datos exportados por Helios. |
| `radiacion.csv` | Tres escenarios; potencia absorbida por hojas y suelo (W) y flujo medio foliar (W/m²). Solo en modo radiación. |
| `hojas_30.csv`, `hojas_50.csv`, `hojas_70.csv` | Área y flujo de cada primitiva foliar, con identificador de planta. |

Cada hoja contiene 4 × 2 primitivas; los archivos de detalle tienen una fila por primitiva, no por hoja. Con los valores iniciales se esperan 128 hojas, 1024 primitivas foliares, área foliar 7.8336 m² y LAI aproximadamente 1.6185. Esos valores son geométricos, no resultados de radiación. Una nueva ejecución sobre el mismo directorio reemplaza los archivos del mismo nombre; usar carpetas distintas para conservar experimentos.

Para graficar el resultado de radiación desde la raíz del repositorio:

```bash
python -m pip install matplotlib
python simulacion_helios/graficar.py resultados-radiacion/radiacion.csv
```

Se genera `resultados-radiacion/comparacion_PAR.png`. El script solo utiliza resultados previamente calculados; no inventa observaciones si faltan datos.

## Cambiar el escenario

Editar `Parametros` en `src/escena.h` para modificar filas, columnas, separación y tamaño o cantidad de hojas. La forma de las plantas está en `crearEscena`. Si se amplía la plantación, ampliar también el suelo para cubrirla. Editar las elevaciones, azimut, intensidad o número de rayos en `src/main.cpp`. Después de editar C++, recompilar.

## Referencias

- [Documentación de Helios](https://plantsimulationlab.github.io/Helios/).
- Instalación: consultar **Install and Set-up** en el menú de la documentación oficial.
- [Modelo de radiación](https://plantsimulationlab.github.io/Helios/_radiation_doc.html).
- [Ejemplo oficial de radiación, tutorial 10](https://github.com/PlantSimulationLab/Helios/tree/master/samples/tutorial10).
