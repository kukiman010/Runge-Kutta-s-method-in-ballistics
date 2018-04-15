#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <iostream>
#include <cmath>

using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
    double Cx_43(double n);
    void PRINT();
    void f_650();
    void f_320();
    void f_380();
    void f_405();
    void runge();
    int INT(double n);

    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;

//    double *V;
//    double *O;//тета
//    double *X;
//    int num_V,num_O,num_X;
//    float Pi;
//    float e;
//    float g, t;
    double R0, A, T4, V1, T5, W0, S, D, i43, MX, CX, CX1, H, H0, pi, Q, H1;
    double K4;
    double X[8];
    double T[10];
    double F[4];
    double E0,E5, X_X;
    double k[4][4];

    double T1,J1,Q3,T3,Q1,P,K2,K3,K;
};

#endif // MAINWINDOW_H
