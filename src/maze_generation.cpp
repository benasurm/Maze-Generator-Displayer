#include <algorithm>
#include <random>
#include <time.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <stack>

#define TOP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4

typedef unsigned short usInt;

struct Position
{
    uint32_t x, y;
};

void InitPixelArr(std::vector<std::vector<usInt>> &node_arr)
{
    std::vector<usInt> temp_row(node_arr.size());
    std::fill(temp_row.begin(), temp_row.end(), 0);
    for(int i = 0; i < node_arr.size(); i++)
    {
        node_arr[i] = temp_row;
    }
}

void InitRandGenObj(std::uniform_int_distribution<> &dis, 
    int start, int end)
{
    dis.param(std::uniform_int_distribution<int>::param_type(start, end));
}

void GetRandEdgePos(Position &start, int dir, 
    std::uniform_int_distribution<> &dis, const uint32_t maze_size)
{
    std::mt19937 gen(rand());
    InitRandGenObj(dis, 1, maze_size);
    switch (dir)
    {
        case TOP:
            start.x = dis(gen) - 1;
            start.y = 0;
            break;
        case DOWN:
            start.x = dis(gen) - 1;
            start.y = maze_size - 1;
            break;
        case LEFT:
            start.x = 0;
            start.y = dis(gen) - 1;
            break;
        case RIGHT:
            start.x = maze_size - 1;
            start.y = dis(gen) - 1;
            break;
        default:
            break;
    }
}

void GetStartEndPos(Position &start, Position &end, 
    std::uniform_int_distribution<> &dis, const uint32_t maze_size)
{
    std::mt19937 gen(rand());
    InitRandGenObj(dis, 1, 4);

    int start_dir = dis(gen);
    int end_dir = start_dir % 2 != 0 ? start_dir+1 : start_dir-1;
    GetRandEdgePos(start, start_dir, dis, maze_size);
    GetRandEdgePos(end, end_dir, dis, maze_size);
}

void ShuffleVisitOrder(int arr[], const int size)
{
    std::mt19937 seed(rand());
    std::shuffle(arr, arr + size, seed);
}

int GetOppositeDir(int dir)
{
    int to_comp = (dir <= DOWN) ? DOWN : RIGHT;
    int res = (dir == to_comp) ? to_comp - 1 : to_comp;
    return res;
}

void ComputeNextCoords(Position curr_coords, Position &next_coords, 
    int dir)
{
    int pos_change = 1;
    if(dir == TOP || dir == LEFT)
        pos_change = -1;

    next_coords.x = curr_coords.x;
    next_coords.y = curr_coords.y;
    if(dir == TOP || dir == DOWN) next_coords.y += pos_change;
    else if(dir == LEFT || dir == RIGHT) next_coords.x += pos_change;
}

bool AreCoordsInBounds(Position &coords, uint32_t maze_size)
{
    return (coords.x >= 0 && coords.x < maze_size)
        && (coords.y >= 0 && coords.y < maze_size);
}

int GetAdjacentCount(Position &coords, const uint32_t maze_size)
{
    if(coords.y == 0 || coords.x == 0
        || coords.y == maze_size - 1 || coords.x == maze_size - 1)
    {
        return 2;
    }
    return 3;
}

void VisitRandomNodes(std::vector<std::vector<usInt>> &node_arr, 
    int to_shuffle[], const int shuffle_size, Position &start, Position &end,
    std::stack<Position> &path_to_end)
{
    uint32_t visit_count = 0;
    uint32_t total_cells = node_arr.size() * node_arr[0].size();
    int opp_dir;
    bool any_adj;
    bool end_reach = false;
    Position curr_pos, next_pos;
    curr_pos.x = start.x;
    curr_pos.y = start.y;
    //node_arr[curr_pos.y][curr_pos.x] = TOP;
    if(start.y == 0)
    {
        node_arr[curr_pos.y][curr_pos.x] = TOP;
    }
    else if(start.y == node_arr.size() - 1)
    {
        node_arr[curr_pos.y][curr_pos.x] = DOWN;
    }
    else if(start.x == 0)
    {
        node_arr[curr_pos.y][curr_pos.x] = LEFT;
    }
    else if(start.x == node_arr.size() - 1)
    {
        node_arr[curr_pos.y][curr_pos.x] = RIGHT;
    }

    while(visit_count < total_cells)
    {
        if(!end_reach)
        {
            if(curr_pos.x == end.x && curr_pos.y == end.y)
            {
                end_reach = true;
            }
            path_to_end.push(curr_pos);
        }
        ShuffleVisitOrder(to_shuffle, shuffle_size);
        any_adj = false;
        for(int i = 0; i < shuffle_size; i++)
        {
            ComputeNextCoords(curr_pos, next_pos, to_shuffle[i]);
            if(AreCoordsInBounds(next_pos, node_arr.size()) && 
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
        if(!any_adj)
        {
            if(curr_pos.y == start.y && curr_pos.x == start.x) break;
            int from_dir = node_arr[curr_pos.y][curr_pos.x];
            ComputeNextCoords(curr_pos, next_pos, from_dir);
            if(AreCoordsInBounds(next_pos, node_arr.size()))
            {
                curr_pos.x = next_pos.x;
                curr_pos.y = next_pos.y;
                if(!end_reach) path_to_end.pop();
            }
        }
    }
    if(visit_count == total_cells) std::cout << "should have visited all" << std::endl;
}

void PrintMazeToFile(std::vector<std::vector<usInt>> &node_arr)
{
    std::ofstream outFile("result.txt");
    for(uint32_t y = 0; y < node_arr.size(); y++)
    {
        for(uint32_t x = 0; x < node_arr[y].size(); x++)
        {
            outFile << node_arr[y][x];
        }
        outFile << std::endl;
    }
}

int main()
{
    const uint32_t maze_size = 20;
    std::vector<std::vector<usInt>> node_arr(maze_size);
    std::uniform_int_distribution<> dis;
    std::stack<Position> path_to_end;
    int to_shuffle[4] = { 1, 2, 3, 4 };
    const uint32_t shuffle_size = 4;
    Position start, end;
    srand(time(0));

    InitPixelArr(node_arr);
    GetStartEndPos(start, end, dis, maze_size);
    VisitRandomNodes(node_arr, to_shuffle, shuffle_size, start, end, path_to_end);
    PrintMazeToFile(node_arr);
}


