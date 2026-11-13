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

Para abrir la escena 3D, reconfigura con `-DCON_VISUALIZADOR=ON`, vuelve a compilar y define `$env:HELIOS_BUILD = (Resolve-Path ./build-geometria).Path` antes de ejecutar. Así Helios encontrará los recursos gráficos. Cierra la ventana para terminar el programa.

Para calcular radiación (sin ventana):

```powershell
cmake -S simulacion_helios -B build-radiacion -DHELIOS_ROOT="D:/Progra/Helios" -DMODO=radiacion -DCON_VISUALIZADOR=OFF
cmake --build build-radiacion --config Release --parallel 2
./build-radiacion/simulacion_radiacion.exe resultados-radiacion
```

Helios selecciona el backend disponible. Para solicitar Vulkan explícitamente, añadir `-DFORCE_VULKAN_BACKEND=ON` al comando de configuración de radiación. Para mostrar el mapa de radiación en 3D, usar `-DCON_VISUALIZADOR=ON`; se muestra el último caso, a 70°. Los CSV se guardan antes de abrir la ventana.

## Linux

Con las carpetas `SS-Escom` y `Helios` juntas, ejecuta `cd simulacion_helios` desde la raíz de `SS-Escom`. Si ya estás en `simulacion_helios`, empieza por el comando `ls`.

```bash
cd simulacion_helios
ls ../../Helios/core/CMake_project.cmake
```

Si `ls` no encuentra el archivo, ajusta `HELIOS_ROOT` a la ruta real de Helios en los comandos de CMake. Si CMake no está disponible y ya usas Conda, instala CMake con `conda install -c conda-forge cmake` y comprueba la instalación con `cmake --version`.

### Geometría sin ventana

```bash
cmake -S . -B build-geometria -DHELIOS_ROOT="$PWD/../../Helios" -DMODO=geometria -DCON_VISUALIZADOR=OFF -DCMAKE_BUILD_TYPE=Release
cmake --build build-geometria --parallel 2
./build-geometria/simulacion_geometria resultados-geometria
```

### Visualizador 3D en Rocky Linux 9

Instala las bibliotecas de desarrollo de OpenGL y X11 en el equipo donde vas a compilar. Este paso requiere permisos de administrador.

```bash
sudo dnf install libglvnd-devel mesa-libGLU-devel libX11-devel libXrandr-devel libXinerama-devel libXcursor-devel libXi-devel
```

Compila el visualizador en su propia carpeta para conservar la compilación de geometría sin ventana.

```bash
cmake -S . -B build-visualizador -DHELIOS_ROOT="$PWD/../../Helios" -DMODO=geometria -DCON_VISUALIZADOR=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build-visualizador --parallel 2
```

Para abrir la escena, ejecuta el programa desde una terminal del escritorio gráfico. Si te conectas al servidor por XRDP, abre la terminal dentro de esa sesión. `HELIOS_BUILD` apunta a la carpeta donde CMake copió los shaders y otros recursos del visualizador.

```bash
HELIOS_BUILD="$PWD/build-visualizador" ./build-visualizador/simulacion_geometria resultados-visualizador
```

Si ya compilaste con el visualizador activado en `build-geometria`, puedes usar ese ejecutable directamente.

```bash
HELIOS_BUILD="$PWD/build-geometria" ./build-geometria/simulacion_geometria resultados-geometria
```

Si la ventana sigue sin abrir y aparece un error del controlador `virtio_gpu`, prueba el renderizado por software anteponiendo `LIBGL_ALWAYS_SOFTWARE=1` al comando de ejecución. Por ejemplo, con `build-visualizador`:

```bash
LIBGL_ALWAYS_SOFTWARE=1 HELIOS_BUILD="$PWD/build-visualizador" ./build-visualizador/simulacion_geometria resultados-visualizador
```

### Radiación sin ventana

Ejecuta este modo en un equipo con un backend de radiación compatible con Helios.

```bash
cmake -S . -B build-radiacion -DHELIOS_ROOT="$PWD/../../Helios" -DMODO=radiacion -DCON_VISUALIZADOR=OFF -DCMAKE_BUILD_TYPE=Release
cmake --build build-radiacion --parallel 2
HELIOS_BUILD="$PWD/build-radiacion" ./build-radiacion/simulacion_radiacion resultados-radiacion
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

Para graficar el resultado de radiación, ejecuta estos comandos desde el mismo directorio en el que ejecutaste el programa C++. En Linux, desde `simulacion_helios`:

```bash
python -m pip install matplotlib
python graficar.py resultados-radiacion/radiacion.csv
```

En Windows, desde la raíz del repositorio, usa `python simulacion_helios/graficar.py resultados-radiacion/radiacion.csv`. Se genera `resultados-radiacion/comparacion_PAR.png`. El script solo utiliza resultados previamente calculados; no inventa observaciones si faltan datos.

## Cambiar el escenario

Editar `Parametros` en `src/escena.h` para modificar filas, columnas, separación y tamaño o cantidad de hojas. La forma de las plantas está en `crearEscena`. Si se amplía la plantación, ampliar también el suelo para cubrirla. Editar las elevaciones, azimut, intensidad o número de rayos en `src/main.cpp`. Después de editar C++, recompilar.

## Referencias

- [Documentación de Helios](https://plantsimulationlab.github.io/Helios/).
- Instalación: consultar **Install and Set-up** en el menú de la documentación oficial.
- [Modelo de radiación](https://plantsimulationlab.github.io/Helios/_radiation_doc.html).
- [Ejemplo oficial de radiación, tutorial 10](https://github.com/PlantSimulationLab/Helios/tree/master/samples/tutorial10).
