"""Graficar el CSV producido por simulacion_radiacion; no genera datos."""
import argparse
import csv
from pathlib import Path


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("csv", type=Path, help="Ruta a radiacion.csv")
    parser.add_argument("--salida", type=Path, help="PNG de salida")
    args = parser.parse_args()
    with args.csv.open(encoding="utf-8", newline="") as archivo:
        filas = list(csv.DictReader(archivo))
    if not filas:
        parser.error("El CSV no contiene resultados.")
    filas.sort(key=lambda fila: float(fila["elevacion_grados"]))

    import matplotlib
    matplotlib.use("Agg")
    import matplotlib.pyplot as plt

    elevaciones = [float(f["elevacion_grados"]) for f in filas]
    fig, ax = plt.subplots(figsize=(8, 4.5), layout="constrained")
    for columna, etiqueta in [("PAR_absorbida_hojas_W", "Hojas"),
                              ("PAR_absorbida_suelo_W", "Suelo")]:
        ax.plot(elevaciones, [float(f[columna]) for f in filas],
                marker="o", label=etiqueta)
    ax.set(xlabel="Elevación solar (grados)", ylabel="Potencia PAR absorbida (W)",
           title="Dosel simplificado: comparación de elevaciones solares")
    ax.set_xticks(elevaciones)
    ax.grid(alpha=0.25)
    ax.legend()
    destino = args.salida or args.csv.with_name("comparacion_PAR.png")
    destino.parent.mkdir(parents=True, exist_ok=True)
    fig.savefig(destino, dpi=180)
    plt.close(fig)
    print(destino.resolve())


if __name__ == "__main__":
    main()
