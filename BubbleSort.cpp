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

// Función para obtener el tiempo actual en nanosegundos
long long obtenerTiempoActualNano() {
    return duration_cast<nanoseconds>(high_resolution_clock::now().time_since_epoch()).count();
}

// Implementación del algoritmo de ordenamiento Burbuja
void ordenarBurbuja(vector<int>& datos) {
    int longitud = datos.size();
    for (int i = 0; i < longitud - 1; i++) {
        bool huboIntercambio = false;
        for (int j = 0; j < longitud - i - 1; j++) {
            if (datos[j] > datos[j + 1]) {
                swap(datos[j], datos[j + 1]);
                huboIntercambio = true;
            }
        }
        if (!huboIntercambio) break; // Si no se hicieron intercambios, los datos ya están ordenados
    }
}

// Generar datos en el mejor caso (ordenados)
vector<int> generarMejorCaso(int tamano) {
    vector<int> datos(tamano);
    for (int i = 0; i < tamano; i++) {
        datos[i] = i;
    }
    return datos;
}

// Generar datos en el peor caso (orden inverso)
vector<int> generarPeorCaso(int tamano) {
    vector<int> datos(tamano);
    for (int i = 0; i < tamano; i++) {
        datos[i] = tamano - i;
    }
    return datos;
}

// Generar datos en un caso promedio (aleatorio)
vector<int> generarCasoPromedio(int tamano) {
    vector<int> datos(tamano);
    for (int i = 0; i < tamano; i++) {
        datos[i] = i;
    }
    random_device rd;
    mt19937 generador(rd());
    shuffle(datos.begin(), datos.end(), generador);
    return datos;
}

// Ejecutar las pruebas de rendimiento (benchmarks) para los diferentes casos
void ejecutarPruebas(const vector<int>& tamanos, vector<long long>& tiemposMejorCaso, vector<long long>& tiemposPeorCaso, vector<long long>& tiemposPromedio) {
    for (int tamano : tamanos) {
        // Mejor caso
        vector<int> mejorCaso = generarMejorCaso(tamano);
        long long inicio = obtenerTiempoActualNano();
        ordenarBurbuja(mejorCaso);
        long long fin = obtenerTiempoActualNano();
        tiemposMejorCaso.push_back(fin - inicio);

        // Peor caso
        vector<int> peorCaso = generarPeorCaso(tamano);
        inicio = obtenerTiempoActualNano();
        ordenarBurbuja(peorCaso);
        fin = obtenerTiempoActualNano();
        tiemposPeorCaso.push_back(fin - inicio);

        // Caso promedio
        vector<int> casoPromedio = generarCasoPromedio(tamano);
        inicio = obtenerTiempoActualNano();
        ordenarBurbuja(casoPromedio);
        fin = obtenerTiempoActualNano();
        tiemposPromedio.push_back(fin - inicio);
    }
}

// Función para graficar los resultados de las pruebas
void graficarResultados(QCustomPlot* customPlot, const vector<int>& tamanos, const vector<long long>& tiemposMejorCaso, const vector<long long>& tiemposPeorCaso, const vector<long long>& tiemposPromedio) {
    QVector<double> ejeX(tamanos.size()), tiemposMejor(tamanos.size()), tiemposPeor(tamanos.size()), tiemposProm(tamanos.size());

    // Cargar los datos
    for (size_t i = 0; i < tamanos.size(); ++i) {
        ejeX[i] = tamanos[i];
        tiemposMejor[i] = tiemposMejorCaso[i] + 10000000; // Ajuste para elevar la línea
        tiemposPeor[i] = tiemposPeorCaso[i];
        tiemposProm[i] = tiemposPromedio[i];
    }

    // Graficar mejor caso
    customPlot->addGraph();
    customPlot->graph(0)->setData(ejeX, tiemposMejor);
    customPlot->graph(0)->setPen(QPen(Qt::blue));
    customPlot->graph(0)->setName("Mejor Caso O(n)");

    // Graficar peor caso
    customPlot->addGraph();
    customPlot->graph(1)->setData(ejeX, tiemposPeor);
    customPlot->graph(1)->setPen(QPen(Qt::red));
    customPlot->graph(1)->setName("Peor Caso O(n^2)");

    // Graficar caso promedio
    customPlot->addGraph();
    customPlot->graph(2)->setData(ejeX, tiemposProm);
    customPlot->graph(2)->setPen(QPen(Qt::green));
    customPlot->graph(2)->setName("Caso Promedio O(n^2)");

    // Etiquetas de los ejes
    customPlot->xAxis->setLabel("Tamaño de entrada (n)");
    customPlot->yAxis->setLabel("Tiempo (nanosegundos)");

    // Ajustar rangos de los ejes
    customPlot->xAxis->setRange(0, tamanos.back());
    customPlot->yAxis->setRange(0, *max_element(tiemposPeor.begin(), tiemposPeor.end()) + 100);  // Ajuste del eje Y

    // Mostrar leyenda
    customPlot->legend->setVisible(true);

    // Redibujar la gráfica
    customPlot->replot();
}

// Graficar el comportamiento teórico de la complejidad temporal
void graficarComportamientoTeorico(QCustomPlot* customPlot, const vector<int>& tamanos) {
    QVector<double> ejeX(tamanos.size()), teoricoMejor(tamanos.size()), teoricoPeor(tamanos.size()), teoricoProm(tamanos.size());

    // Cargar los datos con la complejidad teórica
    for (size_t i = 0; i < tamanos.size(); ++i) {
        ejeX[i] = tamanos[i];
        teoricoMejor[i] = tamanos[i];         // O(n)
        teoricoPeor[i] = pow(tamanos[i], 2);  // O(n^2)
        teoricoProm[i] = pow(tamanos[i], 2);  // O(n^2) en promedio
    }

    // Graficar mejor caso
    customPlot->addGraph();
    customPlot->graph(0)->setData(ejeX, teoricoMejor);
    customPlot->graph(0)->setPen(QPen(Qt::blue, 2));
    customPlot->graph(0)->setName("Mejor Caso (Teórico) O(n)");

    // Graficar peor caso
    customPlot->addGraph();
    customPlot->graph(1)->setData(ejeX, teoricoPeor);
    customPlot->graph(1)->setPen(QPen(Qt::red, 2));
    customPlot->graph(1)->setName("Peor Caso (Teórico) O(n^2)");

    // Graficar caso promedio
    customPlot->addGraph();
    customPlot->graph(2)->setData(ejeX, teoricoProm);
    customPlot->graph(2)->setPen(QPen(Qt::green, 2));
    customPlot->graph(2)->setName("Caso Promedio (Teórico) O(n^2)");

    // Etiquetas de los ejes
    customPlot->xAxis->setLabel("Tamaño de entrada (n)");
    customPlot->yAxis->setLabel("Operaciones");

    // Ajustar rangos de los ejes
    customPlot->xAxis->setRange(0, tamanos.back());
    customPlot->yAxis->setRange(0, pow(tamanos.back(), 2));  // Ajuste del eje Y

    // Mostrar leyenda
    customPlot->legend->setVisible(true);

    // Redibujar la gráfica
    customPlot->replot();
}

int main(int argc, char *argv[]) {
    // Configurar los tamaños de las pruebas
    vector<int> tamanos = {100, 1000, 5000, 10000, 50000};
    vector<long long> tiemposMejorCaso, tiemposPeorCaso, tiemposPromedio;

    // Ejecutar las pruebas de rendimiento
    ejecutarPruebas(tamanos, tiemposMejorCaso, tiemposPeorCaso, tiemposPromedio);

    // Iniciar la aplicación gráfica
    QApplication app(argc, argv);

    // Gráfica de resultados de las pruebas
    QCustomPlot graficoResultados;
    graficoResultados.legend->setVisible(true);
    graficarResultados(&graficoResultados, tamanos, tiemposMejorCaso, tiemposPeorCaso, tiemposPromedio);
    graficoResultados.resize(800, 600);
    graficoResultados.show();

    // Gráfica del comportamiento teórico
    QCustomPlot graficoTeorico;
    graficoTeorico.legend->setVisible(true);
    graficarComportamientoTeorico(&graficoTeorico, tamanos);
    graficoTeorico.resize(800, 600);
    graficoTeorico.show();

    return app.exec();
}

