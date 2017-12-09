#ifndef PAINTERWIDGET_H
#define PAINTERWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QGraphicsView>
#include <QLabel>
#include <QScrollArea>
#include <QItemDelegate>
#include <QPainter>
#include <QPalette>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QGraphicsItem>
#include <QTableWidget>
#include <QDebug>
#include <vector>

namespace Ui {
class PainterWidget;
}

class PainterWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PainterWidget(QWidget *parent = 0);
    ~PainterWidget();

    QImage* image;
    void draw();
    void drawLeftFixator(QPainter* painter);
    void drawRightFixator(QPainter* painter);
	void drawBars(QPainter* painter);
	void drawForces(QPainter* painter);

	void setNodes(std::vector<double> _nodes);
    void setBars(std::vector<std::vector<double>> _bars);
    void setForces(std::vector<std::vector<float>> _forces);
    void setFixators(bool _leftFixator, bool _rightFixator);
    void setScale(float _scale);
	void clearArea();
private:
    Ui::PainterWidget *ui;
	std::vector<double> nodes;
	std::vector<double> nodesc;
    std::vector<std::vector<double>> bars;
    std::vector<std::vector<float>> forces;
	std::vector<std::vector<double>> barsc;
    bool leftFixator = false;
    bool rightFixator = false;
    double scale = 1;
    int yAxie;
    int lastPos;
    QGraphicsScene *scene;
    QRect percentRect(float x1, float y1, float x2, float y2);
    QPoint percentPoint(float x, float y);
protected:

};

#endif // PAINTERWIDGET_H
