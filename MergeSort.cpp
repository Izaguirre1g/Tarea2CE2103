#include "qcustomplot.h"
#include <QApplication>
#include <QVector>
#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm> // Para std::shuffle
#include <random>    // Para std::random_device y std::mt19937
#include <cmath>     // Para funciones matemáticas

using namespace std;
using namespace std::chrono;

// Función para obtener el tiempo en nanosegundos
long long obtenerTiempoEnNanoSegundos() {
    return duration_cast<nanoseconds>(high_resolution_clock::now().time_since_epoch()).count();
}

// Función de mezcla para Merge Sort
void mezclarVectores(vector<int>& arr, int izq, int medio, int der) {
    int tamIzq = medio - izq + 1;
    int tamDer = der - medio;

    vector<int> subIzq(tamIzq);
    vector<int> subDer(tamDer);

    for (int i = 0; i < tamIzq; i++)
        subIzq[i] = arr[izq + i];
    for (int j = 0; j < tamDer; j++)
        subDer[j] = arr[medio + 1 + j];

    int i = 0, j = 0, k = izq;
    while (i < tamIzq && j < tamDer) {
        if (subIzq[i] <= subDer[j]) {
            arr[k] = subIzq[i];
            i++;
        } else {
            arr[k] = subDer[j];
            j++;
        }
        k++;
    }

    while (i < tamIzq) {
        arr[k] = subIzq[i];
        i++;
        k++;
    }

    while (j < tamDer) {
        arr[k] = subDer[j];
        j++;
        k++;
    }
}

// Implementación de Merge Sort
void ordenarPorMezcla(vector<int>& arr, int izq, int der) {
    if (izq < der) {
        int medio = izq + (der - izq) / 2;

        ordenarPorMezcla(arr, izq, medio);
        ordenarPorMezcla(arr, medio + 1, der);

        mezclarVectores(arr, izq, medio, der);
    }
}

// Genera un array en el mejor caso (ordenado)
vector<int> generarMejorCaso(int n) {
    vector<int> arreglo(n);
    for (int i = 0; i < n; i++) {
        arreglo[i] = i;
    }
    return arreglo;
}

// Genera un array en el peor caso (orden inverso)
vector<int> generarPeorCaso(int n) {
    vector<int> arreglo(n);
    for (int i = 0; i < n; i++) {
        arreglo[i] = n - i;
    }
    return arreglo;
}

// Genera un array en un caso promedio (aleatorio)
vector<int> generarCasoPromedio(int n) {
    vector<int> arreglo(n);
    for (int i = 0; i < n; i++) {
        arreglo[i] = i;
    }
    random_device rd;
    mt19937 g(rd());
    shuffle(arreglo.begin(), arreglo.end(), g);
    return arreglo;
}

// Realiza los benchmarks y almacena los resultados
void ejecutarBenchmarks(const vector<int>& tamanos, vector<long long>& tiemposMejorCaso, vector<long long>& tiemposPeorCaso, vector<long long>& tiemposCasoPromedio) {
    for (int n : tamanos) {
        // Mejor caso
        vector<int> mejorCaso = generarMejorCaso(n);
        long long inicio = obtenerTiempoEnNanoSegundos();
        ordenarPorMezcla(mejorCaso, 0, n - 1);
        long long fin = obtenerTiempoEnNanoSegundos();
        tiemposMejorCaso.push_back(fin - inicio);

        // Peor caso
        vector<int> peorCaso = generarPeorCaso(n);
        inicio = obtenerTiempoEnNanoSegundos();
        ordenarPorMezcla(peorCaso, 0, n - 1);
        fin = obtenerTiempoEnNanoSegundos();
        tiemposPeorCaso.push_back(fin - inicio);

        // Caso promedio
        vector<int> casoPromedio = generarCasoPromedio(n);
        inicio = obtenerTiempoEnNanoSegundos();
        ordenarPorMezcla(casoPromedio, 0, n - 1);
        fin = obtenerTiempoEnNanoSegundos();
        tiemposCasoPromedio.push_back(fin - inicio);
    }
}

// Función para graficar resultados de benchmarks
void graficarResultados(QCustomPlot* grafico, const vector<int>& tamanos, const vector<long long>& tiemposMejor, const vector<long long>& tiemposPeor, const vector<long long>& tiemposPromedio) {
    QVector<double> x(tamanos.size()), yMejor(tamanos.size()), yPeor(tamanos.size()), yPromedio(tamanos.size());

    for (size_t i = 0; i < tamanos.size(); ++i) {
        x[i] = tamanos[i];
        yMejor[i] = tiemposMejor[i];
        yPeor[i] = tiemposPeor[i];
        yPromedio[i] = tiemposPromedio[i];
    }

    // Graficar mejor caso
    grafico->addGraph();
    grafico->graph(0)->setData(x, yMejor);
    grafico->graph(0)->setPen(QPen(Qt::blue));
    grafico->graph(0)->setName("Mejor Caso O(nlogn)");

    // Graficar peor caso
    grafico->addGraph();
    grafico->graph(1)->setData(x, yPeor);
    grafico->graph(1)->setPen(QPen(Qt::red));
    grafico->graph(1)->setName("Peor Caso O(nlogn)");

    // Graficar caso promedio
    grafico->addGraph();
    grafico->graph(2)->setData(x, yPromedio);
    grafico->graph(2)->setPen(QPen(Qt::green));
    grafico->graph(2)->setName("Caso Promedio O(nlogn)");

    // Ajustar etiquetas y rango de ejes
    grafico->xAxis->setLabel("Tamaño de entrada (n)");
    grafico->yAxis->setLabel("Tiempo (nanosegundos)");

    grafico->xAxis->setRange(0, tamanos.back());
    grafico->yAxis->setRange(0, *max_element(yPeor.begin(), yPeor.end()) + 100);

    // Mostrar leyenda y replotear
    grafico->legend->setVisible(true);
    grafico->replot();
}

// Función para graficar la complejidad teórica
void graficarTeoria(QCustomPlot* grafico, const vector<int>& tamanos) {
    QVector<double> x(tamanos.size()), yTeoricoMejor(tamanos.size()), yTeoricoPeor(tamanos.size()), yTeoricoPromedio(tamanos.size());

    for (size_t i = 0; i < tamanos.size(); ++i) {
        x[i] = tamanos[i];
        yTeoricoMejor[i] = pow(tamanos[i], 2); 
        yTeoricoPeor[i] = pow(tamanos[i], 2); 
        yTeoricoPromedio[i] = pow(tamanos[i], 2);   
    }

    grafico->addGraph();
    grafico->graph(0)->setData(x, yTeoricoMejor);
    grafico->graph(0)->setPen(QPen(Qt::blue, 2));
    grafico->graph(0)->setName("Mejor Caso (Teórico) O(nlogn)");

    grafico->addGraph();
    grafico->graph(1)->setData(x, yTeoricoPeor);
    grafico->graph(1)->setPen(QPen(Qt::red, 2));
    grafico->graph(1)->setName("Peor Caso (Teórico) O(nlogn)");

    grafico->addGraph();
    grafico->graph(2)->setData(x, yTeoricoPromedio);
    grafico->graph(2)->setPen(QPen(Qt::green, 2));
    grafico->graph(2)->setName("Caso Promedio (Teórico) O(nlogn)");

    grafico->xAxis->setLabel("Tamaño de entrada (n)");
    grafico->yAxis->setLabel("Operaciones");

    grafico->xAxis->setRange(0, tamanos.back());
    grafico->yAxis->setRange(0, pow(tamanos.back(), 2));

    grafico->legend->setVisible(true);
    grafico->replot();
}

int main(int argc, char *argv[]) {
    vector<int> tamanos = {100, 1000, 5000, 10000, 50000}; 
    vector<long long> tiemposMejor, tiemposPeor, tiemposPromedio;

    ejecutarBenchmarks(tamanos, tiemposMejor, tiemposPeor, tiemposPromedio);

    QApplication app(argc, argv);

    QCustomPlot graficoResultados;
    graficoResultados.legend->setVisible(true);
    graficarResultados(&graficoResultados, tamanos, tiemposMejor, tiemposPeor, tiemposPromedio);
    graficoResultados.resize(800, 600);
    graficoResultados.show();

    QCustomPlot graficoTeorico;
    graficarTeoria(&graficoTeorico, tamanos);
    graficoTeorico.resize(800, 600);
    graficoTeorico.show();

    return app.exec();
}

