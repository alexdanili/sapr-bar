#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "processor.h"
#include "postprocessor.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QApplication>
#include <QTableWidget>
#include <QStyledItemDelegate>
#include <QItemDelegate>
#include <regex>
#include <QRegExp>
#include <QPainter>
#include <QComboBox>
#include <QLineEdit>
#include <QObject>

namespace Ui {
class MainWindow;
}
class PostProcessor;
class NodesDlegate : public QItemDelegate
{
public:
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;

};

class BarsDlegate : public QItemDelegate
{
public:
    QWidget *createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;

};

class ForcesDlegate : public QStyledItemDelegate
{
public:
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;
};

    class MainWindow : public QMainWindow
    {
        Q_OBJECT

	public:
        explicit MainWindow(QWidget *parent = 0);
        bool leftFixator = false;
		QString curPro;
		bool UserCh = true;
        bool rightFixator = false;
		Processor* processor;
		PostProcessor* postprocessor;
		bool sorting = false;
        bool error;
		QImage* image;
        ~MainWindow();

    private slots:

        void on_addButton_released(std::string table);

        void on_deleteButton_released(std::string table);

        void on_SaveImageButton_released();

        void on_submenuAbout_triggered();

        void on_ScaleValue_valueChanged(int value);

        void on_leftFixator_clicked(bool checked);

        void on_RightFixator_clicked(bool checked);

        void on_NodesTable_cellChanged();

        void on_BarsTable_cellChanged(int row, int column);

		void on_ForcesTable_cellChanged();

        void on_submenuSave_triggered();

        void on_submenuOpen_triggered();

		void on_submenuClose_triggered();

        void on_submenuProcessor_triggered();

		void on_RefreshImageButton_clicked();

		void on_submenuPostProcessor_triggered();

		void on_action_triggered();

	private:
		Ui::MainWindow *ui;
        bool checkBarsRow(int row);
        bool checkForcesRow(int row);
    protected:
		void resizeEvent(QResizeEvent* event);

    };

#endif // MAINWINDOW_H
