#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QGraphicsEllipseItem>
#include <QPen>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _model(new QStandardItemModel(0, 6, this))
    // _plotter(new GraphPlotter(this))
{
    ui->setupUi(this);

    _model->setHeaderData(0, Qt::Horizontal, QObject::tr("T Время"));
    _model->setHeaderData(1, Qt::Horizontal, QObject::tr("V Скорость"));
    _model->setHeaderData(2, Qt::Horizontal, QObject::tr("X Дистанция"));
    _model->setHeaderData(3, Qt::Horizontal, QObject::tr("H Высота"));
    _model->setHeaderData(4, Qt::Horizontal, QObject::tr("θ угол"));
    _model->setHeaderData(5, Qt::Horizontal, QObject::tr("CX1 Ускорение"));
    ui->tableView->setModel(_model);

    _scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(_scene);

    // _graphWidget = new GraphWidget(ui->widget);
    // _graphWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->widget->setVisible(false);


    x_max = 10000, y_max = 5000;
    // _scene->setSceneRect(0, 0, 1200, 600);
    // QPen axisPen(Qt::black, 3);
    // _scene->addLine(50, 550, 1150, 550, axisPen);
    // _scene->addLine(50, 550, 50, 50, axisPen);

    // pen = QPen(Qt::blue, 4);
    // linePen = QPen(Qt::red, 2);
    // previousPoint = QPointF();


/*
     y/height
    ^
    |
    |
    ------>
          x / width
*/



    ui->graphicsView->setMinimumHeight(300);
    ui->graphicsView->setMinimumWidth(500);

    int y_size = ui->graphicsView->height();
    int x_size = ui->graphicsView->width();

    int y_max = 5000;
    int x_max = 10000;

    int y_km = y_max / 1000;
    int x_km = x_max / 1000;

    int y_km_to_pix = y_size / y_km;
    int x_km_to_pix = x_size / x_km;

    _squareSize = ( x_km_to_pix + y_km_to_pix) /2;

    _scene->setSceneRect(x_size, y_size, 0,0);

    _wight_line = _squareSize * x_km;
    _height_line = _squareSize * y_km;
    _lastPoint = QPointF(0,_height_line);

    for (int x=0, x_c=0; x <= x_km; x++, x_c += _squareSize)
    {
        if (x == 0)
            _scene->addLine(0, 0, 0, _height_line, QPen(Qt::black, 3));
        else
        {
            _scene->addLine(x_c, 0, x_c, _height_line, QPen(Qt::lightGray));
            auto* text = _scene->addText(QString::number(1000 * x));
            text->setPos(x_c - text->boundingRect().width() / 2, _height_line - text->boundingRect().height());
        }
    }

    for (int y=0, y_c=0; y <= y_km; y++, y_c += _squareSize)
    {
        if (y == 0)
        {
            _scene->addLine(0, _height_line, _wight_line, _height_line, QPen(Qt::black, 3));
            auto* text = _scene->addText(QString::number(y));
            text->setPos(-text->boundingRect().width(), _height_line - y_c - text->boundingRect().height() / 2);
        }
        else
        {
            _scene->addLine(0, _height_line - y_c, _wight_line, _height_line - y_c , QPen(Qt::lightGray));
            auto* text = _scene->addText(QString::number(1000 * y));
            text->setPos(-text->boundingRect().width(), _height_line - y_c - text->boundingRect().height() / 2);
        }
    }


//    e = 2.71828;
//    g= 9.81;
//    t= 0.1;
    E0 = 0.01;
    E5 = 0.1;

    X[0] = 0;
    X[1] = 0;
    X[2] = 0;
    X[3] = 0;

    H0 = 1;
    H1 = 1;

    i43 = 0.8379;
    D = 0.12;
    W0 = 0;
    T4 = 0;

    T[0] = 0;
    K = 1;
    K4 = 0;
    //T[0] = H1;//T(1)
    H = H0;

    T1= 10;
    J1 = 200;
    Q3 = 1;
    T3 = 1000;

    T[2] = H1;
    K2 = 0;
    K3 = 0;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    D = ui->doubleSpinBox->value();
    X[0] = ui->doubleSpinBox_2->value();
    X[3] = ui->doubleSpinBox_3->value();
    Q = ui->doubleSpinBox_4->value();
    i43 = ui->doubleSpinBox_5->value();

//    float density_table[3][18]= {{0,250,500,750,1000,1500,2000,2500,3000,3500,4000,4500,5000,6000,7000,8000,9000,10000},//высота в метрах
//                                 {1.22,1.20,1.17,1.14,1.11,1.06,1.00,0.96,0.91,0.86,0.82,0.78,0.74,0.66,0.59,0.53,0.47,0.41},//давление
//                                 {15,13,12,10,8,5,2,-1,-4,-8,-11,-14,-18,24,-30,-37,-44,-50}};// температура
    X[3] = X[3] / 180 * M_PI; // tetta (угол стрельбы) to radian
    S = M_PI * pow(D,2) / 4;

    f_650();

    for(int i=0;i<45;i++)
        f_320();

    cout<<"T5 "<<T5<<"\nR0 "<<R0<<"\nA "<<A<<"\nV1 "<<V1 <<"\nMX "<<MX <<"\nCX "<<CX<<"\n"<<endl;
}


double MainWindow::Cx_43(double n)
{
//    float a[2][40]= {{0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0,1.1,1.2,1.3,1.4,1.5,1.6,1.7,1.8,1.9,2.0,2.1,2.2,2.3,2.4,2.5,2.6,2.7,2.8,2.9,3.0,3.1,3.2,3.3,3.4,3.5,3.6,3.7,3.8,3.9,4.0},
//               {0.157,0.158,0.158,0.158,0.157,0.157,0.158,0.160,0.190,0.325,0.378,0.385,0.381,0.371,0.361,0.351,0.342,0.332,0.324,0.316,0.309,0.303,0.297,0.292,0.287,0.279,0.278,0.278,0.277,0.270,0.267,0.265,0.263,0.263,0.261,0.261,0.260,0.260,0.259,0.26}};

    float a[2][40] = {{0.001,0.25,0.5,0.6,0.7,0.75,0.8,0.85,0.9,0.95,0.975,1,1.05,1.1,1.15,1.2,1.25,1.3,1.35,1.4,1.5,1.6,1.8,2.2,2.5,3,5,10,20},
                      {0.157,0.157,0.157,0.157,0.157,0.158,0.159,0.165,0.184,0.235,0.285,0.323,0.36,0.377,0.384,0.382,0.38,0.376,0.371,0.362,0.351,0.332,0.317,0.3,0.288,0.271,0.26,0.26,0.26, }};

    for(int i=0;i<40;i++)
    {
        if(a[0][i] >= n)
        {
            if(a[0][i] == n)
                return a[1][i];
            else
                return a[1][i-1];
        }
    }
   return a[1][39];
}

void MainWindow::PRINT()
{
     addRow( T[0], X[0], X[1], X[2], X[3], CX1);

     // _graphWidget->addPoint(X[1], X[2], QString::number(T[0]) );
     // test_add(X[1], X[2]);
     addPointWithText(X[1], X[2]);

     H = H0;
}

void MainWindow::f_650()
{
    Q1 = Q - Q3 / T1 * (T[0] - T3) * K;
    P = Q3 * J1 / T1 * K;

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
        CX = Cx_43(MX);
    else
        CX =0.157;

    CX1 = CX * i43 - CX * 0.1 * i43 * K;
    X_X = CX1 * S * R0 * V1 * V1 / 2;

    F[0] = 9.80665 / Q1 * (P - X_X - Q1 * sin(X[3]));
    F[1] = X[0] * cos(X[3]);
    F[2] = X[0] * sin(X[3]);
    F[3] = -9.80665 * (X_X * W0 * sin(X[3]) / (Q1 * V1) + cos(X[3])) / X[0];
}

void MainWindow::f_320()
{
    if(K2 == 1)
    {
        if(abs(X[2] < E0))//350
        {
            PRINT();
            ui->label_info->setText("endl programm!!");
        }
        else
        {
            H = X[2] * H /(X[6] - X[2]);
            K2 = 1;
            runge();
        }
    }
    else if(K3 == 1)
    {}
    else if(X[3]>0)
        f_380();
    else
    {
        if(K4 == 1)
        {
            if(X[2] > 0)
                f_380();
            else
            {
                if(abs(X[2] < E0))//350
                {
                    PRINT();
                    ui->label_info->setText("endl programm!!");
                }
                else
                {
                    H = X[2] * H /(X[6] - X[2]);
                    K2 = 1;
                    runge();
                }
            }
        }
        else if(abs(X[3]) < E0)
        {
            K4 =1;
            PRINT();
            runge();
        }
        else
        {
            H = -H/2;
            runge();
        }
    }
}

void MainWindow::runge()
{
    for (int i=0;i<4;i++)//runge
        X[4 + i]=X[i];

    f_650();

    for (int i=0;i<4;i++)
        k[0][i] = F[i];

    double H2 = H / 2;
    T[0] = T[0] + H2;

    for(int L=0;L<3;L++)
    {
        int g =L;
        ++g;
        T[0] = T[0] + INT(g/3) * H2;

        for (int i=0;i<4;i++)
            X[i] = X[4 + i] + H2 * k[L][i] * INT(g/3 + 1);

        f_650();
        for (int j=0;j<4;j++)
            k[g][j] = F[j];
    }
    for (int i=0;i<4;i++)
        X[i] = X[4 + i] + H / 6 * (k[0][i] + 2 * k[1][i] + 2 * k[2][i] + k[3][i]);
}

void MainWindow::f_380()
{
    if(abs(T[0] - T[1])<E0)//380
    {
        T[1] = T[1] + H1;//390
        PRINT();//400
        f_405();
    }
    else if(T[0]+H<T[1])
    {
        f_405();
    }
    else
    {
        H = T[1] - T[0];
        f_405();
    }
}

int MainWindow::INT(double n)
{
    return n;
}

void MainWindow::f_405()
{
    if(abs(X[1] - 4940) < E5)//405
    {
        PRINT();
        runge();
    }
    else
    {
        if(abs(T[0] - T[2]) < E0)//410
        {
               if(K == 0)//420
               {
                   T[2] = T[2] + T1;//430
                   K =1;
                   PRINT();
                   runge();
               }
               else
               {
                   K = 0;
                   Q =Q1;
                   T[2] = T[2]+1000;
                   PRINT();
                   runge();
               }
        }
        else if(T[0] + H < T[2])
            runge();
        else
        {
            H = T[2] - T[0];
            runge();
        }
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    for(int i =0;i<4;i++)
        X[i] = 0;

    H0 = 1;
    H1 = 1;

    i43 = 0.8379;
    D = 0.12;
    W0 = 0;
    T4 = 0;

    T[0] = 0;
    K4 = 0;
    //T[0] = H1;//T(1)
    H = H0;
    ui->label_info->clear();
    _scene->clear();

    if(_model)
        _model->removeRows(0, _model->rowCount());
}

void MainWindow::addRow(const double T, const double V, const double X, const double H, const double TETA, const double CX1)
{
    QList<QStandardItem *> items;

    items.append(new QStandardItem(QString::number( T )));
    items.append(new QStandardItem(QString::number( V )));
    items.append(new QStandardItem(QString::number( X )));
    items.append(new QStandardItem(QString::number( H )));
    items.append(new QStandardItem(QString::number( TETA / M_PI *180 )));
    items.append(new QStandardItem(QString::number( CX1 )));

    _model->appendRow(items);
}

void MainWindow::addPointWithText(double x, double y, QString text)
{
    const QColor& pointColor = Qt::green;
    const QColor& lineColor = Qt::red;
    int ballSize = 8;

    double x_toPix = (x / 1000) * _squareSize;
    double y_toPix = (y / 1000) * _squareSize;

    double d_x = 0.0, d_y= 0.0;
    d_y = (_height_line - (ballSize/2)) - y_toPix;
    d_x = (ballSize/2) + x_toPix;

    QPointF point(d_x, d_y);
    _scene->addEllipse(point.x()-8, point.y(), ballSize, ballSize, QPen(pointColor), QBrush(pointColor));


    if( text.isEmpty())
    {
        text = "(%1, %2)";
        text = text.arg(x).arg(y);
    }

    QGraphicsTextItem* textItem = _scene->addText( text );
    textItem->setDefaultTextColor(Qt::black);
    textItem->setRotation(90);
    textItem->setPos(point.x()+15, point.y() - 100);

    if (_lastPoint.x() != -1 && _lastPoint.y() != -1)
        _scene->addLine(_lastPoint.x(), _lastPoint.y(), d_x, d_y, QPen(lineColor, 2));

    _lastPoint = point;
}

