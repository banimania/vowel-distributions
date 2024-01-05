#include <chrono>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

void replaceAll(std::string& str, const std::string& from, const std::string& to) {
    if (from.empty()) return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}

std::string distr(std::string entrada, char (&vocales)[5]) {
  std::string distribucion = "";
  replaceAll(entrada, "á", "a");
  replaceAll(entrada, "é", "e");
  replaceAll(entrada, "í", "i");
  replaceAll(entrada, "ó", "o");
  replaceAll(entrada, "ú", "u");
  for (char c : entrada) {
    for (char vocal : vocales) {
      if (c == vocal) distribucion += c;
    }
  }
  return distribucion;
}

std::vector<std::string> todas_las_distribuciones() {
  std::vector<std::string> distribuciones;

  for (const auto &distr : std::filesystem::directory_iterator("distribuciones")) {
    std::string ruta = distr.path();
    std::string nombre_distr = ruta.substr(ruta.find("/") + 1);
    distribuciones.push_back(nombre_distr);
  }

  return distribuciones;
}

void combinaciones(std::string objetivo, const std::vector<std::string>& strings, std::vector<std::string>& combinacionActual, std::vector<std::vector<std::string>>& todas_combinaciones) {
    if (objetivo.empty()) {
        todas_combinaciones.push_back(combinacionActual);
        return;
    }

    for (const auto& s : strings) {
        if (objetivo.find(s) == 0) {
            std::string nuevoObjetivo = objetivo.substr(s.length());
            combinacionActual.push_back(s);
            combinaciones(nuevoObjetivo, strings, combinacionActual, todas_combinaciones);
            combinacionActual.pop_back();
        }
    }
}

std::vector<std::string> generarPalabras(std::string distribucion) {
  std::vector<std::string> palabras;
  std::fstream archivo_distribucion("distribuciones/" + distribucion);

  if (archivo_distribucion.is_open()) {
    std::string palabra;

    while(getline(archivo_distribucion, palabra)) {
      palabras.push_back(palabra);
    }

  }

  return palabras;
}

int main(int argc, const char* argv[]) {
  auto tInicio = std::chrono::high_resolution_clock::now();

  if (argc == 1) {
    std::cout << "Necesitas introducir, al menos, una palabra." << std::endl;
    return 0;
  }

  char vocales[] = {'a', 'e', 'i', 'o', 'u'};

  std::string entrada = "";

  for (int i = 1; i < argc; i++) {
    entrada += std::string(argv[i]) + " ";
  }

  std::string distrib = distr(entrada, vocales);

  std::cout << "Distribución: " << distrib << std::endl;

  if (!std::filesystem::exists("distribuciones")) {
    std::cout << "Generando distribuciones..." << std::endl;

    int n = 0;

    std::filesystem::create_directory("distribuciones");

    std::fstream archivo_palabras;
    archivo_palabras.open("palabras.txt", std::ios::in);

    if (!archivo_palabras.is_open()) {
      std::cout << "Error al leer el archivo palabras.txt" << std::endl;
      return 0;
    }

    std::string palabraIt, distribIt;

    while (getline(archivo_palabras, palabraIt)) {
      distribIt = distr(palabraIt, vocales);

      std::ifstream f("distribuciones/" + distribIt);
      if (!f.good()) n++;

      std::ofstream archivo_nuevo("distribuciones/" + distribIt, std::ios::app);
      archivo_nuevo << palabraIt << std::endl;
      archivo_nuevo.close();
    }

    archivo_palabras.close();

    std::cout << "¡" << n << " distribuciones generadas!" << std::endl;
  }

  std::cout << "Distribuciones coincidentes: " << std::endl;

  std::vector<std::vector<std::string>> todas_combinaciones;
  std::vector<std::string> combinacionActual;
  combinaciones(distrib, todas_las_distribuciones(), combinacionActual, todas_combinaciones);

  /*for (std::vector<std::string> combinacion : todas_combinaciones) {
    for (std::string agrupacion : combinacion) {
      std::cout << agrupacion << " ";
    }
    std::cout << std::endl;
  }*/
  for (const auto& combinacion : todas_combinaciones) {
    for (const auto& distribucion : combinacion) {
        std::vector<std::string> palabras = generarPalabras(distribucion);
        for (const auto& palabra : palabras) {
          std::cout << palabra << "/";
        }
        std::cout << std::endl;
        std::cout << std::endl;
    }
    std::cout << "-------------------------";
  }

  auto tFinal = std::chrono::high_resolution_clock::now();
  auto t = std::chrono::duration_cast<std::chrono::milliseconds>(tFinal - tInicio);
  std::cout << "Coincidencias encontradas en " << t.count() << " milisegundos." << std::endl;

  return 0;
}

