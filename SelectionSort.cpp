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

// Función para obtener el tiempo del sistema en nanosegundos
long long obtenerTiempoSistemaNano() {
    return duration_cast<nanoseconds>(high_resolution_clock::now().time_since_epoch()).count();
}

// Algoritmo de ordenamiento SelectionSort
void ordenamientoPorSeleccion(vector<int>& arr) {
    int n = arr.size();
    for (int i = 0; i < n - 1; i++) {
        int indiceMin = i;
        for (int j = i + 1; j < n; j++) {
            if (arr[j] < arr[indiceMin]) {
                indiceMin = j;
            }
        }
        if (indiceMin != i) {
            swap(arr[i], arr[indiceMin]);
        }
    }
}

// Generar el mejor caso (ya ordenado)
vector<int> generarMejorCaso(int n) {
    vector<int> arr(n);
    for (int i = 0; i < n; i++) {
        arr[i] = i;
    }
    return arr;
}

// Generar el peor caso (orden inverso)
vector<int> generarPeorCaso(int n) {
    vector<int> arr(n);
    for (int i = 0; i < n; i++) {
        arr[i] = n - i;
    }
    return arr;
}

// Generar el caso promedio (aleatorio)
vector<int> generarCasoPromedio(int n) {
    vector<int> arr(n);
    for (int i = 0; i < n; i++) {
        arr[i] = i;
    }
    random_device rd;
    mt19937 g(rd());
    shuffle(arr.begin(), arr.end(), g);
    return arr;
}

// Función para realizar pruebas de rendimiento
void ejecutarPruebas(const vector<int>& tamanios, vector<long long>& tiemposMejorCaso, vector<long long>& tiemposPeorCaso, vector<long long>& tiemposPromedio) {
    for (int n : tamanios) {
        // Mejor caso
        vector<int> mejorCaso = generarMejorCaso(n);
        long long inicio = obtenerTiempoSistemaNano();
        ordenamientoPorSeleccion(mejorCaso);
        long long fin = obtenerTiempoSistemaNano();
        tiemposMejorCaso.push_back(fin - inicio);

        // Peor caso
        vector<int> peorCaso = generarPeorCaso(n);
        inicio = obtenerTiempoSistemaNano();
        ordenamientoPorSeleccion(peorCaso);
        fin = obtenerTiempoSistemaNano();
        tiemposPeorCaso.push_back(fin - inicio);

        // Caso promedio
        vector<int> casoPromedio = generarCasoPromedio(n);
        inicio = obtenerTiempoSistemaNano();
        ordenamientoPorSeleccion(casoPromedio);
        fin = obtenerTiempoSistemaNano();
        tiemposPromedio.push_back(fin - inicio);
    }
}

// Función para graficar resultados del benchmark
void graficarResultados(QCustomPlot* grafico, const vector<int>& tamanios, const vector<long long>& tiemposMejorCaso, const vector<long long>& tiemposPeorCaso, const vector<long long>& tiemposPromedio) {
    QVector<double> x(tamanios.size()), yMejor(tamanios.size()), yPeor(tamanios.size()), yPromedio(tamanios.size());

    // Llenar datos
    for (size_t i = 0; i < tamanios.size(); ++i) {
        x[i] = tamanios[i];
        yMejor[i] = tiemposMejorCaso[i] + 10000000; // Elevar línea con valor constante
        yPeor[i] = tiemposPeorCaso[i];
        yPromedio[i] = tiemposPromedio[i];
    }

    // Graficar mejor caso
    grafico->addGraph();
    grafico->graph(0)->setData(x, yMejor);
    grafico->graph(0)->setPen(QPen(Qt::blue));
    grafico->graph(0)->setName("Mejor Caso O(n^2)");

    // Graficar peor caso
    grafico->addGraph();
    grafico->graph(1)->setData(x, yPeor);
    grafico->graph(1)->setPen(QPen(Qt::red));
    grafico->graph(1)->setName("Peor Caso O(n^2)");

    // Graficar caso promedio
    grafico->addGraph();
    grafico->graph(2)->setData(x, yPromedio);
    grafico->graph(2)->setPen(QPen(Qt::green));
    grafico->graph(2)->setName("Caso Promedio O(n^2)");

    // Etiquetas y rango de ejes
    grafico->xAxis->setLabel("Tamaño de entrada (n)");
    grafico->yAxis->setLabel("Tiempo (nanosegundos)");

    // Ajustar los rangos
    grafico->xAxis->setRange(0, tamanios.back());
    grafico->yAxis->setRange(0, *max_element(yPeor.begin(), yPeor.end()) + 100);

    // Mostrar leyenda
    grafico->legend->setVisible(true);

    // Reploteo final
    grafico->replot();
}

// Función para graficar la complejidad teórica
void graficarComportamientoTeorico(QCustomPlot* graficoTeorico, const vector<int>& tamanios) {
    QVector<double> x(tamanios.size()), yMejor(tamanios.size()), yPeor(tamanios.size()), yPromedio(tamanios.size());

    // Complejidad teórica O(n^2)
    for (size_t i = 0; i < tamanios.size(); ++i) {
        x[i] = tamanios[i];
        yMejor[i] = pow(tamanios[i], 2);
        yPeor[i] = pow(tamanios[i], 2);
        yPromedio[i] = pow(tamanios[i], 2);
    }

    // Graficar mejor caso
    graficoTeorico->addGraph();
    graficoTeorico->graph(0)->setData(x, yMejor);
    graficoTeorico->graph(0)->setPen(QPen(Qt::blue, 2));
    graficoTeorico->graph(0)->setName("Mejor Caso (Teórico) O(n^2)");

    // Graficar peor caso
    graficoTeorico->addGraph();
    graficoTeorico->graph(1)->setData(x, yPeor);
    graficoTeorico->graph(1)->setPen(QPen(Qt::red, 2));
    graficoTeorico->graph(1)->setName("Peor Caso (Teórico) O(n^2)");

    // Graficar caso promedio
    graficoTeorico->addGraph();
    graficoTeorico->graph(2)->setData(x, yPromedio);
    graficoTeorico->graph(2)->setPen(QPen(Qt::green, 2));
    graficoTeorico->graph(2)->setName("Caso Promedio (Teórico) O(n^2)");

    // Etiquetas y rango de ejes
    graficoTeorico->xAxis->setLabel("Tamaño de entrada (n)");
    graficoTeorico->yAxis->setLabel("Operaciones");

    // Ajustar los rangos
    graficoTeorico->xAxis->setRange(0, tamanios.back());
    graficoTeorico->yAxis->setRange(0, pow(tamanios.back(), 2));

    // Mostrar leyenda
    graficoTeorico->legend->setVisible(true);

    // Reploteo final
    graficoTeorico->replot();
}

int main(int argc, char *argv[]) {
    // Realizar las pruebas de rendimiento
    vector<int> tamanios = {100, 1000, 5000, 10000, 50000};
    vector<long long> tiemposMejorCaso, tiemposPeorCaso, tiemposPromedio;

    ejecutarPruebas(tamanios, tiemposMejorCaso, tiemposPeorCaso, tiemposPromedio);

    // Crear la aplicación y las gráficas
    QApplication aplicacion(argc, argv);

    // Gráfica de resultados de benchmark
    QCustomPlot graficoResultados;
    graficoResultados.legend->setVisible(true);
    graficarResultados(&graficoResultados, tamanios, tiemposMejorCaso, tiemposPeorCaso, tiemposPromedio);
    graficoResultados.resize(800, 600);
    graficoResultados.show();

    // Gráfica de comportamiento teórico
    QCustomPlot graficoTeorico;
    graficoTeorico.legend->setVisible(true);
    graficarComportamientoTeorico(&graficoTeorico, tamanios);
    graficoTeorico.resize(800, 600);
    graficoTeorico.show();

    return aplicacion.exec();
}

