#include <iostream>
#include <vector>
#include <algorithm>
#include <utility>
#include <string>
#include <sstream>
#include <random>
#include <boost/math/constants/constants.hpp>
#include <cmath>
#include <chrono>
#include <list>
#include <QtCore>
#include <QtWidgets>
#include <QtCharts>
#include "chart.h"

using namespace std;

#define COUNT_OF_GAMMA 1000

template<typename T>
void hist1(vector<T>, int);
int len_num(int);

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QMainWindow window;
    MyWidget *my_wgt = new MyWidget(); 
    my_wgt->genChart();
    window.setCentralWidget(my_wgt);
    window.show();
    return app.exec();
}



template<typename T>
void hist1(vector<T> v, int column) {
    int size = v.size();
    int max_len_hist = 100;
    T max_v, min_v, current_k, k;
    // k - отражает сколько значений входит в каждый интервал (стлбик)
    int i, j;
    double m , max_fx;

    auto minmax_pair = minmax_element(v.begin(), v.end());
    min_v = *(minmax_pair.first);
    max_v = *(minmax_pair.second);

    max_fx = 0;

    double step = (max_v - min_v) / static_cast<double>(column);
    k = static_cast<int>(step);
    if(step-k > 0) {
        k++;
        column = (max_v-min_v)/k;
    }

    int *fx = new int[column]{};
    for(i = 0; i < size; i++) {
        for(current_k = min_v, j = 0; (current_k < max_v) && (v[i] >= current_k); current_k+=k, j++) { }
        fx[j-1] += 1;
        if(fx[j-1] > max_fx)  max_fx = fx[j-1];
    }

    m = (max_len_hist / max_fx);

    qDebug() << "2: " << step;
    for(int i = 0; i < column; ++i) {
        qDebug() << fx[i];
    }
    for(i = 0; i < column; i++) {
        fx[i] *= m;
        ostringstream str;
        str << "[" << min_v+i*k << "; " << min_v+(i+1)*k <<")";
        string s(str.str());
        cout << s;
        for(j = 0; j < static_cast<int>(10-s.size()); j++) {
            std::cout << " ";
        }
        std::cout << "|";
        for(j = 0; j < fx[i]; j++) {
            std::cout << "-";
        }
        
        std::cout << std::endl;
    }

}

int len_num(int x) {
    int i = 1;
    x/=10;
    while(x) {
        x/=10;
        i++;
    }
    return i;
}



