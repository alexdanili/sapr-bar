#ifndef POSTPROCESSOR_H
#define POSTPROCESSOR_H

#include <QtWidgets/QWidget>
#include <QGraphicsView>
#include <qgraphicsitem.h>
#include <QFileDialog>
#include <QTextStream>
#include <QTextCodec>
#include "painterwidget.h"
#include <algorithm>
#include "processor.h"

namespace Ui {
	class PostProcessor;
}
class PostProcessor : public QWidget
{
	Q_OBJECT

public:
	explicit PostProcessor(QWidget *parent = 0);
	~PostProcessor();
	void setProcessor(Processor* _processor);
	float nyscale = 1;
	float nxscale = 1;
	float syscale = 1;
	float sxscale = 1;
	float uyscale = 1;
	float uxscale = 1;
	PainterWidget* pw;
	void drawNx();
	void drawSx();
	void drawUx();

protected:
	void changeEvent(QEvent *e);
	void resizeEvent(QResizeEvent *event);

private slots:
	void on_nxysplus_clicked();

	void on_nxysmiuns_clicked();

	void on_nxxsminus_clicked();

	void on_nxxsplus_clicked();
\


	void on_pushButton_clicked();

	void on_pushButton_2_clicked();

	void on_pushButton_3_clicked();

	void on_pushButton_4_clicked();

	void on_uyminusbtn_clicked();

	void on_uyplusbtn_clicked();

	void on_uxminusbtn_clicked();

	void on_uxplusbtn_clicked();

	void on_nvalue_valueChanged(int arg1);

	void on_formreport_clicked();

private:
	QPoint percentPoint(float x, float y);
	Processor* processor;
	QGraphicsScene *nxscene;
	QGraphicsScene *sxscene;
	QGraphicsScene *uxscene;
	Ui::PostProcessor *ui;
};

#endif // POSTPROCESSOR_H
