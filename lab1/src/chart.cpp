#include "chart.h"
#include <boost/math/constants/constants.hpp>
#include <cmath>


long double gamma_f(double x, double a) {
    return (pow(x, a-1) / (pow(2, a) * tgammal(a))) * exp(-x/2);
}


MyWidget::MyWidget(QWidget *parent) : 
    QWidget(parent) {
    QHBoxLayout *hbl1, *hbl2, *hbl3, *hbl4, *hbl5, *hbl6;
    QLabel *lbl_a, *lbl_b, *lbl_size, *lbl_column;
    QVBoxLayout *vbl1;
    QWidget *wgt1;


    chartViewHist = new QChartView();
    chartViewDist = new QChartView();
    lineEdit_a = new QLineEdit();
    lineEdit_b = new QLineEdit();
    lineEdit_size = new QLineEdit();
    lineEdit_column = new QLineEdit();
    btn = new QPushButton("generate!");

    vbl1 = new QVBoxLayout();

    hbl1 = new QHBoxLayout();
    hbl2 = new QHBoxLayout();
    hbl3 = new QHBoxLayout();
    hbl4 = new QHBoxLayout();
    hbl5 = new QHBoxLayout();
    hbl6 = new QHBoxLayout();

    lbl_a = new QLabel("a: ");
    lbl_b = new QLabel("b: ");
    lbl_size = new QLabel("size: ");
    lbl_column = new QLabel("column: ");
    lbl_hi = new QLabel("");
    wgt1 = new QWidget();

    lineEdit_a->setText(QString::number(1));
    lineEdit_b->setText(QString::number(1));
    lineEdit_size->setText(QString::number(1000));
    lineEdit_column->setText(QString::number(column));


    lineEdit_a->setMaximumWidth(60);
    lineEdit_b->setMaximumWidth(60);
    lineEdit_size->setMaximumWidth(60);
    lineEdit_column->setMaximumWidth(60);
    btn->setMaximumWidth(50);
    btn->setMaximumSize(QSize(100, 40));
    wgt1->setMaximumSize(QSize(200, 200));
    
    hbl1->addWidget(lbl_a);
    hbl1->addWidget(lineEdit_a);
    hbl2->addWidget(lbl_b);
    hbl2->addWidget(lineEdit_b);
    hbl3->addWidget(lbl_size);
    hbl3->addWidget(lineEdit_size);
    hbl4->addWidget(lbl_column);
    hbl4->addWidget(lineEdit_column);

    hbl5->addWidget(btn);

    vbl1->addLayout(hbl1);
    vbl1->addLayout(hbl2);
    vbl1->addLayout(hbl3);
    vbl1->addLayout(hbl4);
    vbl1->addLayout(hbl5);
    vbl1->addWidget(lbl_hi);

    wgt1->setLayout(vbl1);

    hbl6->addWidget(wgt1);
    hbl6->addWidget(chartViewHist);
    hbl6->addWidget(chartViewDist);
    chartViewHist->setMinimumSize(QSize(600,400));
    chartViewDist->setMinimumSize(QSize(600,400));

    QObject::connect(btn, SIGNAL(clicked()), SLOT(genChart()));
    this->setLayout(hbl6);
}

vector<long double> MyWidget::stat() {
    double temp;
    vector<long double> hist_t;
    double sum_for_mx = 0, sum_for_dx;
    vector<double> hist_multiple_meanx; 
    vector<double> hist_multiple_double_meanx;
    for(int i = 0; i < column; ++i) {
        temp = v_min + step/2 + step*i;
        hist_multiple_meanx.push_back(temp * hist[i]);
        hist_multiple_double_meanx.push_back(temp*temp * hist[i]);
        sum_for_mx += hist_multiple_meanx[i];
        sum_for_dx += hist_multiple_double_meanx[i];
    }
    mx_stat = sum_for_mx / v_size;
    dx_stat = sum_for_dx / v_size - mx_stat * mx_stat;
    // mx = a_stat / b_stat
    // dx = a_stat / b_stat^2
    qDebug() << "mx = " << mx_stat;
    qDebug() << "dx = " << dx_stat;
    b_stat = mx_stat / dx_stat;
    a_stat = mx_stat * b_stat;
    qDebug() << "a_stat = " << a_stat;
    qDebug() << "b_stat = " << b_stat;
    qDebug() << "step = " << step;
    
    
    for(int i = 0; i < column ; ++i) {
        long double t = v_min + step/2 + step*i;
        long double x = 2 * b_stat * t;
        hist_t.push_back(x/t * gamma_f(x, a_stat) * step);
    }
    return hist_t;
}

vector<double> MyWidget::gen_dist() {
    vector<double> v_dist;
    v_dist.resize(column);
    double v_dist_temp = 0, v_dist_teor_temp = 0;
    for(int i = 0; i < column; i++) {
        v_dist_temp += hist[i];
        v_dist[i] = v_dist_temp;

        v_dist_teor_temp  += hist_teor[i];
        dist_teor.push_back(v_dist_teor_temp);
    }
    return v_dist;
}

vector<double> MyWidget::generate_g_vector(int size, double a, double b) {
    /**
     * Генерация выборки значений Г(a, b)
     */
    vector<double> v;
    v_size = size;
    for(int i = 0; i < size; i++) {
        v.emplace_back(generate_g(a, b));
    }
    return v;
}

double MyWidget::generate_g(double a, double b) {
    /**
     * Генерирует единственное значение Г(a, b)
     * a > 0
     * b > 0 
     */
    int a_int = a;
    double a_frac = a - a_int;
    double eps{};
    mt19937 mt_engine(chrono::system_clock::now().time_since_epoch().count());
    uniform_real_distribution<> uniform_dist;
    if (a_frac > 0) {
        eps = m(a_frac);
    } 
    double ln{};
    for(int i = 0; i < a_int; i++) {
        ln += log(uniform_dist(mt_engine));
    }
    return 1/b * (eps - ln);
}

double MyWidget::m(double d_frac) {
    /**
     * Генерирует Г(d_frac,1)
     * 0 < d_frac < 1
     * Используется только в тех случаях, где a из Г(a, b) НЕ ЦЕЛОЕ
     * В других случаях этот метод скип  
    */
    double nu;
    double e = boost::math::double_constants::e;
    double v1, v2;
    mt19937 mt_engine(chrono::system_clock::now().time_since_epoch().count());

    double p = e/(e+d_frac);
    double a = 1/d_frac;
    double d = p/(1-p);

    uniform_real_distribution<> uniform_dist;

    int counter{};
    while(true) {
        qDebug() << ++counter << " start;";
        v1 = uniform_dist(mt_engine);
        v2 = uniform_dist(mt_engine);
        if(v1 < p) {
            nu = pow(v1/p, a);
        } else {
            nu = -log(d*(v1/p - 1));
        }
        if( (nu <= 1 && exp(-nu) < v2 ) || (nu > 0 && pow(nu, (d_frac - 1)) < v2)   ) {
            qDebug() << counter << " end; nu = " << nu;
            return nu;
        }
    }
}

vector<double> MyWidget::part_hist(vector<double>& v) {
    /**
     * Создает новый вектор, этот вектор является разделением выборки по диапазонам
     * Единственная задача этого метода - дать входные данные для метода построения гистограммы hist
     */
    int size = v.size();
    auto minmax_pair = minmax_element(v.begin(), v.end());
    v_min = *(minmax_pair.first);
    v_max = *(minmax_pair.second);

    step = (v_max - v_min) / column;

    vector<double> hist{};
    int temp{};
    sort(v.begin(), v.end());
    double upp_bound = v_min + step;
    for(int j = 0; j < size; ++j) {
        if(v[j] > upp_bound) {
            hist.emplace_back(temp);
            temp = 0;
            upp_bound += step;
        }
        temp += 1; 
    }  
    return hist;
}

QChart* MyWidget::generate_hist(vector<double>& hist, 
    vector<long double>& f,double zoom) {
    /**
     * Генерирует гистограмму по заданной выборке
     * Полностью завершает создание chartView
     */
    QChart *chart = new QChart();
    QBarSet *barset = new QBarSet("");
    QBarSeries *barseries = new QBarSeries();
    double max_hist = *max_element(hist.begin(), hist.end());
    for(int i = 0; i < column; ++i) {
        *barset << (hist[i] / static_cast<double>(zoom));
    }
    barseries->append(barset);
    chart->addSeries(barseries);

    QValueAxis *axisY = new QValueAxis();
    QValueAxis *axisX = new QValueAxis();
    chart->addAxis(axisY, Qt::AlignLeft);
    chart->addAxis(axisX, Qt::AlignBottom);
    barseries->attachAxis(axisY);

    axisY->setRange(0, (max_hist / static_cast<double>(zoom)));
    axisX->setRange(v_min, v_max);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
    QLineSeries *lineseries = new QLineSeries();
    QList<QPointF> points;
    points.push_back(QPointF(0, f[0]));
    for(int i = 0; i < column; ++i) {
        QPointF point(i+0.5, f[i]);
        points.push_back(point);
    }
    points.push_back(QPointF(column, f[column-1]));
    lineseries->append(points);
    chart->addSeries(lineseries);
    return chart;
}


void MyWidget::genChart() {
    a = (lineEdit_a->text()).toDouble();
    b = (lineEdit_b->text()).toDouble();
    v_size = (lineEdit_size->text()).toInt();
    column = (lineEdit_column->text()).toInt();
    if(a <= 0 || b <= 0 || v_size <= 0 || column <= 0) {
        QDialog *dial = new QDialog();
        QHBoxLayout *hbl = new QHBoxLayout();
        QLabel *lbl = new QLabel("Incorrect Data!");
        hbl->addWidget(lbl);
        dial->setLayout(hbl);
        dial->setModal(true);
        dial->setWindowTitle("ERROR!");
        dial->show();
        return;
    }
    v = generate_g_vector(v_size, a, b);
    hist = part_hist(v);
    hist_teor = stat(); 
    dist_teor.clear();
    chartViewHist->setChart(generate_hist(hist, hist_teor, v_size));
    chartViewHist->setRenderHint(QPainter::Antialiasing);
    dist = gen_dist();
    chartViewDist->setChart(generate_hist(dist, dist_teor, dist[column-1]));
    chartViewDist->setRenderHint(QPainter::Antialiasing);
    if(hi()) {
        lbl_hi->setText("Гипотеза принимается!");
    } else {
        lbl_hi->setText("Гипотеза отвергается!");
    }
}

bool MyWidget::hi() {
    double hi_q = 0;
    for(int i = 0; i < column; ++i) {
        hi_q += pow(hist[i] - hist_teor[i] * v_size, 2)/ (hist_teor[i] * v_size);
    }
    qDebug() << hi_q;
    return hi_q < 23.7;
}
