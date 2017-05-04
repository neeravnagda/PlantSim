#ifndef SCENEMANAGERDIALOG_H
#define SCENEMANAGERDIALOG_H

#include <QDialog>
#include <QString>

namespace Ui {
class SceneManagerDialog;
}

class SceneManagerDialog : public QDialog
{
		Q_OBJECT

	public:
		explicit SceneManagerDialog(QWidget *parent = 0);
		~SceneManagerDialog();
		void addRow(const QString& _plantType, const float& x, const float& y, const float& z);
		const Ui::SceneManagerDialog& getUI() {return *m_ui;}

	signals:
		void plantVisibility(unsigned _index, bool _state);
		void deletePlantSignal(unsigned _index);

	private slots:
		void setCurrentRow(int _r, int _c, int _pr, int _pc);
		void togglePlantVisibility();
		void removePlant();

	private:
		Ui::SceneManagerDialog *m_ui;
		int m_currentRow;
};

#endif // SCENEMANAGERDIALOG_H