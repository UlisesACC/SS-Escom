#pragma once
#include "Context.h"
#include <cmath>
#include <stdexcept>
#include <vector>

// Metros; geometria didactica, sin calibracion para una especie vegetal.
struct Parametros {
    int filas = 4;
    int columnas = 4;
    int hojas_por_planta = 8;
    float separacion = 0.38f;
    float lado_suelo = 2.2f;
    float largo_hoja = 0.34f;
    float ancho_hoja = 0.18f;
};

struct Escena {
    std::vector<uint> hojas;
    std::vector<uint> suelo;
    float area_foliar = 0;
};

inline Escena crearEscena(helios::Context &context, const Parametros &p) {
    using namespace helios;
    if (p.filas < 1 || p.columnas < 1 || p.hojas_por_planta < 1 ||
        p.separacion <= 0 || p.lado_suelo <= 0 ||
        p.largo_hoja <= 0 || p.ancho_hoja <= 0) {
        throw std::invalid_argument("Las dimensiones y cantidades deben ser positivas.");
    }
    Escena e;
    e.suelo = context.addTile(make_vec3(0, 0, 0),
        make_vec2(p.lado_suelo, p.lado_suelo), nullrotation,
        make_int2(30, 30), make_RGBcolor(0.40f, 0.27f, 0.15f));
    context.setPrimitiveData(e.suelo, "twosided_flag", uint(0));
    for (int i = 0; i < p.filas; ++i) {
        for (int j = 0; j < p.columnas; ++j) {
            const float x = (i - (p.filas - 1) * 0.5f) * p.separacion;
            const float y = (j - (p.columnas - 1) * 0.5f) * p.separacion;
            for (int k = 0; k < p.hojas_por_planta; ++k) {
                const float angulo = deg2rad(137.5f * k + 20.f * (i + j));
                auto ids = context.addTile(
                    make_vec3(x + 0.12f * std::cos(angulo),
                              y + 0.12f * std::sin(angulo), 0.18f + 0.07f * k),
                    make_vec2(p.largo_hoja, p.ancho_hoja),
                    make_SphericalCoord(deg2rad(25.f), angulo),
                    make_int2(4, 2), make_RGBcolor(0.15f, 0.60f, 0.20f));
                for (auto id : ids) {
                    context.setPrimitiveData(id, "planta", int(i * p.columnas + j));
                    context.setPrimitiveData(id, "altura", 0.18f + 0.07f * k);
                    e.area_foliar += context.getPrimitiveArea(id);
                }
                e.hojas.insert(e.hojas.end(), ids.begin(), ids.end());
            }
        }
    }
    return e;
}
