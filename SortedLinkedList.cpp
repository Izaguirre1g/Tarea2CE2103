#include "qcustomplot.h"
#include <QApplication>
#include <QVector>
#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm> // Para std::shuffle
#include <random>    // Para std::random_device y std::mt19937
#include <cmath>     // Para funciones matemáticas
#include <list>      // Para std::list

using namespace std;
using namespace std::chrono;

// Función de medición de tiempo
long long getSystemTimeNano() {
    return duration_cast<nanoseconds>(high_resolution_clock::now().time_since_epoch()).count();
}

// Algoritmo de búsqueda en lista enlazada ordenada
bool searchSortedLinkedList(const list<int>& lst, int value) {
    for (const auto& elem : lst) {
        if (elem == value) {
            return true;
        }
        if (elem > value) {
            break; // La lista está ordenada, así que no necesitamos seguir buscando
        }
    }
    return false;
}

//Mejor caso (ya ordenada)
list<int> generateSortedList(int n) {
    list<int> lst;
    for (int i = 0; i < n; ++i) {
        lst.push_back(i);
    }
    return lst;
}

//Peor caso (orden inverso)
list<int> generateWorstCaseList(int n) {
    list<int> lst;
    for (int i = n - 1; i >= 0; --i) {
        lst.push_back(i);
    }
    return lst;
}

//Caso promedio (aleatorio)
list<int> generateAverageCaseList(int n) {
    vector<int> vec(n);
    for (int i = 0; i < n; ++i) {
        vec[i] = i;
    }
    random_device rd;
    mt19937 g(rd());
    shuffle(vec.begin(), vec.end(), g);

    list<int> lst(vec.begin(), vec.end());
    return lst;
}

// Función para realizar los benchmarks y devolver los resultados
void runBenchmarks(const vector<int>& sizes, vector<long long>& bestCaseTimes, vector<long long>& worstCaseTimes, vector<long long>& avgCaseTimes) {
    for (int n : sizes) {
        // Mejor caso
        list<int> sortedList = generateSortedList(n);
        int searchValue = 0; // El valor 0 está siempre en la lista
        long long start = getSystemTimeNano();
        searchSortedLinkedList(sortedList, searchValue);
        long long end = getSystemTimeNano();
        bestCaseTimes.push_back(end - start);

        // Peor caso
        list<int> worstCaseList = generateWorstCaseList(n);
        searchValue = n; // El valor n no está en la lista
        start = getSystemTimeNano();
        searchSortedLinkedList(worstCaseList, searchValue);
        end = getSystemTimeNano();
        worstCaseTimes.push_back(end - start);

        // Caso promedio
        list<int> averageCaseList = generateAverageCaseList(n);
        searchValue = n / 2; // Valor medio para el caso promedio
        start = getSystemTimeNano();
        searchSortedLinkedList(averageCaseList, searchValue);
        end = getSystemTimeNano();
        avgCaseTimes.push_back(end - start);
    }
}

//resultados del benchmark
void plotResults(QCustomPlot* customPlot, const vector<int>& sizes, const vector<long long>& bestCaseTimes, const vector<long long>& worstCaseTimes, const vector<long long>& avgCaseTimes) {
    QVector<double> x(sizes.size()), yBest(sizes.size()), yWorst(sizes.size()), yAvg(sizes.size());

    // Llenar los datos
    for (size_t i = 0; i < sizes.size(); ++i) {
        x[i] = sizes[i];
        yBest[i] = bestCaseTimes[i] + 1000; // Añadir un valor constante para elevar la línea
        yWorst[i] = worstCaseTimes[i];
        yAvg[i] = avgCaseTimes[i];
    }

    // Graficar mejor caso
    customPlot->addGraph();
    customPlot->graph(0)->setData(x, yBest);
    customPlot->graph(0)->setPen(QPen(Qt::blue));
    customPlot->graph(0)->setName("Mejor Caso O(1)");

    // Graficar peor caso
    customPlot->addGraph();
    customPlot->graph(1)->setData(x, yWorst);
    customPlot->graph(1)->setPen(QPen(Qt::red));
    customPlot->graph(1)->setName("Peor Caso O(n)");

    // Graficar caso promedio
    customPlot->addGraph();
    customPlot->graph(2)->setData(x, yAvg);
    customPlot->graph(2)->setPen(QPen(Qt::green));
    customPlot->graph(2)->setName("Caso Promedio O(n)");

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
        yBest[i] = 1;                   // O(1)
        yWorst[i] = sizes[i];          // O(n)
        yAvg[i] = sizes[i];            // O(n) caso promedio
    }

    // Graficar mejor caso
    customPlot->addGraph();
    customPlot->graph(0)->setData(x, yBest);
    customPlot->graph(0)->setPen(QPen(Qt::blue, 2));
    customPlot->graph(0)->setName("Mejor Caso (Teórico) O(1)");

    // Graficar peor caso
    customPlot->addGraph();
    customPlot->graph(1)->setData(x, yWorst);
    customPlot->graph(1)->setPen(QPen(Qt::red, 2));
    customPlot->graph(1)->setName("Peor Caso (Teórico) O(n)");

    // Graficar caso promedio
    customPlot->addGraph();
    customPlot->graph(2)->setData(x, yAvg);
    customPlot->graph(2)->setPen(QPen(Qt::green, 2));
    customPlot->graph(2)->setName("Caso Promedio (Teórico) O(n)");

    // Ajustar etiquetas y rango de ejes
    customPlot->xAxis->setLabel("Tamaño de entrada (n)");
    customPlot->yAxis->setLabel("Operaciones");

    // Ajustar los rangos
    customPlot->xAxis->setRange(0, sizes.back());
    customPlot->yAxis->setRange(0, sizes.back());  // Ajustar el rango del eje Y

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

    // Crear la gráfica
    QApplication a(argc, argv);

    // Gráfica de resultados del benchmark
    QCustomPlot customPlot1;
    customPlot1.legend->setVisible(true);
    plotResults(&customPlot1, sizes, bestCaseTimes, worstCaseTimes, avgCaseTimes);
    customPlot1.resize(800, 600);
    customPlot1.show();

    // Gráfica de comportamiento teórico
    QCustomPlot customPlot2;
    customPlot2.legend->setVisible(true);
    plotTheoretical(&customPlot2, sizes);
    customPlot2.resize(800, 600);
    customPlot2.show();

    return a.exec();
}
