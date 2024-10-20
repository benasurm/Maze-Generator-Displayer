#pragma once

#include <wx/wx.h>
#include "MazeField.h"

class MazePanel : public wxPanel
{
public:
	// Constructor
	MazePanel(wxWindow* parent, MazeField* maze_field);

	// Public methods
	void SetCanvas(bool paint_maze);
private:
	// Private members
	bool paint_maze;
	MazeField* maze_field;

	// Private methods
	void RepaintMaze(wxPaintEvent& event);
	void DrawWallsInCell(wxPaintDC* canvas, wxPoint* start_point,
		wxPoint* from, wxPoint* to, int wall_value, int cell_size, int border_size, int x, int y);
};

