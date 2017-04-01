#ifndef PLANTBLUEPRINTDIALOG_H_
#define PLANTBLUEPRINTDIALOG_H_

#include <array>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPalette>

//----------------------------------------------------------------------------------------------------------------------
/// @file PlantBlueprintDialog.h
/// @brief this class contains the user interface required to create a new Plant Blueprint object
/// @author Neerav Nagda
/// @version 1.0
/// @date 31/03/17
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
/// @struct Validation
/// @brief contains enum variables
/// @enum variables
/// @brief enumarated names for the validation array
//----------------------------------------------------------------------------------------------------------------------
struct Validation
{
		enum variables
		{
			BLUEPRINTNAME = 0,
			GRAMMARFILE = 1,
			COUNT = 2
		};
};
typedef Validation::variables ValidationVariables;

		namespace Ui {
		class PlantBlueprintDialog;
		}

//----------------------------------------------------------------------------------------------------------------------
/// @class PlantBlueprintDialog
/// @brief this class manages the UI and UI functions
//----------------------------------------------------------------------------------------------------------------------
		class PlantBlueprintDialog : public QDialog
		{
				Q_OBJECT

			public slots:
				//----------------------------------------------------------------------------------------------------------------------
				/// @brief check if the blueprint already exists
				//----------------------------------------------------------------------------------------------------------------------
				void checkBlueprintExists();
				//----------------------------------------------------------------------------------------------------------------------
				/// @brief check if the file exists
				/// Change text colour to red if the file does not exist, or green if the file exists
				//----------------------------------------------------------------------------------------------------------------------
				void checkGrammarFileExists();
				//----------------------------------------------------------------------------------------------------------------------
				/// @brief reset text colour to black
				//----------------------------------------------------------------------------------------------------------------------
				void resetGrammarFileTextColour();

			public:
				//----------------------------------------------------------------------------------------------------------------------
				/// @brief constructor
				//----------------------------------------------------------------------------------------------------------------------
				explicit PlantBlueprintDialog(QWidget *parent = 0);
				//----------------------------------------------------------------------------------------------------------------------
				/// @brief destructor
				//----------------------------------------------------------------------------------------------------------------------
				~PlantBlueprintDialog();
				//----------------------------------------------------------------------------------------------------------------------
				/// @brief get function for the UI
				/// @return reference to the UI in this class
				//----------------------------------------------------------------------------------------------------------------------
				const Ui::PlantBlueprintDialog& getUI() {return *m_ui;}
				//----------------------------------------------------------------------------------------------------------------------
				/// @brief create a new instance of the Plant Blueprint
				/// @return status if a new Plant Blueprint was created
				//----------------------------------------------------------------------------------------------------------------------
				bool createPlantBlueprint();

			private:
				//----------------------------------------------------------------------------------------------------------------------
				/// @brief ui for this class
				//----------------------------------------------------------------------------------------------------------------------
				Ui::PlantBlueprintDialog *m_ui;
				//----------------------------------------------------------------------------------------------------------------------
				/// @brief default palette to reset to
				/// This resets text colour to black
				//----------------------------------------------------------------------------------------------------------------------
				QPalette c_defaultPalette;
				//----------------------------------------------------------------------------------------------------------------------
				/// @brief check if the ui element values are valid
				/// All of the values must be set to true to create a new blueprint
				//----------------------------------------------------------------------------------------------------------------------
				std::array<bool, ValidationVariables::COUNT> m_validationChecks;
		};

#endif // PLANTBLUEPRINTDIALOG_H_
