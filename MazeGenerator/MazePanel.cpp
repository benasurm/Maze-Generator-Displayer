#include "MazePanel.h"

// Public method implementations

MazePanel::MazePanel(wxWindow* parent, MazeField* maze_field)
	: wxPanel(parent)
{
	this->SetPosition(wxPoint(320, 30));
	this->SetSize(wxSize(650, 650));
	this->SetBackgroundStyle(wxBG_STYLE_PAINT);
	this->SetBackgroundColour(wxColour(180, 180, 180));
	Bind(wxEVT_PAINT, &MazePanel::RepaintMaze, this);
	Bind(wxEVT_MOUSEWHEEL, &MazePanel::OnZoom, this);
	Bind(wxEVT_LEFT_DOWN, &MazePanel::OnMouseClick, this);
	Bind(wxEVT_LEFT_UP, &MazePanel::OnMouseRelease, this);
	Bind(wxEVT_MOTION, &MazePanel::OnMouseDrag, this);
	paint_maze = false;
	new_maze = false;
	show_solution = false;
	is_panning = false;
	scale_factor = 1;
	viewport_size = (this->GetSize().x - 2 * default_margin);
	scaled_maze_size = viewport_size;
	cursor_offset.x = viewport_size / 2;
	cursor_offset.y = viewport_size / 2;
	last_cursor_pos.x = cursor_offset.x;
	last_cursor_pos.y = cursor_offset.y;
	ResetDiff();
	this->maze_field = maze_field;
}

void MazePanel::SetCanvas(bool paint_maze, bool new_maze)
{
	this->paint_maze = paint_maze;
	this->new_maze = new_maze;
	Refresh();
}

bool MazePanel::GetSolutionState()
{
	return show_solution;
}

void MazePanel::ToggleSolutionState()
{
	show_solution = !show_solution;
}

// Private method implementations

void MazePanel::RepaintMaze(wxPaintEvent& event)
{
	wxAutoBufferedPaintDC canvas(this);
	ClearCanvas(canvas);

	if (!paint_maze) return;
	if (new_maze)
	{
		scale_factor = 1;
		ResetDiff();
	}
	SetOffset(default_margin);
	scaled_maze_size = viewport_size * scale_factor;
	int cell_size = GetCellSize(canvas);
	SetStartPoint(cell_size);

	cursor_offset.x = (scaled_maze_size) / 2 - diff.x * scale_factor;
	cursor_offset.y = (scaled_maze_size) / 2 - diff.y * scale_factor;

	Position temp;
	wxPoint top_left, bot_right;
	top_left.x = top_left.y = 0;
	bot_right.x = bot_right.y = 0;
	
	for (uint32_t y = 0; y < maze_field->GetSize(); y++)
	{
		temp.y = y;
		for (uint32_t x = 0; x < maze_field->GetSize(); x++)
		{
			temp.x = x;
			int wall_value = maze_field->GetPos(temp);

			top_left.x = x * cell_size;
			top_left.y = y * cell_size;

			bot_right.x = top_left.x + cell_size;
			bot_right.y = top_left.y + cell_size;

			wxPoint from, to;
			wxPoint box_top_left_pos;
			box_top_left_pos.x = (cursor_offset.x - viewport_size / 2);
			box_top_left_pos.y = (cursor_offset.y - viewport_size / 2);

			wxPoint top_right = top_left;
			top_right.x = bot_right.x;
			wxPoint bot_left = bot_right;
			bot_left.x = top_left.x;

			if ((wall_value & (1 << (PATH_CELL - 1)))
				&& IsAnyPointInBox(top_left, top_right, bot_left, bot_right) && show_solution)
			{
				from.x = std::max(cursor_offset.x - viewport_size / 2, top_left.x)
					- box_top_left_pos.x + start_point.x;
				from.y = std::max(cursor_offset.y - viewport_size / 2, top_left.y)
					- box_top_left_pos.y + start_point.y;

				to.x = std::min(cursor_offset.x + viewport_size / 2, bot_right.x)
					- box_top_left_pos.x + start_point.x;
				to.y = std::min(cursor_offset.y + viewport_size / 2, bot_right.y)
					- box_top_left_pos.y + start_point.y;

				canvas.SetBrush(*wxGREEN_BRUSH);
				canvas.SetPen(*wxTRANSPARENT_PEN);
				canvas.DrawRectangle(from.x, from.y, to.x - from.x, to.y - from.y);
			}
		}
	}

	for (uint32_t y = 0; y < maze_field->GetSize(); y++)
	{
		temp.y = y;
		for (uint32_t x = 0; x < maze_field->GetSize(); x++)
		{
			temp.x = x;
			int wall_value = maze_field->GetPos(temp);

			top_left.x = x * cell_size;
			top_left.y = y * cell_size;

			bot_right.x = top_left.x + cell_size;
			bot_right.y = top_left.y + cell_size;

			DrawWallsInCell(canvas, top_left, bot_right, wall_value, temp, cell_size);
		}
	}
}

void MazePanel::DrawWallsInCell(wxAutoBufferedPaintDC& canvas, wxPoint& top_left,
	wxPoint& bot_right, int wall_value, Position& temp, int cell_size)
{
	wxPoint from, to;
	wxPoint box_top_left_pos;
	box_top_left_pos.x = (cursor_offset.x - viewport_size / 2);
	box_top_left_pos.y = (cursor_offset.y - viewport_size / 2);

	canvas.SetPen(wxPen(wxColor(149, 0, 255), cell_size * cell_brd_ratio));
	if ((wall_value & (1 << (RIGHT - 1)))
		&& IsInBox(bot_right))
	{
		from.x = bot_right.x - box_top_left_pos.x + start_point.x;
		to.x = from.x;

		from.y = std::max(cursor_offset.y - viewport_size / 2, top_left.y)
			- box_top_left_pos.y + start_point.y;
		to.y = std::min(cursor_offset.y + viewport_size / 2, bot_right.y)
			- box_top_left_pos.y + start_point.y;

		canvas.DrawLine(from, to);
	}
	if (wall_value & (1 << (LEFT - 1))
		&& IsInBox(top_left))
	{
		from.x = top_left.x - box_top_left_pos.x + start_point.x;
		to.x = from.x;

		from.y = std::max(cursor_offset.y - viewport_size / 2, top_left.y)
			- box_top_left_pos.y + start_point.y;
		to.y = std::min(cursor_offset.y + viewport_size / 2, bot_right.y)
			- box_top_left_pos.y + start_point.y;

		canvas.DrawLine(from, to);
	}
	if (wall_value & (1 << (DOWN - 1))
		&& IsInBox(bot_right))
	{
		from.x = std::max(cursor_offset.x - viewport_size / 2, top_left.x)
			- box_top_left_pos.x + start_point.x;
		to.x = std::min(cursor_offset.x + viewport_size / 2, bot_right.x)
			- box_top_left_pos.x + start_point.x;

		from.y = bot_right.y - box_top_left_pos.y + start_point.y;
		to.y = from.y;

		canvas.DrawLine(from, to);
	}
	if (wall_value & (1 << (TOP - 1))
		&& IsInBox(top_left) && temp.y == 0)
	{		
		from.x = std::max(cursor_offset.x - viewport_size / 2, top_left.x)
			- box_top_left_pos.x + start_point.x;
		to.x = std::min(cursor_offset.x + viewport_size / 2, bot_right.x)
			- box_top_left_pos.x + start_point.x;

		from.y = top_left.y - box_top_left_pos.y + start_point.y;
		to.y = from.y;

		canvas.DrawLine(from, to);
	}
}

void MazePanel::ClearCanvas(wxAutoBufferedPaintDC& canvas)
{
	canvas.SetBackground(wxBrush(wxColour(canvas_bkgr_clr, canvas_bkgr_clr, canvas_bkgr_clr)));
	canvas.Clear();
}

bool MazePanel::IsInBox(wxPoint& point)
{
	int half_viewport = (viewport_size / 2);
	return point.x >= cursor_offset.x - half_viewport
		&& point.x <= cursor_offset.x + half_viewport
		&& point.y >= cursor_offset.y - half_viewport
		&& point.y <= cursor_offset.y + half_viewport;
}

bool MazePanel::IsAnyPointInBox(wxPoint& top_left, wxPoint& top_right,
	wxPoint& bot_left, wxPoint& bot_right)
{
	return (IsInBox(top_left) || IsInBox(top_right)
		|| IsInBox(bot_left) || IsInBox(bot_right));
}

int MazePanel::GetCellSize(wxAutoBufferedPaintDC& canvas)
{
	int n = maze_field->GetSize();
	return scaled_maze_size / n;
}

void MazePanel::ResetDiff()
{
	diff.x = diff.y = 0;
}

void MazePanel::SetStartPoint(int cell_size)
{
	if (scale_factor <= 1)
	{
		int round_maze_len = cell_size * maze_field->GetSize();
		int offset = ((viewport_size + 2 * default_margin) - round_maze_len) / 2;
		SetOffset(offset);
	}
}

void MazePanel::SetOffset(int margin)
{
	start_point.x = start_point.y = margin;
}

void MazePanel::ClampDiff(int& diff)
{
	int diff_bound = (scaled_maze_size - viewport_size) / (2 * scale_factor);
	if (diff > diff_bound)
	{
		diff = diff_bound;
	}
	else if (diff < (-1 * diff_bound))
	{
		diff = (- 1 * diff_bound);
	}
}

void MazePanel::SetDraggedDiff(int& diff, int curr_mouse_pos, int last_cursor_pos)
{
	int delta = diff * scale_factor + (curr_mouse_pos - last_cursor_pos);
	if (delta <= (scaled_maze_size - viewport_size) / 2 &&
		delta >= (viewport_size - scaled_maze_size) / 2)
	{
		diff += (curr_mouse_pos - last_cursor_pos);
	}
}

void MazePanel::OnZoom(wxMouseEvent& event)
{
	if (!MouseInCanvas()) return;
	double update = 0.1;
	if (event.GetWheelRotation() < 0) update *= -1;
	if ((update < 0 && scale_factor > 1) || (update > 0 && scale_factor < 3))
	{		
		scale_factor += update;
		int scaled_maze_size = (viewport_size) * scale_factor;

		if (scaled_maze_size > viewport_size)
		{
			ClampDiff(diff.x);
			ClampDiff(diff.y);
		}
		else ResetDiff();
		SetCanvas(true, false);
	}
}

void MazePanel::OnMouseClick(wxMouseEvent& event)
{
	if (MouseInCanvas())
	{
		last_cursor_pos = GetMouseCanvasPos();
		is_panning = true;
	}
}

void MazePanel::OnMouseRelease(wxMouseEvent& event)
{
	is_panning = false;
}

void MazePanel::OnMouseDrag(wxMouseEvent& event)
{
	if (is_panning)
	{
		wxPoint curr_mouse_pos = GetMouseCanvasPos();
		wxPoint delta;
		int scaled_maze_size = (viewport_size) * scale_factor;
		
		if (scaled_maze_size > viewport_size)
		{
			SetDraggedDiff(diff.x, curr_mouse_pos.x, last_cursor_pos.x);
			SetDraggedDiff(diff.y, curr_mouse_pos.y, last_cursor_pos.y);
		}
		else ResetDiff();
		last_cursor_pos = curr_mouse_pos;
		SetCanvas(true, false);
	}
}

wxPoint MazePanel::GetMouseCanvasPos()
{
	wxPoint cursor_pos = wxGetMousePosition();
	cursor_pos.x -= this->GetScreenPosition().x;
	cursor_pos.y -= this->GetScreenPosition().y;
	return cursor_pos;
}

bool MazePanel::MouseInCanvas()
{
	wxPoint cursor_pos = GetMouseCanvasPos();
	return (cursor_pos.x >= 0
		&& cursor_pos.x <= this->GetSize().x
		&& cursor_pos.y >= 0 
		&& cursor_pos.y <= this->GetSize().y);
}