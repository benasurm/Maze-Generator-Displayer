#include "MazeField.h"
#include <codecvt>
#include <locale>
#include <cmath>

// Public method implementations

MazeField::MazeField(const uint32_t maze_size)
{
    SetNewField(node_arr, 0, maze_size);
    SetNewField(wall_arr, 15, maze_size);
}

uint32_t MazeField::GetSize()
{
    return node_arr.size();
}

int MazeField::GetPos(Position& to_get)
{
    if (AreCoordsInBounds(to_get)) return wall_arr[to_get.y][to_get.x];
    return 0;
}


void MazeField::GenerateRandMaze()
{
    srand(time(0));
    SetStartEndPos();
    VisitRandomNodes();
}

void MazeField::SetNewMaze(const uint32_t maze_size)
{
    SetNewField(node_arr, 0, maze_size);
    SetNewField(wall_arr, 15, maze_size);
    while (!path_to_end.empty()) path_to_end.pop();
}

void MazeField::ComputeWallsToDraw()
{
    Position curr_pos, next_pos;
    int opp_dir;
    for (uint32_t y = 0; y < node_arr.size(); y++)
    {
        for (uint32_t x = 0; x < node_arr.size(); x++)
        {
            curr_pos.x = x;
            curr_pos.y = y;
            wall_arr[y][x] -= 1 << (node_arr[y][x] - 1);
            for (int i = 1; i <= 4; i++)
            {
                if (i != node_arr[y][x])
                {
                    ComputeNextCoords(curr_pos, next_pos, i);
                    if (AreCoordsInBounds(next_pos))
                    {
                        opp_dir = GetOppositeDir(i);
                        if (opp_dir == node_arr[next_pos.y][next_pos.x])
                        {
                            wall_arr[y][x] -= 1 << (i - 1);
                        }
                    }
                    else if (curr_pos.Equals(end))
                    {
                        RemoveEndPointWalls(end);
                    }
                }
            }
        }
    }
}

// Private method implementations

void MazeField::SetNewField(std::vector<std::vector<int>>& arr, int value_to_set,
    const uint32_t maze_size)
{
    for (uint32_t i = 0; i < arr.size(); i++)
    {
        arr[i].clear();
    }
    arr.clear();
    arr.resize(maze_size);
    InitArr(arr, value_to_set);
}

void MazeField::InitArr(std::vector<std::vector<int>>& arr, int value_to_set)
{
    for (int i = 0; i < arr.size(); i++)
    {
        arr[i] = std::vector<int>(arr.size(), value_to_set);
    }
}

void MazeField::InitRandGenObj(int start, int end)
{
    dis.param(std::uniform_int_distribution<int>::param_type(start, end));
}

void MazeField::SetRandEdgePos(int dir, Position& pos)
{
    std::mt19937 gen(rand());
    InitRandGenObj(1, node_arr.size());
    switch (dir)
    {
    case TOP:
        pos.x = dis(gen) - 1;
        pos.y = 0;
        break;
    case DOWN:
        pos.x = dis(gen) - 1;
        pos.y = node_arr.size() - 1;
        break;
    case LEFT:
        pos.x = 0;
        pos.y = dis(gen) - 1;
        break;
    case RIGHT:
        pos.x = node_arr.size() - 1;
        pos.y = dis(gen) - 1;
        break;
    default:
        break;
    }
}

void MazeField::SetStartEndPos()
{
    std::mt19937 gen(rand());
    InitRandGenObj(1, 4);

    int start_dir = dis(gen);
    int end_dir = start_dir % 2 != 0 ? start_dir + 1 : start_dir - 1;
    SetRandEdgePos(start_dir, start);
    SetRandEdgePos(end_dir, end);
}

void MazeField::ShuffleVisitOrder(int arr[], const int size)
{
    std::mt19937 seed(rand());
    std::shuffle(arr, arr + size, seed);
}

int MazeField::GetOppositeDir(int dir)
{
    int to_comp = (dir <= DOWN) ? DOWN : RIGHT;
    int res = (dir == to_comp) ? to_comp - 1 : to_comp;
    return res;
}

void MazeField::ComputeNextCoords(Position& curr_coords, Position& next_coords, int dir)
{
    int pos_change = 1;
    if (dir == TOP || dir == LEFT)
        pos_change = -1;

    next_coords.x = curr_coords.x;
    next_coords.y = curr_coords.y;
    if (dir == TOP || dir == DOWN) next_coords.y += pos_change;
    else if (dir == LEFT || dir == RIGHT) next_coords.x += pos_change;
}

bool MazeField::AreCoordsInBounds(Position& coords)
{
    return (coords.x >= 0 && coords.x < node_arr.size())
        && (coords.y >= 0 && coords.y < node_arr.size());
}

int MazeField::GetAdjacentCount(Position& coords)
{
    if (coords.y == 0 || coords.x == 0
        || coords.y == node_arr.size() - 1 || coords.x == node_arr.size() - 1)
    {
        return 2;
    }
    return 3;
}

void MazeField::VisitRandomNodes()
{
    uint32_t visit_count = 0;
    uint32_t total_cells = node_arr.size() * node_arr[0].size();
    int opp_dir;
    bool any_adj;
    bool end_reach = false;
    Position curr_pos, next_pos;
    curr_pos.x = start.x;
    curr_pos.y = start.y;
    if (start.y == 0)
    {
        node_arr[curr_pos.y][curr_pos.x] = TOP;
    }
    else if (start.y == node_arr.size() - 1)
    {
        node_arr[curr_pos.y][curr_pos.x] = DOWN;
    }
    else if (start.x == 0)
    {
        node_arr[curr_pos.y][curr_pos.x] = LEFT;
    }
    else if (start.x == node_arr.size() - 1)
    {
        node_arr[curr_pos.y][curr_pos.x] = RIGHT;
    }

    while (visit_count < total_cells)
    {
        if (!end_reach)
        {
            if (curr_pos.x == end.x && curr_pos.y == end.y)
            {
                end_reach = true;
            }
            path_to_end.push(curr_pos);
        }
        ShuffleVisitOrder(to_shuffle, 4);
        any_adj = false;
        for (int i = 0; i < 4; i++)
        {
            ComputeNextCoords(curr_pos, next_pos, to_shuffle[i]);
            if (AreCoordsInBounds(next_pos) &&
                node_arr[next_pos.y][next_pos.x] == 0)
            {
                any_adj = true;
                opp_dir = GetOppositeDir(to_shuffle[i]);
                curr_pos.x = next_pos.x;
                curr_pos.y = next_pos.y;
                node_arr[curr_pos.y][curr_pos.x] = opp_dir;
                visit_count++;
                break;
            }
        }
        // Return back if all neighbours are visited
        if (!any_adj)
        {
            if (curr_pos.y == start.y && curr_pos.x == start.x) break;
            int from_dir = node_arr[curr_pos.y][curr_pos.x];
            ComputeNextCoords(curr_pos, next_pos, from_dir);
            if (AreCoordsInBounds(next_pos))
            {
                curr_pos.x = next_pos.x;
                curr_pos.y = next_pos.y;
                if (!end_reach) path_to_end.pop();
            }
        }
    }
}

void MazeField::RemoveEndPointWalls(Position& end_point)
{
    int x = end_point.x;
    int y = end_point.y;
    if (y == 0)
    {
        if(wall_arr[y][x] >= 1 << (TOP - 1))
            wall_arr[y][x] -= 1 << (TOP - 1);
    }
    else if (y == GetSize() - 1)
    {
        if (wall_arr[y][x] >= 1 << (DOWN - 1))
            wall_arr[y][x] -= 1 << (DOWN - 1);
    }
    else if (x == 0)
    {
        if (wall_arr[y][x] >= 1 << (LEFT - 1))
            wall_arr[y][x] -= 1 << (LEFT - 1);
    }
    else if (x == GetSize() - 1)
    {
        if (wall_arr[y][x] >= 1 << (RIGHT - 1))
            wall_arr[y][x] -= 1 << (RIGHT - 1);
    }
}
