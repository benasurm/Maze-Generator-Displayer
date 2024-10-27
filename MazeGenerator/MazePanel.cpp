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
	is_panning = false;
	this->maze_field = maze_field;
	scale_factor = 1;
	cursor_offset = new wxPoint((this->GetSize().x - 2 * default_margin) / 2, 
		(this->GetSize().x - 2 * default_margin) / 2);
}

void MazePanel::SetCanvas(bool paint_maze, bool new_maze)
{
	this->paint_maze = paint_maze;
	this->new_maze = new_maze;
	Refresh();
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
		cursor_offset->x = (this->GetSize().x - 2 * default_margin) / 2;
		cursor_offset->y = (this->GetSize().y - 2 * default_margin) / 2;
	}

	int canvas_size = (this->GetSize().x - 2 * default_margin);
	int cell_size = GetCellSize(canvas, canvas_size);

	Position temp;
	wxPoint top_left, bot_right;
	top_left.x = top_left.y = 0;
	bot_right.x = bot_right.y = 0;
	
	canvas.SetPen(wxPen(wxColor(149, 0, 255), cell_size * cell_brd_ratio));
	for (uint32_t y = 0; y < maze_field->GetSize(); y++)
	{
		temp.y = y;
		for (uint32_t x = 0; x < maze_field->GetSize(); x++)
		{
			temp.x = x;
			int wall_value = maze_field->GetPos(temp);

			top_left.x = x * cell_size;
			top_left.y = y * cell_size;

			bot_right.x = (x + 1) * cell_size;
			bot_right.y = (y + 1) * cell_size;

			DrawWallsInCell(canvas, &top_left, &bot_right, wall_value, canvas_size, temp);
		}
	}
}

void MazePanel::DrawWallsInCell(wxAutoBufferedPaintDC& canvas, wxPoint* top_left,
	wxPoint* bot_right, int wall_value, int canvas_size, Position& temp)
{
	wxPoint from, to;
	wxPoint box_top_left_pos;
	box_top_left_pos.x = (cursor_offset->x - canvas_size / 2);
	box_top_left_pos.y = (cursor_offset->y - canvas_size / 2);
	if ((wall_value & (1 << (RIGHT - 1)))
		&& IsInBox(*bot_right, canvas_size))
	{
		from.x = bot_right->x - box_top_left_pos.x + default_margin;
		to.x = from.x;

		from.y = std::max(cursor_offset->y - canvas_size / 2, top_left->y)
			- box_top_left_pos.y + default_margin;
		to.y = std::min(cursor_offset->y + canvas_size / 2, bot_right->y)
			- box_top_left_pos.y + default_margin;

		canvas.DrawLine(from, to);
	}
	if (wall_value & (1 << (LEFT - 1))
		&& IsInBox(*top_left, canvas_size) && temp.x == 0)
	{
		from.x = top_left->x - box_top_left_pos.x + default_margin;
		to.x = from.x;

		from.y = std::max(cursor_offset->y - canvas_size / 2, top_left->y)
			- box_top_left_pos.y + default_margin;
		to.y = std::min(cursor_offset->y + canvas_size / 2, bot_right->y)
			- box_top_left_pos.y + default_margin;

		canvas.DrawLine(from, to);
	}
	if (wall_value & (1 << (DOWN - 1))
		&& IsInBox(*bot_right, canvas_size))
	{
		from.x = std::max(cursor_offset->x - canvas_size / 2, top_left->x)
			- box_top_left_pos.x + default_margin;
		to.x = std::min(cursor_offset->x + canvas_size / 2, bot_right->x)
			- box_top_left_pos.x + default_margin;

		from.y = bot_right->y - box_top_left_pos.y + default_margin;
		to.y = from.y;

		canvas.DrawLine(from, to);
	}
	if (wall_value & (1 << (TOP - 1))
		&& IsInBox(*top_left, canvas_size) && temp.y == 0)
	{		
		from.x = std::max(cursor_offset->x - canvas_size / 2, top_left->x)
			- box_top_left_pos.x + default_margin;
		to.x = std::min(cursor_offset->x + canvas_size / 2, bot_right->x)
			- box_top_left_pos.x + default_margin;

		from.y = top_left->y - box_top_left_pos.y + default_margin;
		to.y = from.y;

		canvas.DrawLine(from, to);
	}
}

void MazePanel::ClearCanvas(wxAutoBufferedPaintDC& canvas)
{
	canvas.SetBackground(wxBrush(wxColour(canvas_bkgr_clr, canvas_bkgr_clr, canvas_bkgr_clr)));
	canvas.Clear();
}

int MazePanel::GetCellSize(wxAutoBufferedPaintDC& canvas, int canvas_size)
{
	int n = maze_field->GetSize();
	return canvas_size * scale_factor / n;
}

bool MazePanel::IsInBox(wxPoint& point, int canvas_size)
{
	return point.x >= cursor_offset->x - canvas_size / 2
		&& point.x <= cursor_offset->x + canvas_size / 2
		&& point.y >= cursor_offset->y - canvas_size / 2
		&& point.y <= cursor_offset->y + canvas_size / 2;
}

void MazePanel::OnZoom(wxMouseEvent& event)
{
	if (!MouseInCanvas()) return;
	double update = 0.1;
	if (event.GetWheelRotation() < 0) update *= -1;
	if ((update < 0 && scale_factor > 0.8) || (update > 0 && scale_factor < 3))
	{
		scale_factor += update;
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
	if (is_panning && MouseInCanvas())
	{
		wxPoint curr_mouse_pos = GetMouseCanvasPos();
		wxPoint diff = curr_mouse_pos - last_cursor_pos;

		int canvas_size = (this->GetSize().x - 2 * default_margin);
		int scaled_canvas_size = canvas_size * scale_factor;

		cursor_offset->x = std::clamp(cursor_offset->x - diff.x, 200, scaled_canvas_size - 200);
		cursor_offset->y = std::clamp(cursor_offset->y - diff.y, 200, scaled_canvas_size - 200);
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