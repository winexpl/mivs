#pragma once

#include <QtCharts>
#include <vector>
#include <QtCore>
#include <QtWidgets>

using namespace std;

class MyWidget : public QWidget {
    Q_OBJECT
    double a, b;
    int v_size{}, column{15};
    double v_min{}, v_max{}, step{};
    double mx_stat, dx_stat;
    double a_stat, b_stat; // альфа и лямбда
    
    vector<double> v;
    vector<double> hist;
    vector<double> dist;
    vector<long double> hist_teor;
    vector<long double> dist_teor;

    QLabel *lbl_hi;
    QPushButton *btn;
    QLineEdit *lineEdit_a;
    QLineEdit *lineEdit_b;
    QLineEdit *lineEdit_size;
    QLineEdit *lineEdit_column;
    QChartView *chartViewHist;
    QChartView *chartViewDist;

public:
    MyWidget(QWidget *parent = nullptr);
    vector<double> part_hist(vector<double>&);
    QChart* generate_hist(vector<double>&, vector<long double>&, double);
    double m(double);
    double generate_g(double, double);
    vector<double> generate_g_vector(int, double, double);
    vector<double> gen_dist();
    vector<long double> stat();
    bool hi();
public slots:
    void genChart();
};
