#include <algorithm>
#include <random>
#include <time.h>
#include <fstream>
#include <vector>

#define TOP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4

typedef unsigned uInt;
typedef unsigned short usInt;

struct Position
{
    uInt x, y;
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

void ShuffleVisitOrder(int arr[], const int size)
{
    srand(time(0));
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

bool AreCoordsInBounds(Position &coords, uInt arr_size)
{
    return (coords.x >= 0 && coords.x < arr_size)
        && (coords.y >= 0 && coords.y < arr_size);
}

int GetAdjacentCount(Position &coords, const uInt arr_size)
{
    if(coords.y == 0 || coords.x == 0
        || coords.y == arr_size - 1 || coords.x == arr_size - 1)
    {
        return 2;
    }
    return 3;
}

void VisitRandomNodes(std::vector<std::vector<usInt>> &node_arr, 
    int to_shuffle[], const int shuffle_size)
{
    uInt visit_count = 0;
    uInt total_cells = node_arr.size() * node_arr[0].size();
    int opp_dir;
    bool any_adj;
    Position curr_pos, next_pos;
    curr_pos.x = 0;
    curr_pos.y = 0;
    node_arr[curr_pos.y][curr_pos.x] = 1;

    while(visit_count < total_cells)
    {
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
            if(curr_pos.y == 0 && curr_pos.x == 0) break;
            int from_dir = node_arr[curr_pos.y][curr_pos.x];
            ComputeNextCoords(curr_pos, next_pos, from_dir);
            if(AreCoordsInBounds(next_pos, node_arr.size()))
            {
                curr_pos.x = next_pos.x;
                curr_pos.y = next_pos.y;
            }
        }
    }
}

void PrintMazeToFile(std::vector<std::vector<usInt>> &node_arr)
{
    std::ofstream outFile("result.txt");
    for(uInt y = 0; y < node_arr.size(); y++)
    {
        for(uInt x = 0; x < node_arr[y].size(); x++)
        {
            outFile << node_arr[y][x];
        }
        outFile << std::endl;
    }
}

int main()
{
    const uInt arr_size = 100;
    std::vector<std::vector<usInt>> node_arr(arr_size);
    int to_shuffle[4] = { 1, 2, 3, 4 };
    const uInt shuffle_size = 4;
    Position init_pos;
    init_pos.x = 0;
    init_pos.y = 0;

    InitPixelArr(node_arr);
    VisitRandomNodes(node_arr, to_shuffle, shuffle_size);
    PrintMazeToFile(node_arr);
}


