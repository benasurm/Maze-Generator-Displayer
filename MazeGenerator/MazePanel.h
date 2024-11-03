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
	bool GetSolutionState();
	void ToggleSolutionState();
private:
	// Private members
	// Constants
	const double cell_brd_ratio = 0.3;
	const int canvas_bkgr_clr = 210;
	const int default_margin = 25;

	// Drawing options
	bool paint_maze, new_maze, show_solution;
	
	// Current state parameters
	bool is_panning;
	double scale_factor;
	int viewport_size, scaled_maze_size;
	wxPoint cursor_offset;
	wxPoint last_cursor_pos;
	wxPoint diff;
	wxPoint start_point;

	// Pointer to maze field object
	MazeField* maze_field;

	// Private methods
	
	// Drawing methods
	void RepaintMaze(wxPaintEvent& event);
	void DrawPathCell(wxAutoBufferedPaintDC& canvas, wxPoint& top_left,
		wxPoint& bot_right, int wall_value, Position& temp, int cell_size, wxPoint& box_top_left_pos);
	void DrawWallsInCell(wxAutoBufferedPaintDC& canvas, wxPoint& top_left,
		wxPoint& bot_right, int wall_value, Position& temp, int cell_size, wxPoint& box_top_left_pos);
	void ClearCanvas(wxAutoBufferedPaintDC& canvas);
	
	// Event methods
	void OnZoom(wxMouseEvent& event);
	void OnMouseClick(wxMouseEvent& event);
	void OnMouseRelease(wxMouseEvent& event);
	void OnMouseDrag(wxMouseEvent& event);

	// Computation methods
	bool IsInBox(wxPoint& point);
	bool IsAnyPointInBox(wxPoint& top_left, wxPoint& top_right,
		wxPoint& bot_left, wxPoint& bot_right);
	int GetCellSize(wxAutoBufferedPaintDC& canvas);
	void ResetDiff();
	void SetStartPoint(int cell_size);
	void SetOffset(int margin);
	void ClampDiff(int& diff);
	void SetDraggedDiff(int& diff, int curr_mouse_pos, int last_cursor_pos);

	// Mouse methods
	wxPoint GetMouseCanvasPos();
	bool MouseInCanvas();

};