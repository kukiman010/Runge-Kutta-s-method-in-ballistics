#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QPen>
#include <QVector>
#include <QPair>
#include <QPointF>
#include <QString>
#include <cmath>

class GraphWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GraphWidget(QWidget *parent = nullptr);

    void addPoint(double x, double y, const QString &name);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QVector<QPair<QPointF, QString>> namedPoints;
    void getMinMaxValues(double &minX, double &maxX, double &minY, double &maxY) const;
};

#endif // GRAPHWIDGET_H
