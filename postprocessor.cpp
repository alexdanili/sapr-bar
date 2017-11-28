#include "postprocessor.h"
#include "ui_postprocessor.h"

PostProcessor::PostProcessor(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::PostProcessor)
{
	ui->setupUi(this);
	nxscene = new QGraphicsScene(this);
	sxscene = new QGraphicsScene(this);
	uxscene = new QGraphicsScene(this);
	ui->nxview->setScene(nxscene);
	ui->sxview->setScene(sxscene);
	ui->uxview->setScene(uxscene);
	ui->nvalue->setMaximum(50);
}
QPoint PostProcessor::percentPoint(float x, float y)
{
	x *= 100;
	y *= 100;
	int xp = x*(size().width()/100);
	int yp = y*(size().height()/100);
	return QPoint(xp,yp);
}
PostProcessor::~PostProcessor()
{
	delete ui;
}

void PostProcessor::setProcessor(Processor* _processor)
{
    this->processor = _processor;
	if(processor->n > 0)
		ui->nvalue->setValue(processor->n);
	nxscene->clear();
	for(int i = 0; i < percentPoint(1,1).x()/50; i++)
	{
		nxscene->addLine(i*60,percentPoint(0,0.5).y(),50 + i*60,percentPoint(0,0.5).y(), QPen(Qt::red));
	}



	ui->tableWidget->setRowCount(processor->nMat.size()+processor->nMat.size()/(processor->n+1));
	for(int j = 0; j < processor->nMat.size()/(processor->n+1); j++)
	{
		QTableWidgetItem* item1 = new QTableWidgetItem;
		QTableWidgetItem* item2 = new QTableWidgetItem;
		QTableWidgetItem* item3 = new QTableWidgetItem;
		item1->setText("Nx"+QString::number(j+1));
		item2->setText("Sx"+QString::number(j+1));
		item3->setText("Ux"+QString::number(j+1));
		item1->setBackgroundColor(Qt::gray);
		item2->setBackgroundColor(Qt::gray);
		item3->setBackgroundColor(Qt::gray);
		ui->tableWidget->setItem(j*(processor->n+1)+j,0,item1);
		ui->tableWidget->setItem(j*(processor->n+1)+j,1,item2);
		ui->tableWidget->setItem(j*(processor->n+1)+j,2,item3);
		for(int i = 0; i < processor->n+1; i++)
		{
			QTableWidgetItem* item1 = new QTableWidgetItem;
			QTableWidgetItem* item2 = new QTableWidgetItem;
			QTableWidgetItem* item3 = new QTableWidgetItem;
			item1->setText(QString::number(processor->nMat[j*(processor->n+1)+i]));
			item2->setText(QString::number(processor->sMat[j*(processor->n+1)+i]));
			if(fabs(processor->sMat[j*(processor->n+1)+i]) > processor->bars[j][2])
			{
				item2->setBackgroundColor(Qt::red);
			}

			item3->setText(QString::number(processor->udopMat[j][i]));
			ui->tableWidget->setItem(j*(processor->n+1)+j+i+1,0,item1);
			ui->tableWidget->setItem(j*(processor->n+1)+j+i+1,1,item2);
			ui->tableWidget->setItem(j*(processor->n+1)+j+i+1,2,item3);
		}
	}

	drawNx();
	drawSx();
	drawUx();



}

static bool abs_compare(float a, float b)
{
	return (std::fabs(a) < std::fabs(b));
}

void PostProcessor::drawNx()
{
	nxscale = (size().width())/(processor->nodes[processor->bars[processor->bars.size()-1][1]]-processor->nodes[processor->bars[0][0]]);
	nyscale = (size().height()/2)/(fabs(*std::max_element(processor->nMat.begin(),processor->nMat.end(),abs_compare)));

	nxscene = new QGraphicsScene(this);
	ui->nxview->setScene(nxscene);
	float sm = 0;
	for(int i = 0; i < processor->bars.size(); i++)
	{
		float len = processor->nodes[processor->bars[i][1]]-processor->nodes[processor->bars[i][0]];
		sm = processor->nodes[processor->bars[i][0]];
		QVector<QPointF> points;
		for(int j = 0; j< processor->n+1; j++)
		{
			points.push_back(QPointF(sm+j*len/(processor->n) ,(processor->nMat[i*(processor->n+1)+j])));
		}
		points.push_back(QPointF(points[points.size()-1].x(),0.0f));
		points.push_back(QPointF(points[0].x(),0.0f));
		for(int k = 0; k < points.size(); k++)
		{
			points[k].setX(points[k].x()*nxscale);
			points[k].setY(percentPoint(0.5,0.5).y() - points[k].y()*nyscale);
		}
		nxscene->addPolygon(QPolygonF(points),QPen(Qt::black),QBrush(Qt::green));
		QGraphicsTextItem *text = nxscene->addText(QString::number(processor->nMat[i*(processor->n+1)+0]));
		text->setPos(points[0]);
		QGraphicsTextItem *text1 = nxscene->addText(QString::number(processor->nMat[i*(processor->n+1)+processor->n]));
		text1->setPos(points[points.size()-3]);
	}
}

void PostProcessor::drawSx()
{

	sxscale = (size().width())/(processor->nodes[processor->bars[processor->bars.size()-1][1]]-processor->nodes[processor->bars[0][0]]);
	syscale = (size().height()/2)/(fabs(*std::max_element(processor->sMat.begin(),processor->sMat.end(),abs_compare)));

	sxscene = new QGraphicsScene(this);
	ui->sxview->setScene(sxscene);
	float sm = 0;
	for(int i = 0; i < processor->bars.size(); i++)
	{
		float len = processor->nodes[processor->bars[i][1]]-processor->nodes[processor->bars[i][0]];
		sm = processor->nodes[processor->bars[i][0]];
		QVector<QPointF> points;
		for(int j = 0; j< processor->n+1; j++)
		{
			points.push_back(QPointF(sm+j*len/(processor->n) ,(processor->sMat[i*(processor->n+1)+j])));
		}
		points.push_back(QPointF(points[points.size()-1].x(),0.0f));
		points.push_back(QPointF(points[0].x(),0.0f));
		for(int k = 0; k < points.size(); k++)
		{
			points[k].setX(points[k].x()*sxscale);
			points[k].setY(percentPoint(0.5,0.5).y() - points[k].y()*syscale);
		}
		sxscene->addPolygon(QPolygonF(points),QPen(Qt::black),QBrush(Qt::green));
		QGraphicsTextItem *text = sxscene->addText(QString::number(processor->sMat[i*(processor->n+1)+0]));
		text->setPos(points[0]);
		QGraphicsTextItem *text1 = sxscene->addText(QString::number(processor->sMat[i*(processor->n+1)+processor->n]));
		text1->setPos(points[points.size()-3]);
	}
}

void PostProcessor::drawUx()
{
	uxscale = (size().width())/(processor->nodes[processor->bars[processor->bars.size()-1][1]]-processor->nodes[processor->bars[0][0]]);
	float maximum = 0;
	for(int x=0; x<processor->udopMat.size(); ++x)
	  for(int y=0; y<processor->udopMat[x].size(); ++y)
		maximum = std::max(processor->udopMat[x][y], maximum,abs_compare);
	uyscale = (size().height()/2)/maximum;

	uxscene = new QGraphicsScene(this);
	ui->uxview->setScene(uxscene);
	float sm = 0;
	for(int i = 0; i < processor->bars.size(); i++)
	{
		float len = processor->nodes[processor->bars[i][1]]-processor->nodes[processor->bars[i][0]];
		sm = processor->nodes[processor->bars[i][0]];
		QVector<QPointF> points;
		for(int j = 0; j< processor->n+1; j++)
		{
			points.push_back(QPointF(sm+j*len/(processor->n) ,(processor->udopMat[i][j])));
		}
		points.push_back(QPointF(points[points.size()-1].x(),0.0f));
		points.push_back(QPointF(points[0].x(),0.0f));
		for(int k = 0; k < points.size(); k++)
		{
			points[k].setX(points[k].x()*uxscale);
			points[k].setY(percentPoint(0.5,0.5).y() - points[k].y()*uyscale);
		}
		uxscene->addPolygon(QPolygonF(points),QPen(Qt::black),QBrush(Qt::green));
		QGraphicsTextItem *text = uxscene->addText(QString::number(processor->udopMat[i][0]));
		text->setPos(points[0]);
		QGraphicsTextItem *text1 = uxscene->addText(QString::number(processor->udopMat[i][processor->n]));
		text1->setPos(points[points.size()-3]);
	}
}

void PostProcessor::changeEvent(QEvent *e)
{
	QWidget::changeEvent(e);
	switch (e->type()) {
		case QEvent::LanguageChange:
			ui->retranslateUi(this);
			break;
		default:
			break;
	}
}

void PostProcessor::resizeEvent(QResizeEvent* event)
{
	drawNx();
	drawSx();
	drawUx();

}

void PostProcessor::on_nxysplus_clicked()
{
	nyscale += 1;
	drawNx();
}

void PostProcessor::on_nxysmiuns_clicked()
{
	if(nyscale > 0)
	nyscale -= 1;
	drawNx();
}

void PostProcessor::on_nxxsminus_clicked()
{
	if(nxscale > 0)
	nxscale -= 1;
	drawNx();
}

void PostProcessor::on_nxxsplus_clicked()
{
	nxscale += 1;
	drawNx();
}


void PostProcessor::on_pushButton_clicked()
{
	if(syscale > 0)
		syscale -= 1;
	drawSx();
}

void PostProcessor::on_pushButton_2_clicked()
{

	syscale += 1;
	drawSx();
}

void PostProcessor::on_pushButton_3_clicked()
{
	if(sxscale > 0)
		sxscale -= 1;
	drawSx();
}

void PostProcessor::on_pushButton_4_clicked()
{

	sxscale += 1;
	drawSx();
}

void PostProcessor::on_uyminusbtn_clicked()
{
	if(uyscale > 0)
		uyscale -= 1;
	drawUx();
}

void PostProcessor::on_uyplusbtn_clicked()
{
	uyscale += 1;
	drawUx();
}

void PostProcessor::on_uxminusbtn_clicked()
{
	if(uxscale > 0)
		uxscale -= 1;
	drawUx();
}

void PostProcessor::on_uxplusbtn_clicked()
{
	uxscale += 1;
	drawUx();
}

void PostProcessor::on_nvalue_valueChanged(int arg1)
{
	if(processor->n > 0)
	{
	processor->n = arg1;
	processor->calculate();
	setProcessor(processor);
	}
}

void PostProcessor::on_formreport_clicked()
{

	QString fileName = QFileDialog::getSaveFileName(this,tr("Save report"),"",tr("HTML(*.html)"));
	QFileInfo info(fileName);
	QImage image(nxscene->sceneRect().size().toSize(), QImage::Format_ARGB32);  // Create the image with the exact size of the shrunk scene
	image.fill(Qt::white);
	QPainter painter(&image);
	painter.setRenderHint(QPainter::Antialiasing);
	nxscene->render(&painter);
	image.save(info.path()+"\\nx.jpg");


	QImage image1(sxscene->sceneRect().size().toSize(), QImage::Format_ARGB32);  // Create the image with the exact size of the shrunk scene
	image1.fill(Qt::white);
	QPainter painter1(&image1);
	painter1.setRenderHint(QPainter::Antialiasing);
	sxscene->render(&painter1);
	image1.save(info.path()+"\\sx.jpg");

	QImage image2(uxscene->sceneRect().size().toSize(), QImage::Format_ARGB32);  // Create the image with the exact size of the shrunk scene
	image2.fill(Qt::white);
	QPainter painter2(&image2);
	painter2.setRenderHint(QPainter::Antialiasing);
	uxscene->render(&painter2);
	image2.save(info.path()+"\\ux.jpg");
	pw->image->save(info.path()+"\\gr.jpg");

	QFile file(fileName);
	file.open(QIODevice::WriteOnly | QIODevice::Text);
	QTextStream out(&file);
	out.setCodec("Windows-1251");
	out<<tr("<html>\
		  <head>\
		   <title>Отчет</title>\
		  </head>\
		  <body>");
	out<<tr("Конструкция:<br>");
	out<<"<img src=\"gr.jpg\"><br>";
	out<<tr("Эпюр Nx:<br>");
	out<<"<img src=\"nx.jpg\"><br>";
	out<<tr("Эпюр Sx:<br>");
	out<<"<img src=\"sx.jpg\"><br>";
	out<<tr("Эпюр Ux:<br>");
	out<<"<img src=\"ux.jpg\"><br>";
	out<<"<table border=\"1\" width=\"100%\" cellpadding=\"5\">";
	for(int i = 0; i < ui->tableWidget->rowCount(); i++)
	{
		out<<"<tr>";
		for(int j = 0; j < 3; j++)
		{
			if(ui->tableWidget->item(i,j)->backgroundColor() == Qt::gray)
			{
				out<<"<td bgcolor=\"gray\">"<<ui->tableWidget->item(i,j)->text()<<"</td>";
			}
			else if(ui->tableWidget->item(i,j)->backgroundColor() == Qt::red)
			{
				out<<"<td bgcolor=\"red\">"<<ui->tableWidget->item(i,j)->text()<<"</td>";
			}
			else
			{
				out<<"<td>"<<ui->tableWidget->item(i,j)->text()<<"</td>";
			}
		}
		out<<"</tr>";
	}
	out<<"</table>\
		 </body>\
		</html>";
			file.close();
}
