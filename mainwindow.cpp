#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	processor = new Processor();
	postprocessor = new PostProcessor();
	postprocessor->pw = this->ui->PainterObj;
	image = ui->PainterObj->image;
	ui->BarsTable->setHorizontalHeaderItem(4, new QTableWidgetItem("Макс.Доп.Напр(\u03C3)"));

	ui->NodesTable->horizontalHeader()->sortIndicatorOrder();
	ui->NodesTable->setItemDelegate(new NodesDlegate);
	ui->BarsTable->setItemDelegate(new BarsDlegate);
	ui->ForcesTable->setItemDelegate(new ForcesDlegate);

	connect(ui->addNodeButton, &QPushButton::clicked, [this]{on_addButton_released("nodes");});
	connect(ui->addBarButton, &QPushButton::clicked, [this]{on_addButton_released("bars");});
	connect(ui->addForceButton, &QPushButton::clicked, [this]{on_addButton_released("forces");});


	connect(ui->deleteNodesButton, &QPushButton::clicked, [this]{on_deleteButton_released("nodes");});
	connect(ui->deleteBarsButton, &QPushButton::clicked, [this]{on_deleteButton_released("bars");});
	connect(ui->deleteForcesButton, &QPushButton::clicked, [this]{on_deleteButton_released("forces");});
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_addButton_released(std::string table)
{
	if(table == "nodes")
	{
		ui->NodesTable->setRowCount(ui->NodesTable->rowCount()+1);
	}
	else if(table == "bars")
	{
		ui->BarsTable->setRowCount(ui->BarsTable->rowCount()+1);
	}
	else if(table == "forces")
	{
		ui->ForcesTable->setRowCount(ui->ForcesTable->rowCount()+1);
		QComboBox* cb = new QComboBox(this);
		cb->addItem("Сосредоточенная");
		cb->addItem("Распределенная");
		cb->setCurrentIndex(0);
		connect(cb,SIGNAL(currentIndexChanged(int)),this,SLOT(on_ForcesTable_cellChanged()));
		ui->ForcesTable->setCellWidget(ui->ForcesTable->rowCount()-1,0,cb);
	}
}

void MainWindow::on_deleteButton_released(std::string table)
{
	if(table == "nodes")
	{
		if(ui->NodesTable->currentRow() == -1)
		{
			if(ui->NodesTable->rowCount() > 0)
			{
				ui->NodesTable->setRowCount(ui->NodesTable->rowCount()-1);
			}
			else
			{
				QMessageBox messageBox;
				messageBox.critical(0,"Ошибка удаления","Нечего удалять!");
				messageBox.setFixedSize(500,200);
			}

		}
		else
		{
			int pre = ui->NodesTable->currentRow();
			pre++;
			ui->NodesTable->removeRow(ui->NodesTable->currentRow());
			for(int i = 0; i < ui->ForcesTable->rowCount(); i++)
			{
				if(qobject_cast<QComboBox*>(ui->ForcesTable->cellWidget(i,0))->currentText() == "Сосредоточенная")
				{
					QString str = ui->ForcesTable->item(i,1)->text();
					if(str == QString::number(pre))
					{
						ui->ForcesTable->item(i,1)->setText("");
					}
				}
				else
				{
					QString str = ui->ForcesTable->item(i,1)->text();
					if(str != "")
					{
						QString tmp1 = ui->BarsTable->item(str.toInt()-1,0)->text();
						QString tmp2 = ui->BarsTable->item(str.toInt()-1,1)->text();
						if( tmp1.toInt() == pre || tmp2.toInt() == pre || tmp1.toInt() >= ui->NodesTable->rowCount() || tmp2.toInt() >= ui->NodesTable->rowCount())
						{
							ui->ForcesTable->item(i,1)->setText("");
						}
					}
				}
			}

		}
	}
	else if(table == "bars")
	{
		if(ui->BarsTable->currentRow() == -1)
		{
			if(ui->BarsTable->rowCount() > 0)
			{
				ui->BarsTable->setRowCount(ui->BarsTable->rowCount()-1);
			}
			else
			{
				QMessageBox messageBox;
				messageBox.critical(0,"Ошибка удаления","Нечего удалять!");
				messageBox.setFixedSize(500,200);
			}
		}
		else
		{
			ui->BarsTable->removeRow(ui->BarsTable->currentRow());
		}
	}
	else if(table == "forces")
	{
		if(ui->ForcesTable->currentRow() == -1)
		{
			if(ui->ForcesTable->rowCount() > 0)
			{
				ui->ForcesTable->setRowCount(ui->ForcesTable->rowCount()-1);
			}
			else
			{
				QMessageBox messageBox;
				messageBox.critical(0,"Ошибка удаления","Нечего удалять!");
				messageBox.setFixedSize(500,200);
			}
		}
		else
		{
			ui->ForcesTable->removeRow(ui->ForcesTable->currentRow());
		}
	}
	on_BarsTable_cellChanged(0,0);
	on_ForcesTable_cellChanged();
}

void MainWindow::on_SaveImageButton_released()
{
	QString fileName = QFileDialog::getSaveFileName(this,
													tr("Save image"));
	ui->PainterObj->image->save(fileName+".jpg");
}



void MainWindow::on_ScaleValue_valueChanged(int value)
{
	ui->PainterObj->setScale(value);
	ui->PainterObj->draw();
}



void MainWindow::on_leftFixator_clicked(bool checked)
{
	leftFixator = checked;
	ui->PainterObj->setFixators(leftFixator,rightFixator);
}

void MainWindow::on_RightFixator_clicked(bool checked)
{
	rightFixator = checked;
	ui->PainterObj->setFixators(leftFixator,rightFixator);
}

void MainWindow::on_NodesTable_cellChanged()
{
	bool same = false;
	double max = 0;
	int row = 0;
	for(; row < ui->NodesTable->rowCount(); row++)
	{
		if(!sorting)
		{
		if(ui->NodesTable->item(row,0) != NULL)
		{
			auto tmp = ui->NodesTable->item(row,0);
			if(tmp->text() != "")
			{
				for(int i = 0; i < ui->NodesTable->rowCount(); i++)
				{
					if(ui->NodesTable->item(i,0) != NULL)
					{
						if(ui->NodesTable->item(i,0)->text() != "")
						{
							if(i != row && ui->NodesTable->item(i,0)->text() == ui->NodesTable->item(row,0)->text())
							{
								same = true;
								break;
							}
						}
					}
				}
			}

		}
		for (int i = ui->NodesTable->rowCount() - 1; i >= 0; i--)
		{
			for (int j = 0; j < i; j++)
			{
				if(ui->NodesTable->item(i,0) != NULL && ui->NodesTable->item(j,0) != NULL)
				{
					if(ui->NodesTable->item(i,0)->text() != "" && ui->NodesTable->item(j,0)->text() != "")
					{
						if (ui->NodesTable->item(j,0)->text().toDouble() > ui->NodesTable->item(j+1,0)->text().toDouble())
						{
							sorting = true;
							QTableWidgetItem* tmp1 = new QTableWidgetItem;
							tmp1->setText(ui->NodesTable->item(j,0)->text());
							QTableWidgetItem* tmp2 = new QTableWidgetItem;
							tmp2->setText(ui->NodesTable->item(j+1,0)->text());
							ui->NodesTable->setItem(j,0,tmp2);
							ui->NodesTable->setItem(j+1,0,tmp1);
						}
					}
				}
			}
		}
		sorting = false;

		if(same)
		{
			QMessageBox messageBox;
			messageBox.critical(0,"Ошибка записи","Данный узел уже существует!");
			messageBox.setFixedSize(500,200);
			ui->NodesTable->item(ui->NodesTable->currentRow(),0)->setText("");
			break;
		}
		std::vector<double> _nodes;
		_nodes.clear();
		for(int i = 0; i < ui->NodesTable->rowCount(); i++)
		{
			if(ui->NodesTable->item(i,0) != NULL)
			{
				if(ui->NodesTable->item(i,0)->text() != "")
				{
					_nodes.push_back(ui->NodesTable->item(i,0)->text().toDouble());
				}
				else
				{
					_nodes.push_back(0);
				}
			}
			else
			{
				_nodes.push_back(0);
			}
		}

		ui->PainterObj->setNodes(_nodes);
		on_BarsTable_cellChanged(0,0);
		on_ForcesTable_cellChanged();
		}
	}

}

void MainWindow::on_BarsTable_cellChanged(int row, int column)
{
	QRegExp num("([0-9]+)");
	QRegExp powr("([0-9]{0,9}[\\.]?[0-9]{1,3})[\\n\\t\\v\\s]*\\*[\\n\\t\\v\\s]*([0-9]{0,9}[\\.]?[0-9]{1,3})[\\n\\t\\v\\s]*\\^[\\n\\t\\v\\s]*(-?[0-9]{1,3})|([0-9]{0,9}[\\.]?[0-9]{1,3})");
	if(!error)
	{

		if(ui->BarsTable->item(row,column) != NULL)
		{
			if(ui->BarsTable->item(row,column)->text() != "")
			{
				QString item = ui->BarsTable->item(row,column)->text();

				if(column <= 1)
				{
					int ncount = ui->NodesTable->rowCount();

					if(ui->BarsTable->item(row,0) != NULL && ui->BarsTable->item(row,1) != NULL)
					{
						if(ui->BarsTable->item(row,0)->text() != "" && ui->BarsTable->item(row,1)->text() != "")
						{
							if(ui->BarsTable->item(row,0)->text() == ui->BarsTable->item(row,1)->text())
							{
								QMessageBox messageBox;
								messageBox.critical(0,"Ошибка записи","Стержень с началом и концом в одном узле НЕ МОЖЕТ существовать!");
								messageBox.setFixedSize(500,200);
								error = true;
								ui->BarsTable->item(row,column)->setText("");
							}
							else if(ui->BarsTable->item(row,0)->text().toInt() > ui->BarsTable->item(row,1)->text().toInt())
							{
								QMessageBox messageBox;
								messageBox.critical(0,"Ошибка записи","Координата конца должна быть больше координаты начала!");
								messageBox.setFixedSize(500,200);
								error = true;
								ui->BarsTable->item(row,column)->setText("");
							}
							if(ui->NodesTable->item(ui->BarsTable->item(row,0)->text().toInt()-1,0) != NULL && ui->NodesTable->item(ui->BarsTable->item(row,1)->text().toInt()-1,0) != NULL )
							{
								if(ui->NodesTable->item(ui->BarsTable->item(row,0)->text().toInt()-1,0)->text() != "" && ui->NodesTable->item(ui->BarsTable->item(row,1)->text().toInt()-1,0)->text() != "" )
								{
									if(ui->NodesTable->item(ui->BarsTable->item(row,0)->text().toInt()-1,0)->text().toFloat() > ui->NodesTable->item(ui->BarsTable->item(row,1)->text().toInt()-1,0)->text().toFloat())
									{
										QMessageBox messageBox;
										messageBox.critical(0,"Ошибка записи","Координата конца должна быть больше координаты начала!");
										messageBox.setFixedSize(500,200);
										error = true;
										ui->BarsTable->item(row,1)->setText("");
									}
								}
							}

						}

					}
					for(int i = 0; i < ui->BarsTable->rowCount(); i++)
					{
						if(ui->BarsTable->item(i,0) != NULL && ui->BarsTable->item(i,1) != NULL)
						{
							if(ui->BarsTable->item(i,0)->text() != "" && ui->BarsTable->item(i,1)->text() != "")
							{
								if(ui->NodesTable->item(ui->BarsTable->item(i,1)->text().toInt()-2,0) != NULL && ui->NodesTable->item(ui->BarsTable->item(i,0)->text().toInt()-1,0) != NULL)
								{
									if(ui->NodesTable->item(ui->BarsTable->item(i,1)->text().toInt()-2,0)->text().toDouble() != ui->NodesTable->item(ui->BarsTable->item(i,0)->text().toInt()-1,0)->text().toDouble())
									{
										QMessageBox messageBox;
										messageBox.critical(0,"Ошибка записи","Внутри стержня есть узлы");
										messageBox.setFixedSize(500,200);
										error = true;
										ui->BarsTable->item(i,1)->setText("");
										break;
									}
								}

								if(ui->NodesTable->item(ui->BarsTable->item(i,0)->text().toInt()-1,0) != NULL && ui->NodesTable->item(ui->BarsTable->item(i,1)->text().toInt()-1,0) != NULL && ui->NodesTable->item(ui->BarsTable->item(row,column)->text().toInt()-1,0) != NULL)
								{
									double a = ui->NodesTable->item(ui->BarsTable->item(i,0)->text().toInt()-1,0)->text().toDouble();
									double b = ui->NodesTable->item(ui->BarsTable->item(i,1)->text().toInt()-1,0)->text().toDouble();
									double c = ui->NodesTable->item(ui->BarsTable->item(row,column)->text().toInt()-1,0)->text().toDouble();

									if(c<b && c>a)
									{
										QMessageBox messageBox;
										messageBox.critical(0,"Ошибка записи","Стержни пересекаются");
										messageBox.setFixedSize(500,200);
										error = true;
										ui->BarsTable->item(row,column)->setText("");
										break;
									}

								}
							}
						}



					}
					for(int i = 0; i < ui->BarsTable->rowCount(); i++)
					{
						if(column == 0)
						{
							if(i != row)
							{
								if(ui->BarsTable->item(i,0) != NULL)
								{
									auto tmp = ui->BarsTable->item(i,0)->text();
									if(tmp != "")
									{
										if(item == tmp)
										{
											QMessageBox messageBox;
											messageBox.critical(0,"Ошибка записи","Стержень с началом в этом узле уже существует!");
											messageBox.setFixedSize(500,200);
											error = true;
											ui->BarsTable->item(row,column)->setText("");
											break;
										}

									}
								}
							}
						}
						else if(column == 1)
						{
							if(i != row)
							{
								if(ui->BarsTable->item(i,1) != NULL)
								{
									auto tmp = ui->BarsTable->item(i,1)->text();
									if(tmp != "")
									{
										if(item == tmp)
										{
											QMessageBox messageBox;
											messageBox.critical(0,"Ошибка записи","Стержень с концом в этом узле уже существует!");
											messageBox.setFixedSize(500,200);
											error = true;
											ui->BarsTable->item(row,1)->setText("");
											break;
										}
									}
								}
							}
						}

					}
				}else if(column == 3)
				{
					if(ui->BarsTable->item(row,column) != NULL)
					{
						auto tmp = ui->BarsTable->item(row,column)->text();
						if(tmp != "")
						{
							if(tmp.toDouble() == 0)
							{
								QMessageBox messageBox;
								messageBox.critical(0,"Ошибка записи","Модуль упругости не может равняться нулую!");
								messageBox.setFixedSize(500,200);
								error = true;
								ui->BarsTable->item(row,column)->setText("");

							}
						}
					}
				}
				std::vector<std::vector<double>> bars;
				for(int i = 0; i < ui->BarsTable->rowCount(); i++)
				{
					if(checkBarsRow(i))
					{
						int ntrb = ui->BarsTable->item(i,0)->text().toInt()-1;
						int ntre = ui->BarsTable->item(i,1)->text().toInt()-1;
						float begin = ui->NodesTable->item(ntrb,0)->text().toFloat();
						float end = ui->NodesTable->item(ntre,0)->text().toFloat();
						float length = end - begin;
						QString dTmp = ui->BarsTable->item(i,2)->text();
						double d = 0;
						double a = 0;
						bool b1 = powr.exactMatch(dTmp);
						bool b2 = num.exactMatch(dTmp);
						if(b2)
						{
							a = dTmp.toDouble();
							d = pow((a*4)/3.14,0.5);
						}
						else if(b1)
						{
							a = powr.cap(1).toDouble()*pow(powr.cap(2).toDouble(),powr.cap(3).toDouble());
							d = pow((a*4)/3.14,0.5);
						}
						std::vector<double> bar{ntrb,length,d,ntre};

						bars.push_back(bar);
					}
				}
				if(!bars.empty())
				{
					std::vector<double> _nodes;
					_nodes.clear();
					for(int i = 0; i < ui->NodesTable->rowCount(); i++)
					{
						if(ui->NodesTable->item(i,0) != NULL)
						{
							if(ui->NodesTable->item(i,0)->text() != "")
							{
								_nodes.push_back(ui->NodesTable->item(i,0)->text().toDouble());
							}
							else
							{
								_nodes.push_back(0);
							}
						}
						else
						{
							_nodes.push_back(0);
						}
					}

					ui->PainterObj->setNodes(_nodes);
					ui->PainterObj->setBars(bars);
				}
				else
				{
					std::vector<double> np;
					std::vector<std::vector<double>> bp;
					ui->PainterObj->setNodes(np);
					ui->PainterObj->setBars(bp);
				}

			}
		}

	}
	else
	{
		error = false;
	}


}







void MainWindow::on_ForcesTable_cellChanged()
{

	std::vector<std::vector<float>> forces;
	for(int i = 0; i < ui->ForcesTable->rowCount(); i++)
	{

		if(checkForcesRow(i))
		{
			std::vector<float> force;
			float begin = 0;
			int type = 0;
			float end = 0;
			int bn = 0;
			if(qobject_cast<QComboBox*>(ui->ForcesTable->cellWidget(i,0))->currentText() == "Сосредоточенная")
			{
				int tp = ui->ForcesTable->item(i,1)->text().toInt();
				begin = tp - 1;
				type = 1;
			}
			else if(qobject_cast<QComboBox*>(ui->ForcesTable->cellWidget(i,0))->currentText() == "Распределенная")
			{
				begin = ui->BarsTable->item(ui->ForcesTable->item(i,1)->text().toInt() - 1,0)->text().toInt()-1;
				type = 2;
				end = ui->BarsTable->item(ui->ForcesTable->item(i,1)->text().toInt() - 1,1)->text().toInt()-1;
			}
			bn = ui->ForcesTable->item(i,1)->text().toInt()-1;
			force.push_back(type);
			force.push_back(begin);
			force.push_back(end);
			force.push_back(bn);
			force.push_back(ui->ForcesTable->item(i,2)->text().toDouble());
			forces.push_back(force);

		}
	}
	if(!forces.empty())
		ui->PainterObj->setForces(forces);

}

bool MainWindow::checkBarsRow(int row)
{
	bool result = true;
	for(int column = 0; column < ui->BarsTable->columnCount(); column++)
	{
		if(ui->BarsTable->item(row,column) == NULL)
		{
			result = false;
			break;
		}
		else if (ui->BarsTable->item(row,column)->text() == "")
		{
			result = false;
			break;
		}
		else if(column < 2 && ui->NodesTable->item(ui->BarsTable->item(row,column)->text().toInt()-1,0) == NULL)
		{
			result = false;
			break;
		}
		else if(column < 2 && ui->NodesTable->item(ui->BarsTable->item(row,column)->text().toInt()-1,0)->text() == "")
		{
			result = false;
			break;
		}

	}
	return result;
}

bool MainWindow::checkForcesRow(int row)
{
	bool result = true;
	for(int column = 1; column < ui->ForcesTable->columnCount(); column++)
	{
		if(ui->ForcesTable->item(row,column) == NULL)
		{
			result = false;
			break;
		}
		else if (ui->ForcesTable->item(row,column)->text() == "")
		{
			result = false;
			break;
		}
		if(qobject_cast<QComboBox*>(ui->ForcesTable->cellWidget(row,0)) != NULL)
		{
			if(qobject_cast<QComboBox*>(ui->ForcesTable->cellWidget(row,0))->currentText() == "Сосредоточенная" && column == 1)
			{
				int tp = ui->ForcesTable->item(row,column)->text().toInt();
				if(ui->NodesTable->item(tp-1,0) == NULL)
				{
					result = false;
					QMessageBox messageBox;
					messageBox.critical(0,"Ошибка записи","Узла №" + ui->ForcesTable->item(row,column)->text() + " не существует! От 1 до " + QString::number(ui->NodesTable->rowCount()));
					messageBox.setFixedSize(500,200);
					ui->ForcesTable->item(row,column)->setText("");
					break;
				}
				else if(ui->NodesTable->item(tp-1,0)->text() == "")
				{
					result = false;
					break;
				}
				bool flag = false;
				for(int i = 0; i < ui->BarsTable->rowCount(); i++)
				{
					if(ui->BarsTable->item(i,0) != NULL && ui->BarsTable->item(i,1) != NULL)
					{
						if(ui->BarsTable->item(i,0)->text() != "" || ui->BarsTable->item(i,1)->text() != "")
						{
							if(ui->BarsTable->item(i,0)->text().toInt() == tp || ui->BarsTable->item(i,1)->text().toInt() == tp)
							{
								flag = true;
							}
						}
					}
				}
				if(!flag)
				{
					result = false;
					QMessageBox messageBox;
					messageBox.critical(0,"Ошибка записи","Узел не испольщуется ни в каком стержне!");
					messageBox.setFixedSize(500,200);
					ui->ForcesTable->item(row,column)->setText("");
					break;
				}
			}
			else if(qobject_cast<QComboBox*>(ui->ForcesTable->cellWidget(row,0))->currentText() == "Распределенная" && column == 1)
			{
				if(!checkBarsRow(ui->ForcesTable->item(row,column)->text().toInt()-1))
				{
					result = false;
					break;
				}
			}
		}

	}

	return result;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
	QMainWindow::resizeEvent(event);
	ui->PainterObj->draw();
}

QWidget*NodesDlegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	QLineEdit* lineEdit = new QLineEdit(parent);
	QRegExpValidator* validator = new QRegExpValidator(QRegExp("\\d{0,6}[\\.]?\\d{1,3}"));
	lineEdit->setValidator(validator);
	return lineEdit;
}

QWidget *ForcesDlegate::createEditor(QWidget *parent, const QStyleOptionViewItem& option, const QModelIndex &index) const
{
	if(index.column() == 1 )
	{
		QLineEdit* lineEdit = new QLineEdit(parent);
		QIntValidator *validator = new QIntValidator(1, 9999999999, lineEdit);
		lineEdit->setValidator(validator);
		return lineEdit;
	}
	else if(index.column() == 2)
	{
		QLineEdit* lineEdit = new QLineEdit(parent);
		QRegExp r("(-?[0-9]{0,9}[\\.]?[0-9]{1,3})[\\n\\t\\v\\s]*\\*[\\n\\t\\v\\s]*([0-9]+)[\\n\\t\\v\\s]*\\^[\\n\\t\\v\\s]*(-?[0-9]+)|(-?[0-9]{0,9}[\.]?[0-9]{1,3})");
		QRegExpValidator *validator = new QRegExpValidator(r,lineEdit);
		lineEdit->setValidator(validator);
		return lineEdit;
	}
}



QWidget *BarsDlegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QLineEdit *lineEdit = new QLineEdit(parent);
	if(index.column() == 0 || index.column() == 1)
	{
		QIntValidator *validator = new QIntValidator(1, 9999999999, lineEdit);
		lineEdit->setValidator(validator);
	}
	else if(index.column() == 2)
	{
		QLineEdit* lineEdit = new QLineEdit(parent);
		QRegExp r("([0-9]{0,9}[\\.]?[0-9]{1,3})[\\n\\t\\v\\s]*\\*[\\n\\t\\v\\s]*([0-9]{0,9}[\\.]?[0-9]{1,3})[\\n\\t\\v\\s]*\\^[\\n\\t\\v\\s]*(-?[0-9]{1,3})|([0-9]{0,9}[\\.]?[0-9]{1,3})");
		QRegExpValidator *validator = new QRegExpValidator(r,lineEdit);
		lineEdit->setValidator(validator);
		return lineEdit;
	}
	else if(index.column() == 3 || index.column() == 4)
	{
		QLineEdit* lineEdit = new QLineEdit(parent);
		QRegExp r("(-?[0-9]{0,9}[\\.]?[0-9]{1,3})[\\n\\t\\v\\s]*\\*[\\n\\t\\v\\s]*([0-9]{0,9}[\\.]?[0-9]{1,3})[\\n\\t\\v\\s]*\\^[\\n\\t\\v\\s]*(-?[0-9]{1,3})|(-?[0-9]{0,9}[\\.]?[0-9]{1,3})");
		QRegExpValidator *validator = new QRegExpValidator(r,lineEdit);
		lineEdit->setValidator(validator);
		return lineEdit;
	}

	return lineEdit;
}

void MainWindow::on_submenuSave_triggered()
{
	QString fileName = QFileDialog::getSaveFileName(this,tr("Save project"),curPro,tr("SAPR PRO(*.sapr.pro)"));

	QFile file(fileName);
	file.open(QIODevice::WriteOnly | QIODevice::Text);
	QTextStream out(&file);
	out<<"[fix|";
	if(leftFixator)
	{
		out<<"lfix|";
	}
	if(rightFixator)
	{
		out<<"rfix|";
	}
	out<<"]"<<endl;
	out<<"[n|";
	for(int row = 0; row < ui->NodesTable->rowCount(); row++)
	{
		if(ui->NodesTable->item(row,0) != NULL)
		{
			auto tmp = ui->NodesTable->item(row,0);
			if(tmp->text() != "")
			{
				out<<tmp->text()<<"|";
			}
			else
			{
				out<<"."<<"|";
			}
		}
		else
		{
			out<<"."<<"|";
		}
	}
	out<<"]"<<endl;
	for(int row = 0; row < ui->BarsTable->rowCount(); row++)
	{
		out<<"[b|";
		for(int column = 0; column < ui->BarsTable->columnCount(); column++)
		{
			if(ui->BarsTable->item(row,column) != NULL)
			{
				if (ui->BarsTable->item(row,column)->text() != "")
				{
					out<<ui->BarsTable->item(row,column)->text()<<"|";
				}
				else
				{
					out<<"."<<"|";
				}
			}
			else
			{
				out<<"."<<"|";
			}
		}
		out<<"]"<<endl;
	}
	for(int row = 0; row < ui->ForcesTable->rowCount(); row++)
	{
		out<<"[f|";
		for(int column = 0; column < ui->ForcesTable->columnCount(); column++)
		{
			if(column == 0)
			{
				auto tmp = qobject_cast<QComboBox*>(ui->ForcesTable->cellWidget(row,column))->currentText();
				if (qobject_cast<QComboBox*>(ui->ForcesTable->cellWidget(row,column))->currentText() != "")
				{
					out<<qobject_cast<QComboBox*>(ui->ForcesTable->cellWidget(row,column))->currentText()<<"|";
				}
				else
				{
					out<<"."<<"|";
				}
			}
			else
			{
				if(ui->ForcesTable->item(row,column) != NULL)
				{
					if (ui->ForcesTable->item(row,column)->text() != "")
					{
						out<<ui->ForcesTable->item(row,column)->text()<<"|";
					}
					else
					{
						out<<"."<<"|";
					}
				}
				else
				{
					out<<"."<<"|";
				}
			}
		}
		out<<"]"<<endl;
	}

	file.close();
}


void MainWindow::on_submenuOpen_triggered()
{
	QString fileName = QFileDialog::getOpenFileName(this,tr("Open project"),"",tr("SAPR PRO(*.sapr.pro)"));
	if(fileName == "")
	{
		return;
	}
	curPro = fileName;
	QFile file(fileName);
	file.open(QIODevice::ReadOnly | QIODevice::Text);
	QTextStream project(&file);
	QString tmp;
	QRegExp reg("\\[((n|b|f|fix)\\|(.*))\\]\\s*");

	ui->NodesTable->setRowCount(0);
	ui->BarsTable->setRowCount(0);
	ui->ForcesTable->setRowCount(0);
	while(!project.atEnd())
	{
		tmp = project.readLine();
		if(reg.exactMatch(tmp))
		{
			if(reg.cap(2) == "n")
			{
				QString sn = reg.cap(3);
				QStringList ns = sn.split("|");
				ui->NodesTable->setRowCount(ns.length()-1);
				for(int i = 0; i < ns.length()-1; i++)
				{
					QTableWidgetItem* item = new QTableWidgetItem(ns[i]);
					ui->NodesTable->setItem(i,0,item);
				}
			}
			else if(reg.cap(2) == "b")
			{
				QString bn = reg.cap(3);
				QStringList bs = bn.split("|");
				ui->BarsTable->setRowCount(ui->BarsTable->rowCount()+1);
				for(int i = 0; i < bs.length()-1; i++)
				{
					QTableWidgetItem* item = new QTableWidgetItem(bs[i]);
					ui->BarsTable->setItem(ui->BarsTable->rowCount()-1,i,item);
				}

			}
			else if(reg.cap(2) == "f")
			{
				QString fn = reg.cap(3);
				QStringList fs = fn.split("|");
				ui->ForcesTable->setRowCount(ui->ForcesTable->rowCount()+1);
				QComboBox* cb = new QComboBox(this);
				cb->addItem("Сосредоточенная");
				cb->addItem("Распределенная");
				if(fs[0] == "Сосредоточенная")
				{
					cb->setCurrentIndex(0);
				}
				else
				{
					cb->setCurrentIndex(1);
				}
				connect(cb,SIGNAL(currentIndexChanged(int)),this,SLOT(on_ForcesTable_cellChanged()));
				ui->ForcesTable->setCellWidget(ui->ForcesTable->rowCount()-1,0,cb);
				for(int i = 1; i < fs.length()-1; i++)
				{
					QTableWidgetItem* item = new QTableWidgetItem(fs[i]);
					ui->ForcesTable->setItem(ui->ForcesTable->rowCount()-1,i,item);
				}
			}
			else if(reg.cap(2) == "fix")
			{
				QString fixn = reg.cap(3);
				QStringList fixs = fixn.split("|");
				for(int i = 0; i < fixs.length()-1; i++)
				{
					if(fixs[i] == "lfix")
					{
						ui->leftFixator->setChecked(true);
						leftFixator = true;
					}
					else if(fixs[i] == "rfix")
					{
						ui->RightFixator->setChecked(true);
						rightFixator = true;
					}
					ui->PainterObj->setFixators(leftFixator,rightFixator);
				}
			}
		}
	}

}

void MainWindow::on_submenuAbout_triggered()
{
	QMessageBox messageBox;
	messageBox.about(0,"САПР от Данилина А.О.","<h1>Выполнил студент группы ИДБ-15-15, Данилин Алексей Олегович</h1>");
}




void MainWindow::on_submenuClose_triggered()
{
	QMessageBox::StandardButton reply;
	  reply = QMessageBox::question(this, "Close project", "Вы уверенны что хотите закрыть проект?<br>Весь не сохраненный прогресс будет утрачен!",
									QMessageBox::Yes|QMessageBox::No);
	  if (reply == QMessageBox::Yes) {
		  ui->NodesTable->setRowCount(0);
		  ui->BarsTable->setRowCount(0);
		  ui->ForcesTable->setRowCount(0);
		  ui->PainterObj->clearArea();
		  ui->leftFixator->setChecked(false);
		  ui->RightFixator->setChecked(false);
		  curPro = "";
		  update();
	  }
}

void MainWindow::on_submenuProcessor_triggered()
{
	QRegExp num("([0-9]+)");
	QRegExp fnum("([0-9]{0,9}[\\.][0-9]{1,3})");
	QRegExp powr("([0-9]{0,9}[\\.]?[0-9]{1,3})[\\n\\t\\v\\s]*\\*[\\n\\t\\v\\s]*([0-9]{0,9}[\\.]?[0-9]{1,3})[\\n\\t\\v\\s]*\\^[\\n\\t\\v\\s]*(-?[0-9]{1,3})|([0-9]{0,9}[\\.]?[0-9]{1,3})");
	processor->setFixators(leftFixator,rightFixator);
	std::vector<std::vector<float>> bars;
	for(int i = 0; i < ui->BarsTable->rowCount(); i++)
	{
		if(checkBarsRow(i))
		{
			int ntrb = ui->BarsTable->item(i,0)->text().toInt()-1;
			int ntre = ui->BarsTable->item(i,1)->text().toInt()-1;
			float begin = ui->NodesTable->item(ntrb,0)->text().toFloat();
			float end = ui->NodesTable->item(ntre,0)->text().toFloat();
			float length = end - begin;
			QString dTmp = ui->BarsTable->item(i,2)->text();
			double d = 0;
			double a = 0;
			bool b1 = powr.exactMatch(dTmp);
			bool b2 = num.exactMatch(dTmp);
			bool b3 = fnum.exactMatch(dTmp);
			if(b2)
			{
				a = dTmp.toDouble();
				d = pow((a*4)/3.14,0.5);
			}
			else if(b3)
			{
				a = fnum.cap(1).toDouble();

				d = pow((a*4)/3.14,0.5);
			}
			else if(b1)
			{
				a = powr.cap(1).toDouble()*pow(powr.cap(2).toDouble(),powr.cap(3).toDouble());

				d = pow((a*4)/3.14,0.5);
			}
			std::vector<float> bar{ntrb,ntre,a,ui->BarsTable->item(i,3)->text().toFloat()};

			bars.push_back(bar);
		}
	}
	if(!bars.empty())
	{
		std::vector<float> _nodes;
		_nodes.clear();
		for(int i = 0; i < ui->NodesTable->rowCount(); i++)
		{
			if(ui->NodesTable->item(i,0) != NULL)
			{
				if(ui->NodesTable->item(i,0)->text() != "")
				{
					_nodes.push_back(ui->NodesTable->item(i,0)->text().toDouble());
				}
				else
				{
					_nodes.push_back(0);
				}
			}
			else
			{
				_nodes.push_back(0);
			}
		}

		processor->setNodes(_nodes);
		processor->setBars(bars);
	}
	else
	{
		std::vector<float> np;
		std::vector<std::vector<float>> bp;
		processor->setNodes(np);
		processor->setBars(bp);
	}

	std::vector<std::vector<float>> forces;
	for(int i = 0; i < ui->ForcesTable->rowCount(); i++)
	{

		if(checkForcesRow(i))
		{
			std::vector<float> force;
			float begin = 0;
			int type = 0;
			begin =  ui->ForcesTable->item(i,1)->text().toInt() - 1;
			if(qobject_cast<QComboBox*>(ui->ForcesTable->cellWidget(i,0))->currentText() == "Сосредоточенная")
			{

				type = 1;
			}
			else if(qobject_cast<QComboBox*>(ui->ForcesTable->cellWidget(i,0))->currentText() == "Распределенная")
			{
				type = 2;

			}
			force.push_back(type);
			force.push_back(begin);
			force.push_back(ui->ForcesTable->item(i,2)->text().toDouble());
			forces.push_back(force);

		}
	}
	if(!forces.empty())
	{
		processor->setForces(forces);
		processor->calculate();
	}
}

void MainWindow::on_RefreshImageButton_clicked()
{
	ui->PainterObj->draw();
}

void MainWindow::on_submenuPostProcessor_triggered()
{
	if(processor == nullptr)
	{
		processor = new Processor;
	}
	postprocessor->setProcessor(processor);
	postprocessor->show();
}

void MainWindow::on_action_triggered()
{
	QMessageBox::StandardButton reply;
	  reply = QMessageBox::question(this, "New project", "Вы уверенны что хотите создать новый проект?<br>Весь не сохраненный прогресс будет утрачен!",
									QMessageBox::Yes|QMessageBox::No);
	  if (reply == QMessageBox::Yes) {
		  ui->NodesTable->setRowCount(0);
		  ui->BarsTable->setRowCount(0);
		  ui->ForcesTable->setRowCount(0);
		  ui->PainterObj->clearArea();
		  ui->leftFixator->setChecked(false);
		  ui->RightFixator->setChecked(false);
		  curPro = "";
		  update();
	  }

}
