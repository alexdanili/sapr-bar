#include "painterwidget.h"
#include "ui_painterwidget.h"

PainterWidget::PainterWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::PainterWidget)
{
	ui->setupUi(this);
}

PainterWidget::~PainterWidget()
{
	delete ui;
}

QRect PainterWidget::percentRect(float x1 , float y1, float x2, float y2)
{
	x1 *= 100;
	x2 *= 100;
	y1 *= 100;
	y2 *= 100;
	int x1p = x1 * (size().width()/100);
	int y1p = y1 * (size().height()/100);
	int x2p = x2 * (size().width()/100);
	int y2p = y2 * (size().height()/100);
	return QRect(x1p,y1p,x2p,y2p);
}

QPoint PainterWidget::percentPoint(float x, float y)
{
	x *= 100;
	y *= 100;
	int xp = x*(size().width()/100);
	int yp = y*(size().height()/100);
	return QPoint(xp,yp);
}
void PainterWidget::setNodes(std::vector<double> _nodes)
{
	this->nodes = _nodes;
	this->nodesc = _nodes;

}
void PainterWidget::setBars(std::vector<std::vector<double> > _bars)
{
	this->bars = _bars;
	this->barsc = _bars;
	draw();
}

void PainterWidget::setForces(std::vector<std::vector<float> > _forces)
{

	this->forces = _forces;
	draw();
}

void PainterWidget::setFixators(bool _leftFixator, bool _rightFixator)
{
	this->leftFixator = _leftFixator;
	this->rightFixator = _rightFixator;
	draw();
}

void PainterWidget::setScale(float _scale)
{
	this->scale = _scale;
	draw();
}

void PainterWidget::clearArea()
{
	leftFixator = false;
	rightFixator = false;
	bars.clear();
	forces.clear();
	draw();
}

void PainterWidget::drawLeftFixator(QPainter *painter)
{
	painter->drawLine(nodes[bars[0][0]]+20,yAxie - 30, nodes[bars[0][0]]+20, yAxie + 30);
	for(int i = 0; i < 7; i++)
	{
		painter->drawLine(nodes[bars[0][0]]+20,yAxie - 30 + 10*i,nodes[bars[0][0]]+10,yAxie - 20 + 10*i);
	}
	lastPos += nodes[bars[0][0]]+10;
}

void PainterWidget::drawRightFixator(QPainter *painter)
{
	painter->drawLine(nodes[bars[bars.size()-1][0]] + bars[bars.size()-1][1] + 2 + leftFixator*10+10+(bars.size()-1),yAxie - 20, nodes[bars[bars.size()-1][0]] + bars[bars.size()-1][1] + 2 + leftFixator*10+10+(bars.size()-1)  , yAxie + 40);
	for(int i = 0; i < 7; i++)
	{
		painter->drawLine(nodes[bars[bars.size()-1][0]] + bars[bars.size()-1][1]+10+2+ leftFixator*10+10+ (bars.size()-1),yAxie - 30 + 10*i ,nodes[bars[bars.size()-1][0]] + bars[bars.size()-1][1] + 2+ leftFixator*10+10+(bars.size()-1),yAxie - 20 + 10*i);
	}
}

void PainterWidget::drawBars(QPainter* painter)
{

	float diff = yAxie*2;
	//sort width
	std::vector<int> barI;
	std::vector<double> barSort;
	barI.clear();
	barSort.clear();
	for(int i = 0; i < bars.size(); i++)
	{
		barSort.push_back(barsc[i][2]);
		barI.push_back(i);
	}
	for (int i = barSort.size() - 1; i >= 0; i--)
	{
		for (int j = 0; j < i; j++)
		{
			if (barSort[j] > barSort[j + 1])
			{
				double tmp = barSort[j];
				int tmp2 = barI[j];
				barSort[j] = barSort[j + 1];
				barI[j] = barI[j + 1];
				barSort[j+1] = tmp;
				barI[j + 1] = tmp2;
			}
		}
	}
	diff = 30.0f/barSort[0];
	std::vector<double> tmp;
	tmp.clear();
	tmp.resize(barSort.size());

	for(int i =0; i < barSort.size(); i++)
	{
		int c = 0;
		for(int j = 0; j < barSort.size()-1; j++)
		{
			if(barSort[i] == barSort[j] && j != i)
			{
				c++;
				tmp[j] = barSort[i];
			}
		}
		if(c != 0)
		{
			tmp[i] = barSort[i];
		}
	}


	std::vector<bool> flags;
	flags.clear();
	flags.resize(barSort.size());
	for(int i = 0; i < barSort.size(); i++)
	{
		double old = barSort[i];

		if(i == 0)
		{
			barSort[0] = 30;
			flags[0] = true;
		}
		else
		{

			if(!flags[i])
				barSort[i] = barSort[i]*diff;
		}
		for(int j = 0; j < barSort.size(); j++)
		{
			if(barSort[j] == old)
			{
				if(!flags[j])
				{
					barSort[j] = barSort[i];
					flags[j] = true;
				}
			}

		}
	}

	for(int i = 0; i < barI.size(); i++)
	{
		bars[barI[i]][2] = barSort[i]*scale;
	}





	//sort lenth

	std::vector<double> barOld;
	barSort.clear();
	barI.clear();
	for(int i = 0; i < bars.size(); i++)
	{
		barSort.push_back(barsc[i][1]);
		barOld.push_back(barsc[i][1]);
		barI.push_back(i);
	}
	for (int i = barSort.size() - 1; i >= 0; i--)
	{
		for (int j = 0; j < i; j++)
		{
			if (barSort[j] > barSort[j + 1])
			{
				double tmp = barSort[j];
				int tmp2 = barI[j];
				barSort[j] = barSort[j + 1];
				barI[j] = barI[j + 1];
				barSort[j+1] = tmp;
				barI[j + 1] = tmp2;
			}
		}
	}

	diff = 50.0f/barSort[0];
	tmp.clear();
	tmp.resize(barSort.size());

	for(int i =0; i < barSort.size(); i++)
	{
		int c = 0;
		for(int j = 0; j < barSort.size()-1; j++)
		{
			if(barSort[i] == barSort[j] && j != i)
			{
				c++;
				tmp[j] = barSort[i];
			}
		}
		if(c != 0)
		{
			tmp[i] = barSort[i];
		}
	}



	flags.clear();
	flags.resize(barSort.size());
	for(int i = 0; i < barSort.size(); i++)
	{
		double old = barSort[i];

		if(i == 0)
		{
			barSort[0] = 50;
			flags[0] = true;
		}
		else
		{

			if(!flags[i])
				barSort[i] = barSort[i]*diff;
		}
		for(int j = 0; j < barSort.size(); j++)
		{
			if(barSort[j] == old)
			{
				if(!flags[j])
				{
					barSort[j] = barSort[i];
					flags[j] = true;
				}
			}

		}
	}

	for(int i = 0; i < barI.size(); i++)
	{
		bars[barI[i]][1] = barSort[i]*scale;
	}



	double olddl = 0;
	for(int i = 0; i < bars.size(); i++)
	{
		double dl = barOld[i]-bars[i][1];
		olddl += dl;
		nodes[bars[i][3]] = nodesc[bars[i][3]] - olddl;
	}

	for(int i = 0; i < bars.size(); i++)
	{

		painter->drawRect(nodes[bars[i][0]]+i*1 + leftFixator*11+10,percentPoint(0,0.5).y()-bars[i][2]/2,bars[i][1],bars[i][2]);

		painter->drawRect(nodes[bars[i][0]] + 1 + bars[i][1]/2 - 10 + leftFixator*11+10,percentPoint(0,0.5).y()+bars[i][2]/2+10,20,20);
		painter->drawText(nodes[bars[i][0]] + 1 + bars[i][1]/2 - 3 + leftFixator*11+10,percentPoint(0,0.5).y()+bars[i][2]/2+25,QString::number(i+1));

		painter->drawEllipse(nodes[bars[i][0]] + 1 + leftFixator*11+10,percentPoint(0,0.5).y()-bars[i][2]/2-40,20,20);
		painter->drawText(nodes[bars[i][0]] + 8 + leftFixator*11+10,percentPoint(0,0.5).y()-bars[i][2]/2-25,QString::number(i+1));
	}

	painter->drawEllipse(nodes[bars[bars.size()-1][0]] + bars[bars.size()-1][1] + 1 + leftFixator*11+10,percentPoint(0,0.5).y()-bars[bars.size()-1][2]/2-40,20,20);
	painter->drawText(nodes[bars[bars.size()-1][0]] + bars[bars.size()-1][1] + 8 + leftFixator*11+10,percentPoint(0,0.5).y()-bars[bars.size()-1][2]/2-25,QString::number(bars.size()+1));
}

void PainterWidget::drawForces(QPainter* painter)
{
	for(int i = 0; i < forces.size(); i++)
	{
		if(forces[i][0] == 1)
		{
			if(forces[i][4] > 0)
			{
				painter->setPen(Qt::blue);
				int pl = 1+forces[i][3]+leftFixator*11 + 10;

				painter->drawLine(pl+nodes[forces[i][1]]+30-1,percentPoint(0,0.5).y()-3,pl+nodes[forces[i][1]]+30-8,percentPoint(0,0.5).y()-10);
				painter->drawLine(pl+nodes[forces[i][1]]+30-1,percentPoint(0,0.5).y()-2,pl+nodes[forces[i][1]]+30-9,percentPoint(0,0.5).y()-10);
				painter->drawLine(pl+nodes[forces[i][1]]+30-1-1,percentPoint(0,0.5).y()-2,pl+nodes[forces[i][1]]+30-9,percentPoint(0,0.5).y()-9);

				painter->drawLine(pl+nodes[forces[i][1]]-1,percentPoint(0,0.5).y()-1,pl+nodes[forces[i][1]]+30,percentPoint(0,0.5).y()-1);
				painter->drawLine(pl+nodes[forces[i][1]]-1,percentPoint(0,0.5).y(),pl+nodes[forces[i][1]]+30,percentPoint(0,0.5).y());
				painter->drawLine(pl+nodes[forces[i][1]]-1,percentPoint(0,0.5).y()+1,pl+nodes[forces[i][1]]+30,percentPoint(0,0.5).y()+1);

				painter->drawLine(pl+nodes[forces[i][1]]+30-1,percentPoint(0,0.5).y()+3,pl+nodes[forces[i][1]]+30-8,percentPoint(0,0.5).y()+10);
				painter->drawLine(pl+nodes[forces[i][1]]+30-1,percentPoint(0,0.5).y()+2,pl+nodes[forces[i][1]]+30-9,percentPoint(0,0.5).y()+10);
				painter->drawLine(pl+nodes[forces[i][1]]+30-1-1,percentPoint(0,0.5).y()+2,pl+nodes[forces[i][1]]+30-9,percentPoint(0,0.5).y()+9);
			}
			else
			{

				painter->setPen(Qt::blue);
				int pl = 1+forces[i][3] +leftFixator*11 +10;
				painter->drawLine(pl+nodes[forces[i][1]]-30+1,percentPoint(0,0.5).y()-3,pl+nodes[forces[i][1]]-30+8,percentPoint(0,0.5).y()-10);
				painter->drawLine(pl+nodes[forces[i][1]]-30+1,percentPoint(0,0.5).y()-2,pl+nodes[forces[i][1]]-30+9,percentPoint(0,0.5).y()-10);
				painter->drawLine(pl+nodes[forces[i][1]]-30+1-1,percentPoint(0,0.5).y()-2,pl+nodes[forces[i][1]]-30+9,percentPoint(0,0.5).y()-9);

				painter->drawLine(pl+nodes[forces[i][1]]-1,percentPoint(0,0.5).y()-1,pl+nodes[forces[i][1]]-30,percentPoint(0,0.5).y()-1);
				painter->drawLine(pl+nodes[forces[i][1]]-1,percentPoint(0,0.5).y(),pl+nodes[forces[i][1]]-30,percentPoint(0,0.5).y());
				painter->drawLine(pl+nodes[forces[i][1]]-1,percentPoint(0,0.5).y()+1,pl+nodes[forces[i][1]]-30,percentPoint(0,0.5).y()+1);

				painter->drawLine(pl+nodes[forces[i][1]]-30+1,percentPoint(0,0.5).y()+3,pl+nodes[forces[i][1]]-30+8,percentPoint(0,0.5).y()+10);
				painter->drawLine(pl+nodes[forces[i][1]]-30+1,percentPoint(0,0.5).y()+2,pl+nodes[forces[i][1]]-30+9,percentPoint(0,0.5).y()+10);
				painter->drawLine(pl+nodes[forces[i][1]]-30-1+1,percentPoint(0,0.5).y()+2,pl+nodes[forces[i][1]]-30+9,percentPoint(0,0.5).y()+9);
			}
		}

		if(forces[i][0] == 2)
		{


			painter->setPen(QColor(255,137,0));
			int pl = 1+forces[i][3] + leftFixator*11 +10;
			painter->drawLine(pl+nodes[forces[i][1]]-1,percentPoint(0,0.5).y(),pl+nodes[forces[i][2]]-3,percentPoint(0,0.5).y());
			if(forces[i][4] > 0)
			{
				for(int j = 1 ; j < (nodes[forces[i][2]] - nodes[forces[i][1]])/10; j++ )
				{
					painter->drawLine(pl+nodes[forces[i][1]]+10*j,percentPoint(0,0.5).y()-1,pl+nodes[forces[i][1]]+10*j-3,percentPoint(0,0.5).y()-3);
					painter->drawLine(pl+nodes[forces[i][1]]+10*j,percentPoint(0,0.5).y()+1,pl+nodes[forces[i][1]]+10*j-3,percentPoint(0,0.5).y()+3);
				}
			}
			else
			{
				for(int j = 1 ; j < (nodes[forces[i][2]] - nodes[forces[i][1]])/10; j++ )
				{
					painter->drawLine(pl+nodes[forces[i][1]]+10*j-3,percentPoint(0,0.5).y()-1,pl+nodes[forces[i][1]]+10*j,percentPoint(0,0.5).y()-3);
					painter->drawLine(pl+nodes[forces[i][1]]+10*j-3,percentPoint(0,0.5).y()+1,pl+nodes[forces[i][1]]+10*j,percentPoint(0,0.5).y()+3);
				}
			}

		}
	}

	painter->setPen(Qt::black);
}



void PainterWidget::draw()
{
	lastPos = 0;
	QSize s = size();
	int width = 0;
	int height = 0;
	for(int i = 0; i < bars.size(); i++)
	{
		width += 2 + bars[i][1];
	}
	for(int i = 0; i < bars.size(); i++)
	{
		if(bars[i][2]/2 > height)
		{
			height = bars[i][2]/2;
		}
	}
	width += leftFixator*11+10 + rightFixator*11;
	if(width > size().width())
	{

	}
	else
	{
		width = size().width();
	}
	if(bars.size() > 3)
	{
		int k = 0;
	}
	if(fabs(percentPoint(0,0.5).y() - height)*2 > size().height())
	{
		height = fabs(percentPoint(0,0.5).y() - height)*2;
	}
	else
	{
		height = size().height();
	}

	s = QSize(width,height);
	image = new QImage(scale*s, QImage::Format_RGB32);
	if(!image->isNull())
	{

		QPainter* painter = new QPainter(image);
		image->fill(Qt::white);
		painter->scale(scale,scale);
		/*Начало рисования*/
		painter->setPen(Qt::red);
		yAxie = percentPoint(0,0.5).y();
		for(int i = 0; i < percentPoint(1,1).x()/50; i++)
		{
			painter->drawLine(i*60,yAxie,50 + i*60,yAxie);
		}
		painter->setPen(Qt::black);

		if(leftFixator && !bars.empty())
		{
			if(nodes[0] != 0 && nodes.size() > 0)
			{
				for(int i = 1; i < nodes.size(); i++)
				{
					nodes[i] -= nodes[0];
				}
				nodes[0] = 0;
			}
			drawLeftFixator(painter);
		}
		if(!bars.empty())
			drawBars(painter);
		if(!forces.empty())
			drawForces(painter);
		if(rightFixator && !bars.empty())
		{
			drawRightFixator(painter);
		}
		painter->end();
		scene = new QGraphicsScene(this);
		scene->installEventFilter(this);
		scene->addPixmap(QPixmap::fromImage(*image));
		scene->setSceneRect(image->rect());
		ui->graphicsView->setScene(scene);
		ui->graphicsView->installEventFilter(this);
		update();
	}
	else
	{
		qDebug()<<"NULL";
	}
}



