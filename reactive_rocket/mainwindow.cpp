#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

//    Pi = 3.14159265359;
//    e = 2.71828;
//    g= 9.81;
//    t= 0.1;
    E0 = 0.01;
    E5 = 0.1;
    pi = 3.14159265359;

    X[0] = 367;
    X[1] = 0;
    X[2] = 0;
    X[3] = 43.97;// tetta (угол стрельбы)

    H0 = 1;
    H1 = 1;

    i43 = 0.8379;
    D = 0.12;
    W0 = 0;
    T4 = 0;

    T[0] = 0;
    //T[0] = H1;//T(1)
    H = H0;
}

MainWindow::~MainWindow()
{
//    delete O;
//    delete V;
//    delete X;
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{

//    double d, V, O, G, j, y, a, M, T, Cx;

    D = ui->doubleSpinBox->value();
    X[0] = ui->doubleSpinBox_2->value();
    X[3] = ui->doubleSpinBox_3->value();
    Q = ui->doubleSpinBox_4->value();
    i43 = ui->doubleSpinBox_5->value();
//    y = ui->doubleSpinBox_6->value();

    float density_table[3][18]= {{0,250,500,750,1000,1500,2000,2500,3000,3500,4000,4500,5000,6000,7000,8000,9000,10000},//высота в метрах
                                 {1.22,1.20,1.17,1.14,1.11,1.06,1.00,0.96,0.91,0.86,0.82,0.78,0.74,0.66,0.59,0.53,0.47,0.41},//давление
                                 {15,13,12,10,8,5,2,-1,-4,-8,-11,-14,-18,24,-30,-37,-44,-50}};// температура



    X[3] = X[3] / 180 * pi;
    S = pi * pow(D,2) / 4;

    f_650();
    PRINT();
    f_320();


    cout<<"T5 "<<T5<<"\nR0 "<<R0<<"\nA "<<A<<"\nV1 "<<V1 <<"\nMX "<<MX <<"\nCX "<<CX<<"\n"<<endl;
    //ui->statusBar->showMessage(QString::number(X[3]));
}


double MainWindow::Cx_43(double n)
{
    float a[2][40]= {{0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0,1.1,1.2,1.3,1.4,1.5,1.6,1.7,1.8,1.9,
                2.0,2.1,2.2,2.3,2.4,2.5,2.6,2.7,2.8,2.9,3.0,3.1,3.2,3.3,3.4,3.5,3.6,3.7,3.8,3.9,4.0},
               {0.157,0.158,0.158,0.158,0.157,0.157,0.158,0.160,0.190,0.325,0.378,0.385,0.381,0.371,0.361,0.351,0.342,0.332,0.324,
               0.316,0.309,0.303,0.297,0.292,0.287,0.279,0.278,0.278,0.277,0.270,0.267,0.265,0.263,0.263,0.261,0.261,0.260,0.260,0.259,0.260}};

    //n = ui->doubleSpinBox_6->value();

    for(int i=0;i<40;i++)
    {
        if(a[0][i] >= n)
        {
            if(a[0][i] == n)
            {
                return a[1][i];
                break;
            }
            else
            {
                return a[1][i-1];
                break;
            }
        }
    }
   return a[2][40];
}

void MainWindow::PRINT()
{
     cout<<"| "<< T[0] <<" | "<<X[0]<<" | "<<X[1]<<" | "<<X[2]<<" | "<<X[3]<< " |"<<endl;
     ui->label_13->setText(QString::number(T[0]));
     ui->label_14->setText(QString::number(X[0]));
     ui->label_15->setText(QString::number(X[1]));
     ui->label_16->setText(QString::number(X[2]));
     ui->label_17->setText(QString::number(X[3]));
     H = H0;
}

void MainWindow::f_650()
{
    if(X[2]>9300)
    {
        T5 = 221.5 + T4;
        R0 = 0.0372 * 221.5 /(221.5 + T4) * exp(-9.80665 / (287.053 * 221.5) * (X[2] - 10750));
    }
    else
    {
        T5 = 288.9 - 0.006328 * X[2] + T4;
        R0 = 0.12296* 288.9/(288.9+T4)*exp(-(1-9.80665/(0.006328*287.053))*log(T5/(288.9+T4)));
    }

    A = 20.05 * sqrt(T5);
    V1 = sqrt((X[0] * cos(X[3]) - W0) * (X[0] * cos(X[3]) - W0) + X[0] * sin(X[3]) * X[0] * sin(X[3]));
    MX = V1 / A;

    if(MX> 0.73357)
    {
        //ui->statusBar->showMessage("лооол!");
        CX = Cx_43(MX);
    }
    else
        CX =0.157;


    CX1 = CX * i43;
    X_X = CX1 * S * R0 * V1 * V1 / 2;

    F[0] = 9.80665 /(X_X - sin(X[3]));
    F[1] = X[0] * cos(X[3]);
    F[2] = X[0] * sin(X[3]);
    F[3] = -9.80665 * (X_X * W0 * sin(X[3]) / V1 + cos(X[3])) / X[0];
}

void MainWindow::f_320()
{
    if(X[3]>0)
        f_380();

    if(abs(X[3])<E0)
    {}
    else
    {
        H = -H/2;
        //K3 = 0;
        runge();
    }
}

void MainWindow::runge()
{
    for (int i=0;i<4;i++)//runge
        X[4 + i]=X[i];

    f_650();

    for (int i=0;i<4;i++)
        k[0][i] = F[i];

    int H2;
    H2 = H / 2;
    T[0] = T[0] + H2;

    for(int L=0;L<3;L++)// вообще не знаю зачем это -_-
    {
        int INT;
        INT = L / 3;
        T[0] = T[0] + INT * H2;

        for (int i=0;i<4;i++)
            X[i] = X[4 + i] + H2 * k[L][i] * (INT + 1);

        f_650();

        int g =L;
        g++;
        for (int j=0;j<4;j++)
            k[g][j] = F[j];
        g = 0;
    }
    for (int i=0;i<4;i++)
        X[i] = X[4 + 1] + H / 6 * (k[0][i] + 2 * k[1][i] + 2 * k[2][i] + 2 * k[3][i]);
}
void MainWindow::f_380()
{
    if(abs(T[0] - T[1])<E0)
    {
        T[1] = T[1] + H1;
        PRINT();
    }
    else if(T[0]+H<T[1])
    {
    }
    else
    {
        H = T[1] - T[0];
    }

}
void f_405()
{
//    if(abs(X[1] - 4940)<E5)
//    {
//        PRINT();
//        runge();
//    }
//    else
//    {

//    }



}
