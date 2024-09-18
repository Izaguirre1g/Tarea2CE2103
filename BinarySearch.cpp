#include "qcustomplot.h"
#include <QApplication>
#include <QVector>
#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm> // Para std::shuffle
#include <random>    // Para std::random_device y std::mt19937
#include <cmath>     // Para funciones matemáticas
#include <queue>     // Para std::queue

using namespace std;
using namespace std::chrono;

//Medición de tiempo
long long getSystemTimeNano() {
    return duration_cast<nanoseconds>(high_resolution_clock::now().time_since_epoch()).count();
}

//Nodo del árbol de búsqueda binario (BST)
struct TreeNode {
    int value;
    TreeNode* left;
    TreeNode* right;

    TreeNode(int val) : value(val), left(nullptr), right(nullptr) {}
};


TreeNode* insert(TreeNode* root, int value) {
    if (root == nullptr) {
        return new TreeNode(value);
    }
    if (value < root->value) {
        root->left = insert(root->left, value);
    } else {
        root->right = insert(root->right, value);
    }
    return root;
}

// Mejor caso
TreeNode* generateBalancedBST(int n) {
    vector<int> values(n);
    for (int i = 0; i < n; ++i) {
        values[i] = i;
    }
    random_device rd;
    mt19937 g(rd());
    shuffle(values.begin(), values.end(), g);

    TreeNode* root = nullptr;
    for (int val : values) {
        root = insert(root, val);
    }
    return root;
}

//Peor caso
TreeNode* generateWorstCaseBST(int n) {
    TreeNode* root = nullptr;
    for (int i = 0; i < n; ++i) {
        root = insert(root, i);
    }
    return root;
}

//Caso promedio
TreeNode* generateAverageCaseBST(int n) {
    vector<int> values(n);
    for (int i = 0; i < n; ++i) {
        values[i] = i;
    }
    random_device rd;
    mt19937 g(rd());
    shuffle(values.begin(), values.end(), g);

    TreeNode* root = nullptr;
    for (int val : values) {
        root = insert(root, val);
    }
    return root;
}

// Mide el tiempo de inserción en el BST
long long benchmarkInsertion(int n, TreeNode* (*generateBST)(int)) {
    TreeNode* root = generateBST(n);
    auto start = high_resolution_clock::now();
    for (int i = 0; i < n; ++i) {
        insert(root, i);
    }
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<nanoseconds>(end - start).count();
    return duration;
}

// Función para realizar los benchmarks y devolver los resultados
void runBenchmarks(const vector<int>& sizes, vector<long long>& bestCaseTimes, vector<long long>& worstCaseTimes, vector<long long>& avgCaseTimes) {
    for (int n : sizes) {
        // Mejor caso
        long long bestCaseTime = benchmarkInsertion(n, generateBalancedBST);
        bestCaseTimes.push_back(bestCaseTime);

        // Peor caso
        long long worstCaseTime = benchmarkInsertion(n, generateWorstCaseBST);
        worstCaseTimes.push_back(worstCaseTime);

        // Caso promedio
        long long avgCaseTime = benchmarkInsertion(n, generateAverageCaseBST);
        avgCaseTimes.push_back(avgCaseTime);
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
    customPlot->graph(0)->setName("Mejor Caso O(logn)");

    // Graficar peor caso
    customPlot->addGraph();
    customPlot->graph(1)->setData(x, yWorst);
    customPlot->graph(1)->setPen(QPen(Qt::red));
    customPlot->graph(1)->setName("Peor Caso O(n)");

    // Graficar caso promedio
    customPlot->addGraph();
    customPlot->graph(2)->setData(x, yAvg);
    customPlot->graph(2)->setPen(QPen(Qt::green));
    customPlot->graph(2)->setName("Caso Promedio O(logn)");

    // Ajustar etiquetas y rango de ejes
    customPlot->xAxis->setLabel("Tamaño de entrada (n)");
    customPlot->yAxis->setLabel("Tiempo (nanosegundos)");

    // Ajustar los rangos
    customPlot->xAxis->setRange(0, sizes.back());
    customPlot->yAxis->setRange(0, *ranges::max_element(yWorst) + 100);  // Ajustar el rango del eje Y

    // Aplicar escala logarítmica al eje Y
    customPlot->yAxis->setScaleType(QCPAxis::stLogarithmic);

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
        yBest[i] = log2(sizes[i]+100);// se suma 100 para que aparezca en la escala del eje y      // O(log n)
        yWorst[i] = sizes[i];          // O(n)
        yAvg[i] = log2(sizes[i]);      // O(log n) caso promedio
    }

    // Graficar mejor caso
    customPlot->addGraph();
    customPlot->graph(0)->setData(x, yBest);
    customPlot->graph(0)->setPen(QPen(Qt::blue, 2));
    customPlot->graph(0)->setName("Mejor Caso (Teórico) O(logn)");

    // Graficar peor caso
    customPlot->addGraph();
    customPlot->graph(1)->setData(x, yWorst);
    customPlot->graph(1)->setPen(QPen(Qt::red, 2));
    customPlot->graph(1)->setName("Peor Caso (Teórico) O(n)");

    // Graficar caso promedio
    customPlot->addGraph();
    customPlot->graph(2)->setData(x, yAvg);
    customPlot->graph(2)->setPen(QPen(Qt::green, 2));
    customPlot->graph(2)->setName("Caso Promedio (Teórico) O(logn)");

    // Ajustar etiquetas y rango de ejes
    customPlot->xAxis->setLabel("Tamaño de entrada (n)");
    customPlot->yAxis->setLabel("Operaciones");

    // Ajustar los rangos
    customPlot->xAxis->setRange(0, sizes.back());
    customPlot->yAxis->setRange(0, sizes.back());  // Ajustar el rango del eje Y

    // Aplicar escala logarítmica al eje Y
    customPlot->yAxis->setScaleType(QCPAxis::stLogarithmic);

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

    // Crear la gráfica de los resultados
    QApplication a(argc, argv);
    QCustomPlot customPlot1;
    customPlot1.legend->setVisible(true);
    plotResults(&customPlot1, sizes, bestCaseTimes, worstCaseTimes, avgCaseTimes);
    customPlot1.resize(800, 600);
    customPlot1.show();

    // Crear la gráfica del comportamiento teórico
    QCustomPlot customPlot2;
    customPlot2.legend->setVisible(true);
    plotTheoretical(&customPlot2, sizes);
    customPlot2.resize(800, 600);
    customPlot2.show();

    return a.exec();
}
