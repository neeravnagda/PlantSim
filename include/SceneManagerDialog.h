#ifndef SCENEMANAGERDIALOG_H
#define SCENEMANAGERDIALOG_H

#include <QDialog>
#include <QString>

//----------------------------------------------------------------------------------------------------------------------
/// @file SceneManagerDialog.h
/// @brief this class contains allows the management of objects in the scene
/// @author Neerav Nagda
/// @version 1.0
/// @date 11/05/17
/// @class SceneManagerDialog
/// @brief This class manages the ability to delete and hide objects in the scene
//----------------------------------------------------------------------------------------------------------------------

namespace Ui {
class SceneManagerDialog;
}

class SceneManagerDialog : public QDialog
{
		Q_OBJECT

	public:
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Constructor
		/// @param parent The object to parent to
		//----------------------------------------------------------------------------------------------------------------------
		explicit SceneManagerDialog(QWidget *parent = 0);
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Destructor
		//----------------------------------------------------------------------------------------------------------------------
		~SceneManagerDialog();
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Add a row into the table
		/// @param _plantType The type of plant blueprint
		///	@param _x The x position of the plant
		/// @param _y The y position of the plant
		/// @param _z The z position of the plant
		//----------------------------------------------------------------------------------------------------------------------
		void addRow(const QString& _plantType, const float& _x, const float& _y, const float& _z);
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get function for the UI
		/// @return The UI
		//----------------------------------------------------------------------------------------------------------------------
		const Ui::SceneManagerDialog& getUI() {return *m_ui;}

	signals:
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Signal to change plant visibility
		/// @param [out] _index The index into the container of plants
		/// @param [out] _state The new visibility state
		//----------------------------------------------------------------------------------------------------------------------
		void plantVisibility(unsigned _index, bool _state);
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Signal to delete a plant
		/// @param [out] _index The index into the container of plants
		//----------------------------------------------------------------------------------------------------------------------
		void deletePlantSignal(unsigned _index);

	private slots:
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Slot to toggle the plant visibility
		/// This changes the state of all checked items and then emits the plantVisibility signal
		//----------------------------------------------------------------------------------------------------------------------
		void togglePlantVisibility();
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Slot to remove plants
		/// This removes all the checked plants from the table and then emits the deletePlantSignal
		//----------------------------------------------------------------------------------------------------------------------
		void removePlant();

	private:
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief The UI
		//----------------------------------------------------------------------------------------------------------------------
		Ui::SceneManagerDialog *m_ui;
};

#endif // SCENEMANAGERDIALOG_H
