#include "AppFrame.h"
#include <wx/wx.h>
#include <wx/spinctrl.h>

// Event table declaration

wxBEGIN_EVENT_TABLE(AppFrame, wxFrame)
	EVT_BUTTON(GENERATE_BUTTON_ID, AppFrame::OnGenerateButtonClick)
	EVT_BUTTON(SHOW_SOLUTION_ID, AppFrame::OnSolutionButtonClick)
	EVT_BUTTON(CENTER_MAZE_ID, AppFrame::OnCenterButtonClick)
wxEND_EVENT_TABLE()

// Public method implementations

AppFrame::AppFrame(const wxString& title):
	wxFrame(nullptr, wxID_ANY, title)
{
	maze_field = new MazeField(20);
	
	app_panel = new wxPanel(this);
	app_panel->SetPosition(wxPoint(0, 0));
	app_panel->SetSize(wxSize(320, 600));
	app_panel->SetOwnBackgroundColour(wxColour(180, 180, 180));

	maze_panel = new MazePanel(this, maze_field);

	value_spin_label =
		new wxStaticText(app_panel, wxID_ANY, "Select maze side length", wxPoint(50, 80), wxSize(-1, 40));
	value_spin_label->SetFont(this->GetFont().Scale(1.5));

	size_spin_ctrl = new wxSpinCtrl(app_panel, wxID_ANY, "", wxPoint(50, 120), wxSize(100, 40));
	size_spin_ctrl->SetMin(MIN_MAZE_SIZE);
	size_spin_ctrl->SetMax(MAX_MAZE_SIZE);
	size_spin_ctrl->SetValue(DEF_MAZE_SIZE);
	size_spin_ctrl->SetFont(size_spin_ctrl->GetFont().Scale(1.5));

	gen_button = new wxButton(app_panel, GENERATE_BUTTON_ID, "Generate new maze", wxPoint(50, 180), wxSize(200, 30));
	solution_button = new wxButton(app_panel, SHOW_SOLUTION_ID, "Show solution", wxPoint(50, 220), wxSize(200, 30));
	center_button = new wxButton(app_panel, CENTER_MAZE_ID, "Center maze", wxPoint(50, 260), wxSize(200, 30));
	solution_button->Enable(false);
	center_button->Enable(false);
}

// Private method implementations

void AppFrame::OnGenerateButtonClick(wxCommandEvent& event)
{
	uint32_t spin_value = size_spin_ctrl->GetValue();
	maze_field->SetNewMaze(spin_value);
	maze_field->GenerateRandMaze();
	maze_field->ComputeObjsToDraw();
	if(maze_panel->GetSolutionState()) maze_panel->ToggleSolutionState();
	solution_button->SetLabel("Show solution");
	maze_panel->SetCanvas(true, true);
	solution_button->Enable(true);
	center_button->Enable(true);
}

void AppFrame::OnSolutionButtonClick(wxCommandEvent& event)
{
	maze_panel->ToggleSolutionState();
	switch (maze_panel->GetSolutionState())
	{
		case true:
			solution_button->SetLabel("Hide solution");
			break;
		case false:
			solution_button->SetLabel("Show solution");
			break;
	}
	maze_panel->SetCanvas(true, false);
}

void AppFrame::OnCenterButtonClick(wxCommandEvent& event)
{
	maze_panel->SetCanvas(true, true);
}