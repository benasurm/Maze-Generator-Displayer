#pragma once

#include <wx/wx.h>
#include "MazeField.h"
#include "MazePanel.h"

#define MIN_MAZE_SIZE 4
#define MAX_MAZE_SIZE 100
#define DEF_MAZE_SIZE 20

enum custom_ids
{
	GENERATE_BUTTON_ID = 2,
	SHOW_SOLUTION_ID = 3
};

class AppFrame : public wxFrame
{
public:
	AppFrame(const wxString& title);

	wxPanel* app_panel;
	wxSpinCtrl* size_spin_ctrl;
	wxStaticText* value_spin_label;
	wxButton* gen_button;
	wxButton* solution_button;
	MazePanel* maze_panel;
private:
	MazeField* maze_field;

	void OnGenerateButtonClick(wxCommandEvent &event);
	wxDECLARE_EVENT_TABLE();
};
