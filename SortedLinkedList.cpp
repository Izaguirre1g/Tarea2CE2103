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

// Función para obtener el tiempo actual en nanosegundos
long long obtenerTiempoActualEnNano() {
    return duration_cast<nanoseconds>(high_resolution_clock::now().time_since_epoch()).count();
}

// Algoritmo para buscar en una lista enlazada ordenada
bool buscarEnListaOrdenada(const list<int>& lista, int valor) {
    for (const auto& elemento : lista) {
        if (elemento == valor) {
            return true;
        }
        if (elemento > valor) {
            break; // La lista está ordenada, no es necesario continuar
        }
    }
    return false;
}

// Genera una lista ordenada (Mejor caso)
list<int> generarListaOrdenada(int n) {
    list<int> lista;
    for (int i = 0; i < n; ++i) {
        lista.push_back(i);
    }
    return lista;
}

// Genera una lista en orden inverso (Peor caso)
list<int> generarListaPeorCaso(int n) {
    list<int> lista;
    for (int i = n - 1; i >= 0; --i) {
        lista.push_back(i);
    }
    return lista;
}

// Genera una lista aleatoria (Caso promedio)
list<int> generarListaCasoPromedio(int n) {
    vector<int> vectorTemp(n);
    for (int i = 0; i < n; ++i) {
        vectorTemp[i] = i;
    }
    random_device dispositivo;
    mt19937 generador(dispositivo());
    shuffle(vectorTemp.begin(), vectorTemp.end(), generador);

    list<int> lista(vectorTemp.begin(), vectorTemp.end());
    return lista;
}

// Función para realizar los benchmarks y almacenar los resultados
void ejecutarPruebas(const vector<int>& tamanos, vector<long long>& tiemposMejorCaso, vector<long long>& tiemposPeorCaso, vector<long long>& tiemposCasoPromedio) {
    for (int tam : tamanos) {
        // Mejor caso
        list<int> listaOrdenada = generarListaOrdenada(tam);
        int valorBusqueda = 0; // El valor 0 siempre está presente en la lista
        long long inicio = obtenerTiempoActualEnNano();
        buscarEnListaOrdenada(listaOrdenada, valorBusqueda);
        long long fin = obtenerTiempoActualEnNano();
        tiemposMejorCaso.push_back(fin - inicio);

        // Peor caso
        list<int> listaPeorCaso = generarListaPeorCaso(tam);
        valorBusqueda = tam; // El valor tam no está en la lista
        inicio = obtenerTiempoActualEnNano();
        buscarEnListaOrdenada(listaPeorCaso, valorBusqueda);
        fin = obtenerTiempoActualEnNano();
        tiemposPeorCaso.push_back(fin - inicio);

        // Caso promedio
        list<int> listaCasoPromedio = generarListaCasoPromedio(tam);
        valorBusqueda = tam / 2; // Valor intermedio para el caso promedio
        inicio = obtenerTiempoActualEnNano();
        buscarEnListaOrdenada(listaCasoPromedio, valorBusqueda);
        fin = obtenerTiempoActualEnNano();
        tiemposCasoPromedio.push_back(fin - inicio);
    }
}

// Gráfico de resultados del benchmark
void graficarResultados(QCustomPlot* grafico, const vector<int>& tamanos, const vector<long long>& tiemposMejorCaso, const vector<long long>& tiemposPeorCaso, const vector<long long>& tiemposCasoPromedio) {
    QVector<double> x(tamanos.size()), yMejor(tamanos.size()), yPeor(tamanos.size()), yPromedio(tamanos.size());

    // Llenar los datos
    for (size_t i = 0; i < tamanos.size(); ++i) {
        x[i] = tamanos[i];
        yMejor[i] = tiemposMejorCaso[i] + 1000; // Añadir un valor constante para elevar la línea
        yPeor[i] = tiemposPeorCaso[i];
        yPromedio[i] = tiemposCasoPromedio[i];
    }

    // Graficar mejor caso
    grafico->addGraph();
    grafico->graph(0)->setData(x, yMejor);
    grafico->graph(0)->setPen(QPen(Qt::blue));
    grafico->graph(0)->setName("Mejor Caso O(1)");

    // Graficar peor caso
    grafico->addGraph();
    grafico->graph(1)->setData(x, yPeor);
    grafico->graph(1)->setPen(QPen(Qt::red));
    grafico->graph(1)->setName("Peor Caso O(n)");

    // Graficar caso promedio
    grafico->addGraph();
    grafico->graph(2)->setData(x, yPromedio);
    grafico->graph(2)->setPen(QPen(Qt::green));
    grafico->graph(2)->setName("Caso Promedio O(n)");

    // Ajustar etiquetas y rango de ejes
    grafico->xAxis->setLabel("Tamaño de entrada (n)");
    grafico->yAxis->setLabel("Tiempo (nanosegundos)");

    // Ajustar los rangos
    grafico->xAxis->setRange(0, tamanos.back());
    grafico->yAxis->setRange(0, *max_element(yPeor.begin(), yPeor.end()) + 100);  // Ajustar el rango del eje Y

    // Mostrar leyenda
    grafico->legend->setVisible(true);

    // Reploteo final
    grafico->replot();
}

// Gráfico de comportamiento teórico
void graficarComportamientoTeorico(QCustomPlot* grafico, const vector<int>& tamanos) {
    QVector<double> x(tamanos.size()), yMejor(tamanos.size()), yPeor(tamanos.size()), yPromedio(tamanos.size());

    // Llenar los datos con la complejidad teórica
    for (size_t i = 0; i < tamanos.size(); ++i) {
        x[i] = tamanos[i];
        yMejor[i] = 1;               // O(1)
        yPeor[i] = tamanos[i];        // O(n)
        yPromedio[i] = tamanos[i];    // O(n) caso promedio
    }

    // Graficar mejor caso
    grafico->addGraph();
    grafico->graph(0)->setData(x, yMejor);
    grafico->graph(0)->setPen(QPen(Qt::blue, 2));
    grafico->graph(0)->setName("Mejor Caso (Teórico) O(1)");

    // Graficar peor caso
    grafico->addGraph();
    grafico->graph(1)->setData(x, yPeor);
    grafico->graph(1)->setPen(QPen(Qt::red, 2));
    grafico->graph(1)->setName("Peor Caso (Teórico) O(n)");

    // Graficar caso promedio
    grafico->addGraph();
    grafico->graph(2)->setData(x, yPromedio);
    grafico->graph(2)->setPen(QPen(Qt::green, 2));
    grafico->graph(2)->setName("Caso Promedio (Teórico) O(n)");

    // Ajustar etiquetas y rango de ejes
    grafico->xAxis->setLabel("Tamaño de entrada (n)");
    grafico->yAxis->setLabel("Operaciones");

    // Ajustar los rangos
    grafico->xAxis->setRange(0, tamanos.back());
    grafico->yAxis->setRange(0, tamanos.back());  // Ajustar el rango del eje Y

    // Mostrar leyenda
    grafico->legend->setVisible(true);

    // Reploteo final
    grafico->replot();
}

int main(int argc, char *argv[]) {
    // Realizar los benchmarks
    vector<int> tamanosEntrada = {100, 1000, 5000, 10000, 50000}; // Tamaños de entrada
    vector<long long> tiemposMejorCaso, tiemposPeorCaso, tiemposCasoPromedio;

    ejecutarPruebas(tamanosEntrada, tiemposMejorCaso, tiemposPeorCaso, tiemposCasoPromedio);

    // Crear la aplicación Qt
    QApplication aplicacion(argc, argv);

    // Gráfica de resultados del benchmark
    QCustomPlot graficoResultados;
    graficoResultados.legend->setVisible(true);
    graficarResultados(&graficoResultados, tamanosEntrada, tiemposMejorCaso, tiemposPeorCaso, tiemposCasoPromedio);
    graficoResultados.resize(800, 600);
    graficoResultados.show();

    // Gráfica de comportamiento teórico
    QCustomPlot graficoTeorico;
    graficoTeorico.legend->setVisible(true);
    graficarComportamientoTeorico(&graficoTeorico, tamanosEntrada);
    graficoTeorico.resize(800, 600);
    graficoTeorico.show();

    return aplicacion.exec();
}

