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

//Medicion del tiempo
long long getSystemTimeNano() {
    return duration_cast<nanoseconds>(high_resolution_clock::now().time_since_epoch()).count();
}

// Algoritmo Merge Sort
void merge(vector<int>& arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    vector<int> L(n1);
    vector<int> R(n2);

    for (int i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (int j = 0; j < n2; j++)
        R[j] = arr[mid + 1 + j];

    int i = 0, j = 0;
    int k = left;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

void mergeSort(vector<int>& arr, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;

        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);

        merge(arr, left, mid, right);
    }
}

// Genera un array de tamaño n en el mejor caso (ya ordenado)
vector<int> generateBestCase(int n) {
    vector<int> arr(n);
    for (int i = 0; i < n; i++) {
        arr[i] = i;
    }
    return arr;
}

//Peor caso (orden inverso)
vector<int> generateWorstCase(int n) {
    vector<int> arr(n);
    for (int i = 0; i < n; i++) {
        arr[i] = n - i;
    }
    return arr;
}

//Caso promedio (aleatorio)
vector<int> generateAverageCase(int n) {
    vector<int> arr(n);
    for (int i = 0; i < n; i++) {
        arr[i] = i;
    }
    random_device rd;
    mt19937 g(rd());
    shuffle(arr.begin(), arr.end(), g);
    return arr;
}

// Función para realizar los benchmarks y devolver los resultados
void runBenchmarks(const vector<int>& sizes, vector<long long>& bestCaseTimes, vector<long long>& worstCaseTimes, vector<long long>& avgCaseTimes) {
    for (int n : sizes) {
        // Mejor caso
        vector<int> bestCase = generateBestCase(n);
        long long start = getSystemTimeNano();
        mergeSort(bestCase, 0, n - 1);
        long long end = getSystemTimeNano();
        bestCaseTimes.push_back(end - start);

        // Peor caso
        vector<int> worstCase = generateWorstCase(n);
        start = getSystemTimeNano();
        mergeSort(worstCase, 0, n - 1);
        end = getSystemTimeNano();
        worstCaseTimes.push_back(end - start);

        // Caso promedio
        vector<int> averageCase = generateAverageCase(n);
        start = getSystemTimeNano();
        mergeSort(averageCase, 0, n - 1);
        end = getSystemTimeNano();
        avgCaseTimes.push_back(end - start);
    }
}

// Función para graficar los resultados del benchmark
void plotResults(QCustomPlot* customPlot, const vector<int>& sizes, const vector<long long>& bestCaseTimes, const vector<long long>& worstCaseTimes, const vector<long long>& avgCaseTimes) {
    QVector<double> x(sizes.size()), yBest(sizes.size()), yWorst(sizes.size()), yAvg(sizes.size());

    // Llenar los datos
    for (size_t i = 0; i < sizes.size(); ++i) {
        x[i] = sizes[i];
        yBest[i] = bestCaseTimes[i]; 
        yWorst[i] = worstCaseTimes[i];
        yAvg[i] = avgCaseTimes[i];
    }

    // Graficar mejor caso
    customPlot->addGraph();
    customPlot->graph(0)->setData(x, yBest);
    customPlot->graph(0)->setPen(QPen(Qt::blue));
    customPlot->graph(0)->setName("Mejor Caso O(nlogn)");

    // Graficar peor caso
    customPlot->addGraph();
    customPlot->graph(1)->setData(x, yWorst);
    customPlot->graph(1)->setPen(QPen(Qt::red));
    customPlot->graph(1)->setName("Peor Caso O(nlogn)");

    // Graficar caso promedio
    customPlot->addGraph();
    customPlot->graph(2)->setData(x, yAvg);
    customPlot->graph(2)->setPen(QPen(Qt::green));
    customPlot->graph(2)->setName("Caso Promedio O(nlogn)");

    // Ajustar etiquetas y rango de ejes
    customPlot->xAxis->setLabel("Tamaño de entrada (n)");
    customPlot->yAxis->setLabel("Tiempo (nanosegundos)");

    // Ajustar los rangos
    customPlot->xAxis->setRange(0, sizes.back());
    customPlot->yAxis->setRange(0, *max_element(yWorst.begin(), yWorst.end()) + 100);  // Ajustar el rango del eje Y

    // Mostrar leyenda
    customPlot->legend->setVisible(true);

    // Reploteo final
    customPlot->replot();
}

// Función para graficar el comportamiento teórico
void plotTheoretical(QCustomPlot* customPlot, const vector<int>& sizes) {
    QVector<double> x(sizes.size()), yBest(sizes.size()), yWorst(sizes.size()), yAvg(sizes.size());

    // Llenar los datos con la complejidad teórica
    for (size_t i = 0; i < sizes.size(); ++i) {
        x[i] = sizes[i];
        yBest[i] = pow(sizes[i], 2); // O(n log n) para mejor caso en Merge Sort
        yWorst[i] = pow(sizes[i], 2); // O(n log n) para peor caso en Merge Sort
        yAvg[i] = pow(sizes[i], 2);   // O(n log n) caso promedio en Merge Sort
    }

    // Graficar mejor caso
    customPlot->addGraph();
    customPlot->graph(0)->setData(x, yBest);
    customPlot->graph(0)->setPen(QPen(Qt::blue, 2));
    customPlot->graph(0)->setName("Mejor Caso (Teórico) O(nlogn)");

    // Graficar peor caso
    customPlot->addGraph();
    customPlot->graph(1)->setData(x, yWorst);
    customPlot->graph(1)->setPen(QPen(Qt::red, 2));
    customPlot->graph(1)->setName("Peor Caso (Teórico) O(nlogn)");

    // Graficar caso promedio
    customPlot->addGraph();
    customPlot->graph(2)->setData(x, yAvg);
    customPlot->graph(2)->setPen(QPen(Qt::green, 2));
    customPlot->graph(2)->setName("Caso Promedio (Teórico) O(nlogn)");

    // Ajustar etiquetas y rango de ejes
    customPlot->xAxis->setLabel("Tamaño de entrada (n)");
    customPlot->yAxis->setLabel("Operaciones");

    // Ajustar los rangos
    customPlot->xAxis->setRange(0, sizes.back());
    customPlot->yAxis->setRange(0, pow(sizes.back(), 2));  // Ajustar el rango del eje Y

    // Mostrar leyenda
    customPlot->legend->setVisible(true);

    // Reploteo final
    customPlot->replot();
}

int main(int argc, char *argv[]) {
    // Realizar los benchmarks
    vector<int> sizes = {100, 1000, 5000, 10000, 50000}; // Tamaños de entrada
    vector<long long> bestCaseTimes, worstCaseTimes, avgCaseTimes;

    runBenchmarks(sizes, bestCaseTimes, worstCaseTimes, avgCaseTimes);

    //Crear la gráfica
    QApplication a(argc, argv);

    //Gráfica de resultados del benchmark
    QCustomPlot customPlot1;
    customPlot1.legend->setVisible(true);
    plotResults(&customPlot1, sizes, bestCaseTimes, worstCaseTimes, avgCaseTimes);
    customPlot1.resize(800, 600);
    customPlot1.show();

    //Gráfica de comportamiento teórico
    QCustomPlot customPlot2;
    customPlot2.legend->setVisible(true);
    plotTheoretical(&customPlot2, sizes);
    customPlot2.resize(800, 600);
    customPlot2.show();

    return a.exec();
}
