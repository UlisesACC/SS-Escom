#include "escena.h"
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#ifdef SIMULAR_RADIACION
#include "RadiationModel.h"
#endif
#ifdef USAR_VISUALIZADOR
#include "Visualizer.h"
#endif

using namespace helios;

int main(int argc, char **argv) {
    try {
        if (argc > 2) {
            std::cerr << "Uso: " << argv[0] << " [directorio_salida]\n";
            return 1;
        }
        const std::filesystem::path salida = argc == 2 ? argv[1] : "resultados";
        std::filesystem::create_directories(salida);
        Context context;
        Parametros p; // Editar dimensiones y numero de hojas en escena.h.
        const Escena e = crearEscena(context, p);
        const float area_suelo = p.lado_suelo * p.lado_suelo;
        std::ofstream geometria(salida / "geometria.csv");
        geometria.exceptions(std::ios::badbit | std::ios::failbit);
        geometria << "plantas,hojas,primitivas_foliares,area_foliar_m2,area_suelo_m2,LAI\n"
                  << p.filas * p.columnas << ','
                  << p.filas * p.columnas * p.hojas_por_planta << ','
                  << e.hojas.size() << ',' << e.area_foliar << ',' << area_suelo << ','
                  << e.area_foliar / area_suelo << '\n';
        geometria.close();
        context.writeOBJ((salida / "escena.obj").string());
        context.writeXML((salida / "escena.xml").string().c_str());
        std::cout << "Area foliar: " << e.area_foliar << " m2; LAI: "
                  << e.area_foliar / area_suelo << '\n';

#ifdef SIMULAR_RADIACION
        // Absorbedores negros: aislar sombras y geometria sin dispersion.
        // PAR es una banda idealizada: flujo directo normal constante de 500 W/m2.
        context.setPrimitiveData(context.getAllUUIDs(), "reflectivity_PAR", 0.f);
        context.setPrimitiveData(context.getAllUUIDs(), "transmissivity_PAR", 0.f);
        RadiationModel radiation(&context);
        radiation.addRadiationBand("PAR");
        radiation.disableEmission("PAR");
        radiation.setDirectRayCount("PAR", 512);
        radiation.setDiffuseRadiationFlux("PAR", 0.f);
        const uint sol = radiation.addCollimatedRadiationSource(
            sphere2cart(make_SphericalCoord(deg2rad(30.f), deg2rad(45.f))));
        radiation.setSourceFlux(sol, "PAR", 500.f);
        radiation.updateGeometry();
        std::ofstream resumen(salida / "radiacion.csv");
        resumen.exceptions(std::ios::badbit | std::ios::failbit);
        resumen << "elevacion_grados,azimut_grados,flujo_directo_normal_W_m2,"
                   "PAR_absorbida_hojas_W,PAR_absorbida_suelo_W,"
                   "PAR_media_hojas_W_m2,fraccion_absorbida_en_hojas\n";
        for (float elevacion : {30.f, 50.f, 70.f}) {
            radiation.setSourcePosition(sol,
                sphere2cart(make_SphericalCoord(deg2rad(elevacion), deg2rad(45.f))));
            radiation.runBand("PAR");
            float hojas_W, suelo_W, hojas_media;
            context.calculatePrimitiveDataAreaWeightedSum(e.hojas, "radiation_flux_PAR", hojas_W);
            context.calculatePrimitiveDataAreaWeightedSum(e.suelo, "radiation_flux_PAR", suelo_W);
            context.calculatePrimitiveDataAreaWeightedMean(e.hojas, "radiation_flux_PAR", hojas_media);
            const float total_W = hojas_W + suelo_W;
            resumen << std::setprecision(8) << elevacion << ",45,500,"
                    << hojas_W << ',' << suelo_W << ',' << hojas_media << ',';
            if (total_W > 0) resumen << hojas_W / total_W;
            resumen << '\n';
            std::ofstream detalle(salida / ("hojas_" + std::to_string(int(elevacion)) + ".csv"));
            detalle.exceptions(std::ios::badbit | std::ios::failbit);
            detalle << "uuid,planta,area_m2,PAR_absorbida_W_m2\n";
            for (uint id : e.hojas) {
                float flujo;
                int planta;
                context.getPrimitiveData(id, "radiation_flux_PAR", flujo);
                context.getPrimitiveData(id, "planta", planta);
                detalle << id << ',' << planta << ',' << context.getPrimitiveArea(id)
                        << ',' << flujo << '\n';
            }
            detalle.close();
            std::cout << "Elevacion " << elevacion << ": " << hojas_W
                      << " W absorbidos por las hojas\n";
        }
        resumen.close();
#endif
        std::cout << "Resultados: " << std::filesystem::absolute(salida) << '\n';
#ifdef USAR_VISUALIZADOR
        Visualizer visor(1000);
        visor.buildContextGeometry(&context);
#ifdef SIMULAR_RADIACION
        visor.colorContextPrimitivesByData("radiation_flux_PAR");
        visor.setColorbarTitle("PAR absorbida (W/m2), elevacion 70 grados");
#endif
        visor.plotInteractive();
#endif
        return 0;
    } catch (const std::exception &error) {
        std::cerr << "No se completo la simulacion: " << error.what() << '\n';
        return 1;
    }
}
