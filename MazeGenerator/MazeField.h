#pragma once

#include <vector>
#include <stack>
#include <algorithm>
#include <random>
#include <time.h>

#define TOP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4
#define PATH_CELL 5

struct Position
{
    uint32_t x, y;

    bool Equals(Position& to_comp)
    {
        return x == to_comp.x && y == to_comp.y;
    }
};

class MazeField
{
public:
    // Public members
    std::vector<std::vector<int>> node_arr;
    std::vector<std::vector<int>> wall_arr;
    std::stack<Position> path_to_end;

    // Public methods
    MazeField(const uint32_t maze_size);

    uint32_t GetSize();
    int GetPos(Position& to_get);
    void SetNewMaze(const uint32_t maze_size);
    void GenerateRandMaze();
    void ComputeObjsToDraw();
private:
    // Private members
    int to_shuffle[4] = { TOP, DOWN, LEFT, RIGHT };
    std::uniform_int_distribution<> dis;
    Position start, end;

    // Private members
    void SetNewField(std::vector<std::vector<int>>& arr, int value_to_set,
        const uint32_t maze_size);
    void InitArr(std::vector<std::vector<int>>& arr, int value_to_set);
    void InitRandGenObj(int start, int end);
    void SetRandEdgePos(int dir, Position& pos);
    void SetStartEndPos();
    void ShuffleVisitOrder(int arr[], const int size);
    int GetOppositeDir(int dir);
    void ComputeNextCoords(Position& curr_coords, Position& next_coords, int dir);
    bool AreCoordsInBounds(Position& coords);
    int GetAdjacentCount(Position& coords);
    void VisitRandomNodes();
    void RemoveEndPointWalls(Position& end_point);
    void ComputeWallsToDraw();
    void ComputeCellsToMark();
};
