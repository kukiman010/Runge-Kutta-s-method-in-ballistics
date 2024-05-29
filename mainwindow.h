#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <iostream>
#include <cmath>
#include <QStandardItemModel>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QDebug>

#include "graphwidget.h"

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

    void PRINT();

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;

    double Cx_43(double n);
    void f_650();
    void f_320();
    void f_380();
    void f_405();
    void runge();
    int INT(double n);

    void addRow(const double T, const double V, const double X, const double H, const double TETA, const double cx1);
    void addPointWithText(double x, double y, QString text = "");


    double R0, A, T4, V1, T5, W0, S, D, i43, MX, CX, CX1, H, H0, Q, H1;
    double K4;
    double X[8];
    double T[10];
    double F[4];
    double E0,E5, X_X;
    double k[4][4];
    double T1,J1,Q3,T3,Q1,P,K2,K3,K;


    QStandardItemModel* _model;
    QGraphicsScene *    _scene;

    QPointF             _lastPoint;
    double x_max;
    double y_max;

    int _squareSize;
    int _wight_line;
    int _height_line;
};

#endif // MAINWINDOW_H
