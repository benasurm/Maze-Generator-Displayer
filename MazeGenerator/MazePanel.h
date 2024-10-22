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

	// Constants
	const double cell_brd_ratio = 0.3;
	const int canvas_bkgr_clr = 210;
	const int default_margin = 25;

	bool paint_maze;
	MazeField* maze_field;
	wxPoint* start_point;
	// Private methods
	void RepaintMaze(wxPaintEvent& event);
	void DrawWallsInCell(wxPaintDC* canvas, wxPoint* from, wxPoint* to, int wall_value,
		int cell_size, Position& curr_pos);
	void ClearCanvas(wxPaintDC* canvas);
	void SetOffset(int value);
	void SetCellAndBorderSize(wxPaintDC* canvas, int& cell_size,
		int& offset);
};