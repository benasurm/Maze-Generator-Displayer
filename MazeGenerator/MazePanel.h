#pragma once

#include <wx/wx.h>
#include <wx/log.h>
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
	void DrawWallsInCell(wxPaintDC* canvas, int wall_value);
};

