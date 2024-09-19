#include "qcustomplot.h"
#include <QApplication>
#include <QVector>
#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm> // Para std::shuffle
#include <random>    // Para generador de números aleatorios
#include <cmath>     // Para operaciones matemáticas
#include <queue>     // Para el uso de std::queue

using namespace std;
using namespace chrono;

// Función para obtener el tiempo actual en nanosegundos
long long obtenerTiempoSistemaNano() {
    return duration_cast<nanoseconds>(high_resolution_clock::now().time_since_epoch()).count();
}

// Definición de un nodo para el árbol binario de búsqueda (BST)
struct Nodo {
    int valor;
    Nodo* izquierda;
    Nodo* derecha;

    Nodo(int v) : valor(v), izquierda(nullptr), derecha(nullptr) {}
};

// Inserta un nuevo valor en el árbol BST
Nodo* insertar(Nodo* raiz, int valor) {
    if (!raiz) {
        return new Nodo(valor);
    }
    if (valor < raiz->valor) {
        raiz->izquierda = insertar(raiz->izquierda, valor);
    } else {
        raiz->derecha = insertar(raiz->derecha, valor);
    }
    return raiz;
}

// Genera un árbol BST balanceado
Nodo* generarBSTBalanceado(int n) {
    vector<int> valores(n);
    for (int i = 0; i < n; ++i) {
        valores[i] = i;
    }
    random_device rd;
    mt19937 generador(rd());
    shuffle(valores.begin(), valores.end(), generador);

    Nodo* raiz = nullptr;
    for (int v : valores) {
        raiz = insertar(raiz, v);
    }
    return raiz;
}

// Genera un BST en el peor caso (desequilibrado)
Nodo* generarBSTPeorCaso(int n) {
    Nodo* raiz = nullptr;
    for (int i = 0; i < n; ++i) {
        raiz = insertar(raiz, i);
    }
    return raiz;
}

// Genera un BST en el caso promedio
Nodo* generarBSTCasoPromedio(int n) {
    vector<int> valores(n);
    for (int i = 0; i < n; ++i) {
        valores[i] = i;
    }
    random_device rd;
    mt19937 generador(rd());
    shuffle(valores.begin(), valores.end(), generador);

    Nodo* raiz = nullptr;
    for (int v : valores) {
        raiz = insertar(raiz, v);
    }
    return raiz;
}

// Mide el tiempo de inserción en el BST
long long medirTiempoInsercion(int n, Nodo* (*generarBST)(int)) {
    Nodo* raiz = generarBST(n);
    auto inicio = high_resolution_clock::now();
    for (int i = 0; i < n; ++i) {
        insertar(raiz, i);
    }
    auto fin = high_resolution_clock::now();
    return duration_cast<nanoseconds>(fin - inicio).count();
}

// Ejecuta los benchmarks y almacena los resultados
void ejecutarBenchmarks(const vector<int>& tamanos, vector<long long>& tiemposMejorCaso, vector<long long>& tiemposPeorCaso, vector<long long>& tiemposPromedio) {
    for (int n : tamanos) {
        tiemposMejorCaso.push_back(medirTiempoInsercion(n, generarBSTBalanceado));
        tiemposPeorCaso.push_back(medirTiempoInsercion(n, generarBSTPeorCaso));
        tiemposPromedio.push_back(medirTiempoInsercion(n, generarBSTCasoPromedio));
    }
}

// Función para graficar los resultados de los benchmarks
void graficarResultados(QCustomPlot* customPlot, const vector<int>& tamanos, const vector<long long>& tiemposMejorCaso, const vector<long long>& tiemposPeorCaso, const vector<long long>& tiemposPromedio) {
    QVector<double> x(tamanos.size()), yMejor(tamanos.size()), yPeor(tamanos.size()), yPromedio(tamanos.size());

    // Rellenar los datos
    for (size_t i = 0; i < tamanos.size(); ++i) {
        x[i] = tamanos[i];
        yMejor[i] = tiemposMejorCaso[i];
        yPeor[i] = tiemposPeorCaso[i];
        yPromedio[i] = tiemposPromedio[i];
    }

    // Graficar los diferentes casos
    customPlot->addGraph();
    customPlot->graph(0)->setData(x, yMejor);
    customPlot->graph(0)->setPen(QPen(Qt::blue));
    customPlot->graph(0)->setName("Mejor Caso");

    customPlot->addGraph();
    customPlot->graph(1)->setData(x, yPeor);
    customPlot->graph(1)->setPen(QPen(Qt::red));
    customPlot->graph(1)->setName("Peor Caso");

    customPlot->addGraph();
    customPlot->graph(2)->setData(x, yPromedio);
    customPlot->graph(2)->setPen(QPen(Qt::green));
    customPlot->graph(2)->setName("Caso Promedio");

    // Etiquetas y ajustes de ejes
    customPlot->xAxis->setLabel("Tamaño de entrada");
    customPlot->yAxis->setLabel("Tiempo en nanosegundos");
    customPlot->xAxis->setRange(0, tamanos.back());
    customPlot->yAxis->setRange(0, *max_element(yPeor.constBegin(), yPeor.constEnd()) + 100);

    customPlot->legend->setVisible(true);
    customPlot->replot();
}

// Graficar el comportamiento teórico
void graficarTeorico(QCustomPlot* customPlot, const vector<int>& tamanos) {
    QVector<double> x(tamanos.size()), yMejor(tamanos.size()), yPeor(tamanos.size()), yPromedio(tamanos.size());

    for (size_t i = 0; i < tamanos.size(); ++i) {
        x[i] = tamanos[i];
        yMejor[i] = log2(tamanos[i] + 100); // O(log n)
        yPeor[i] = tamanos[i];              // O(n)
        yPromedio[i] = log2(tamanos[i]);    // O(log n)
    }

    customPlot->addGraph();
    customPlot->graph(0)->setData(x, yMejor);
    customPlot->graph(0)->setPen(QPen(Qt::blue, 2));
    customPlot->graph(0)->setName("Mejor Caso (Teórico)");

    customPlot->addGraph();
    customPlot->graph(1)->setData(x, yPeor);
    customPlot->graph(1)->setPen(QPen(Qt::red, 2));
    customPlot->graph(1)->setName("Peor Caso (Teórico)");

    customPlot->addGraph();
    customPlot->graph(2)->setData(x, yPromedio);
    customPlot->graph(2)->setPen(QPen(Qt::green, 2));
    customPlot->graph(2)->setName("Caso Promedio (Teórico)");

    customPlot->xAxis->setLabel("Tamaño de entrada");
    customPlot->yAxis->setLabel("Operaciones");

    customPlot->xAxis->setRange(0, tamanos.back());
    customPlot->yAxis->setRange(0, tamanos.back());

    customPlot->legend->setVisible(true);
    customPlot->replot();
}

int main(int argc, char *argv[]) {
    vector<int> tamanos = {100, 1000, 5000, 10000, 50000};
    vector<long long> tiemposMejorCaso, tiemposPeorCaso, tiemposPromedio;

    ejecutarBenchmarks(tamanos, tiemposMejorCaso, tiemposPeorCaso, tiemposPromedio);

    QApplication app(argc, argv);
    QCustomPlot customPlot1;
    customPlot1.legend->setVisible(true);
    graficarResultados(&customPlot1, tamanos, tiemposMejorCaso, tiemposPeorCaso, tiemposPromedio);
    customPlot1.resize(800, 600);
    customPlot1.show();

    QCustomPlot customPlot2;
    customPlot2.legend->setVisible(true);
    graficarTeorico(&customPlot2, tamanos);
    customPlot2.resize(800, 600);
    customPlot2.show();

    return app.exec();
}

