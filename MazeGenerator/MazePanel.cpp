#include "MazePanel.h"

// Public method implementations

MazePanel::MazePanel(wxWindow* parent, MazeField* maze_field)
	: wxPanel(parent)
{
	this->SetPosition(wxPoint(320, 30));
	this->SetSize(wxSize(650, 650));
	this->SetBackgroundColour(wxColour(180, 180, 180));
	Bind(wxEVT_PAINT, &MazePanel::RepaintMaze, this);
	paint_maze = false;
	this->maze_field = maze_field;
	start_point = new wxPoint(25, 25);
}

void MazePanel::SetCanvas(bool paint_maze)
{
	this->paint_maze = paint_maze;
	Refresh();
}

// Private method implementations

void MazePanel::RepaintMaze(wxPaintEvent& event)
{
	wxPaintDC* canvas = new wxPaintDC(this);
	ClearCanvas(canvas);

	if (!paint_maze) return;

	SetOffset(default_margin);
	int cell_size, offset;
	SetCellAndBorderSize(canvas, cell_size, offset);
	SetOffset(offset);
	
	Position temp;
	wxPoint* from = new wxPoint(0, 0);
	wxPoint* to = new wxPoint(0, 0);
	
	canvas->SetPen(wxPen(wxColor(149, 0, 255), cell_size * cell_brd_ratio));
	
	for (uint32_t y = 0; y < maze_field->GetSize(); y++)
	{
		temp.y = y;
		for (uint32_t x = 0; x < maze_field->GetSize(); x++)
		{
			temp.x = x;
			int wall_value = maze_field->GetPos(temp);
			DrawWallsInCell(canvas, from, to, wall_value, cell_size, temp);
		}
	}
}

void MazePanel::DrawWallsInCell(wxPaintDC* canvas, wxPoint* from, wxPoint* to, int wall_value,
	int cell_size, Position &curr_pos)
{
	if (wall_value >= (1 << (RIGHT - 1)))
	{
		wall_value -= (1 << (RIGHT - 1));

		from->x = start_point->x + (curr_pos.x + 1) * (cell_size);
		to->x = from->x;
		from->y = start_point->y + curr_pos.y * (cell_size);
		to->y = from->y + (cell_size);

		canvas->DrawLine(*from, *to);
	}
	if (wall_value >= (1 << (LEFT - 1)))
	{
		wall_value -= (1 << (LEFT - 1));

		from->x = start_point->x + curr_pos.x * (cell_size);
		to->x = from->x;
		from->y = start_point->y + curr_pos.y * (cell_size);
		to->y = from->y + (cell_size);

		canvas->DrawLine(*from, *to);
	}
	if (wall_value >= (1 << (DOWN - 1)))
	{
		wall_value -= (1 << (DOWN - 1));

		from->x = start_point->x + curr_pos.x * (cell_size);
		to->x = from->x + (cell_size);
		from->y = start_point->y + (curr_pos.y + 1) * (cell_size);
		to->y = from->y;

		canvas->DrawLine(*from, *to);
	}
	if (wall_value >= TOP)
	{		
		wall_value--;

		from->x = start_point->x + curr_pos.x * (cell_size);
		to->x = from->x + (cell_size);
		from->y = start_point->y + curr_pos.y * (cell_size);
		to->y = from->y;

		canvas->DrawLine(*from, *to);
	}
}

void MazePanel::ClearCanvas(wxPaintDC* canvas)
{
	canvas->SetBackground(wxBrush(wxColour(canvas_bkgr_clr, canvas_bkgr_clr, canvas_bkgr_clr)));
	canvas->Clear();
}

void MazePanel::SetOffset(int value)
{
	start_point->x = value;
	start_point->y = value;
}

void MazePanel::SetCellAndBorderSize(wxPaintDC* canvas, int& cell_size, 
	int& offset)
{
	int canvas_w, canvas_h;
	int n = maze_field->GetSize();
	canvas->GetSize(&canvas_w, &canvas_h);
	canvas_w -= 2 * start_point->x;

	cell_size = canvas_w / n;
	int round_maze_len = cell_size * n;
	offset = (canvas_h - round_maze_len) / 2;
}