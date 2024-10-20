#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <chrono>
#include <random>
#include <mutex>

std::mutex mtx; // Mutex para proteger la salida a la consola y el orden de llegada

struct Auto {
    int id;
    int distancia_total;
    int distancia_actual;
    bool terminado;
    Auto(int i, int d) : id(i), distancia_total(d), distancia_actual(0), terminado(false) {}
};

void correr(Auto &auto_obj, int &posicion_llegada, std::vector<std::pair<int, int>> &resultado) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> avance_dist(1, 10); // Avance de 1 a 10 metros
    std::uniform_int_distribution<> tiempo_espera(100, 500); // Espera de 100 a 500 ms

    while (auto_obj.distancia_actual < auto_obj.distancia_total) {
        int avance = avance_dist(gen);
        int espera = tiempo_espera(gen);

        {
            std::lock_guard<std::mutex> lock(mtx); // Proteger la salida a la consola
            auto_obj.distancia_actual += avance;
            if (auto_obj.distancia_actual > auto_obj.distancia_total) {
                auto_obj.distancia_actual = auto_obj.distancia_total; // No avanzar más de lo permitido
            }
            std::cout << "Auto" << auto_obj.id << " avanza " << avance << " metros (total: " 
                      << auto_obj.distancia_actual << " metros)" << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(espera)); // Pausa entre iteraciones
    }

    {
        std::lock_guard<std::mutex> lock(mtx);
        auto_obj.terminado = true;
        posicion_llegada++;
        resultado.push_back({posicion_llegada, auto_obj.id}); // Guardar la posición de llegada y el id del auto
        std::cout << "Auto" << auto_obj.id << " ha terminado la carrera en el lugar " 
                  << posicion_llegada << "!" << std::endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Uso: " << argv[0] << " <distancia_total> <num_autos>" << std::endl;
        return 1;
    }

    int distancia_total = std::stoi(argv[1]);
    int num_autos = std::stoi(argv[2]);

    std::vector<Auto> autos;
    std::vector<std::thread> threads;
    std::vector<std::pair<int, int>> resultado; // Almacenar el lugar de llegada y el id de cada auto
    int posicion_llegada = 0; // Para registrar la posición de llegada

    // Crear los autos
    for (int i = 0; i < num_autos; ++i) {
        autos.emplace_back(i, distancia_total);
    }

    std::cout << "Distancia total carrera: " << distancia_total << " metros" << std::endl;
    std::cout << "-----------------------------" << std::endl;

    // Crear y lanzar las hebras para cada auto
    for (int i = 0; i < num_autos; ++i) {
        threads.emplace_back(correr, std::ref(autos[i]), std::ref(posicion_llegada), std::ref(resultado));
    }

    // Esperar a que todas las hebras terminen
    for (auto &th : threads) {
        if (th.joinable()) {
            th.join();
        }
    }

    // Mostrar el orden de llegada (ya está almacenado en el orden correcto)
    std::cout << "Resultados finales: " << std::endl;
    std::cout << "Lugar  | Auto" << std::endl;
    std::cout << "------------------" << std::endl;
    for (const auto& r : resultado) {
        std::cout << r.first << "     | Auto" << r.second << std::endl;
    }

    return 0;
}
