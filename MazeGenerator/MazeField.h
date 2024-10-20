#pragma once

#include <vector>
#include <stack>
#include <algorithm>
#include <random>
#include <time.h>
#include <wx/wx.h>

#define TOP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4

class MazeField
{
    public:
        // Public members
        std::vector<std::vector<int>> node_arr;
        std::vector<std::vector<int>> wall_arr;
        std::stack<wxPoint*> path_to_end;

        // Public methods
        MazeField(const uint32_t maze_size);

        uint32_t GetSize();
        int GetPos(wxPoint* to_get);
        void SetNewMaze(const uint32_t maze_size);
        void GenerateRandMaze();
        void ComputeWallsToDraw();
    private:
        // Private members
        int to_shuffle[4] = { TOP, DOWN, LEFT, RIGHT };
        std::uniform_int_distribution<> dis;
        wxPoint* start;
        wxPoint* end;

        // Private members
        void SetNewField(std::vector<std::vector<int>>& arr, int value_to_set,
            const uint32_t maze_size);
        void InitArr(std::vector<std::vector<int>>& arr, int value_to_set);
        void InitRandGenObj(int start, int end);
        void SetRandEdgePos(int dir, wxPoint* pos);
        void SetStartEndPos();
        void ShuffleVisitOrder(int arr[], const int size);
        int GetOppositeDir(int dir);
        void ComputeNextCoords(wxPoint* curr_coords, wxPoint* next_coords, int dir);
        bool AreCoordsInBounds(wxPoint* coords);
        int GetAdjacentCount(wxPoint* coords);
        void VisitRandomNodes();
};

