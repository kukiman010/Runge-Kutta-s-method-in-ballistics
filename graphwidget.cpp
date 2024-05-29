#include "graphwidget.h"


#include <QWidget>
#include <QPainter>
#include <QPen>
#include <QVector>
#include <QPair>
#include <QPointF>
#include <QString>
#include <cmath>

GraphWidget::GraphWidget(QWidget *parent)
    : QWidget(parent)
{}

void GraphWidget::addPoint(double x, double y, const QString &name)
{
    namedPoints.append(qMakePair(QPointF(x, y), name));
    update();  // Просим перерисовать виджет
}

void GraphWidget::getMinMaxValues(double &minX, double &maxX, double &minY, double &maxY) const
{
    if (namedPoints.isEmpty()) {
        minX = maxX = minY = maxY = 0;
        return;
    }

    minX = maxX = namedPoints[0].first.x();
    minY = maxY = namedPoints[0].first.y();

    for (const auto &pair : namedPoints) {
        const QPointF &pt = pair.first;
        if (pt.x() < minX) minX = pt.x();
        if (pt.x() > maxX) maxX = pt.x();
        if (pt.y() < minY) minY = pt.y();
        if (pt.y() > maxY) maxY = pt.y();
    }
}

void GraphWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // Цвета и линии
    QPen pen(Qt::black);
    painter.setPen(pen);

    // Вычисляем минимальные и максимальные значения для масштабирования
    double minX, maxX, minY, maxY;
    getMinMaxValues(minX, maxX, minY, maxY);

    // Добавим немного отступа со всех сторон
    double margin = 0.1;
    double diffX = maxX - minX;
    double diffY = maxY - minY;

    if (diffX == 0) diffX = 1; // Чтобы не делить на 0
    if (diffY == 0) diffY = 1;

    minX -= margin * diffX;
    maxX += margin * diffX;
    minY -= margin * diffY;
    maxY += margin * diffY;

    double scaleX = (width() - 100) / (maxX - minX);
    double scaleY = (height() - 100) / (maxY - minY);

    double offsetX = 50;
    double offsetY = height() - 50;

    // Debug: выводим информацию о масштабировании и сдвиге
    qDebug() << "ScaleX:" << scaleX << ", ScaleY:" << scaleY;
    qDebug() << "OffsetX:" << offsetX << ", OffsetY:" << offsetY;

    // Рисуем оси
    painter.drawLine(offsetX, offsetY, width() - 50, offsetY); // X ось
    painter.drawLine(offsetX, offsetY, offsetX, 50);          // Y ось

    // Подписи осей
    painter.drawText(width() - 60, height() - 30, "X");
    painter.drawText(30, 40, "Y");

    // Рисуем названные точки
    for (const auto& namedPoint : namedPoints) {
        QPointF point = namedPoint.first;
        QPointF scaledPoint(offsetX + scaleX * (point.x() - minX), offsetY - scaleY * (point.y() - minY));

        // Debug: выводим информацию о точках
        qDebug() << "Point:" << point << ", ScaledPoint:" << scaledPoint;

        painter.setPen(Qt::red);
        painter.drawEllipse(scaledPoint, 3, 3);  // Рисуем точку
        painter.setPen(Qt::black);
        painter.drawText(scaledPoint + QPointF(5, -5), namedPoint.second);  // Добавляем текст
    }
}
