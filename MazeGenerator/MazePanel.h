#pragma once

#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include "MazeField.h"

class MazePanel : public wxPanel
{
public:
	// Constructor
	MazePanel(wxWindow* parent, MazeField* maze_field);
	// Public methods
	void SetCanvas(bool paint_maze, bool new_maze);
private:
	// Private members

	// Constants
	const double cell_brd_ratio = 0.3;
	const int canvas_bkgr_clr = 210;
	const int default_margin = 25;

	bool paint_maze, new_maze;
	bool is_panning;
	double scale_factor;
	MazeField* maze_field;
	wxPoint* cursor_offset;
	wxPoint last_cursor_pos;
	// Private methods
	void RepaintMaze(wxPaintEvent& event);
	void OnZoom(wxMouseEvent& event);
	void OnMouseClick(wxMouseEvent& event);
	void OnMouseRelease(wxMouseEvent& event);
	void OnMouseDrag(wxMouseEvent& event);
	void DrawWallsInCell(wxAutoBufferedPaintDC& canvas, wxPoint* top_left,
		wxPoint* bot_right, int wall_value, int canvas_size, Position& temp);
	void ClearCanvas(wxAutoBufferedPaintDC& canvas);
	int GetCellSize(wxAutoBufferedPaintDC& canvas, int canvas_size);
	bool IsInBox(wxPoint& point, int canvas_size);
	wxPoint GetMouseCanvasPos();
	bool MouseInCanvas();
};