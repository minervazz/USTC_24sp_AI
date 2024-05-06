#include<vector>
#include<iostream>
#include<queue>
#include<map>
#include<fstream>
#include<sstream>
#include<string>
#include<cmath>
#include<algorithm>

using namespace std;

struct Map_Cell
{
    int type; // 地图的类型：0空地、1障碍、2补给、3起点、4终点
};

struct Search_Cell
{
    int x, y; // 位置坐标
    int g; // 从起点到当前节点的实际代价
    int h; // 从当前节点到终点的估计代价（启发式）
    Search_Cell *parent; // 父节点

    Search_Cell(int x, int y, int g, int h, Search_Cell* parent = nullptr) : x(x), y(y), g(g), h(h), parent(parent) {}
};

// 自定义比较函数对象，按照 Search_Cell 结构体的 g + h 属性进行比较
struct CompareF {
    bool operator()(const Search_Cell *a, const Search_Cell *b) const {
        return (a->g + a->h) > (b->g + b->h); // 较小的 g + h 值优先级更高
    }
};

// 启发式函数，使用曼哈顿距离
int Heuristic_Function(int x, int y, int target_x, int target_y) {
    return abs(x - target_x) + abs(y - target_y);
}

void Astar_search(const string input_file, int &step_nums, string &way)
{
    ifstream file(input_file);
    if (!file.is_open()) {
        cout << "Error opening file!" << endl;
        return;
    }

    string line;
    getline(file, line); // 读取第一行
    stringstream ss(line);
    int M, N, T;
    ss >> M >> N >> T;

    pair<int, int> start_point; // 起点
    pair<int, int> end_point;   // 终点
    Map_Cell **Map = new Map_Cell *[M];
    // 加载地图
    for(int i = 0; i < M; i++)
    {
        Map[i] = new Map_Cell[N];
        getline(file, line);
        stringstream ss(line);
        int cell_type;
        for(int j = 0; j < N; j++)
        {
            ss >> cell_type;
            Map[i][j].type = cell_type;
            if(cell_type == 3)
            {
                start_point = {i, j};
            }
            else if(cell_type == 4)
            {
                end_point = {i, j};
            }
        }
    }

    // A*搜索初始化
    priority_queue<Search_Cell*, vector<Search_Cell*>, CompareF> open_list;
    map<pair<int, int>, Search_Cell*> all_cells;

    Search_Cell *start_cell = new Search_Cell(start_point.first, start_point.second, 0, Heuristic_Function(start_point.first, start_point.second, end_point.first, end_point.second));
    open_list.push(start_cell);
    all_cells[{start_point.first, start_point.second}] = start_cell;

    bool found = false;
    Search_Cell* current;
    while(!open_list.empty() && !found)
    {
        current = open_list.top();
        open_list.pop();

        // 到达终点
        if (current->x == end_point.first && current->y == end_point.second) {
            found = true;
            break;
        }

        // 扩展当前节点
        const vector<pair<int, int>> directions{{0, 1}, {1, 0}, {0, -1}, {-1, 0}}; // 四个可能的移动方向
        for (const auto& dir : directions) {
            int new_x = current->x + dir.first;
            int new_y = current->y + dir.second;

            if (new_x >= 0 && new_x < M && new_y >= 0 && new_y < N && Map[new_x][new_y].type != 1) { // 确保在范围内且不是障碍
                int new_g = current->g + 1;
                int new_h = Heuristic_Function(new_x, new_y, end_point.first, end_point.second);
                auto new_cell = new Search_Cell(new_x, new_y, new_g, new_h, current);

                if (all_cells.find({new_x, new_y}) == all_cells.end() || new_g < all_cells[{new_x, new_y}]->g) { // 发现更短的路径
                    all_cells[{new_x, new_y}] = new_cell;
                    open_list.push(new_cell);
                }
            }
        }
    }

    // 构建路径
    if (found) {
        vector<char> path;
        while (current != nullptr && current->parent != nullptr) {
            if (current->x == current->parent->x + 1) path.push_back('D');
            else if (current->x == current->parent->x - 1) path.push_back('U');
            else if (current->y == current->parent->y + 1) path.push_back('R');
            else if (current->y == current->parent->y - 1) path.push_back('L');
            current = current->parent;
        }
        reverse(path.begin(), path.end());
        step_nums = path.size();
        way = string(path.begin(), path.end());
    } else {
        step_nums = -1;
        way = "";
    }

    // 释放动态内存
    for (auto& cell : all_cells) {
        delete cell.second;
    }
    for(int i = 0; i < M; i++) {
        delete[] Map[i];
    }
    delete[] Map;
}

void output(const string output_file, int &step_nums, string &way)
{
    ofstream file(output_file);
    if(file.is_open())
    {
        file << step_nums << endl;
        if(step_nums >= 0)
        {
            file << way << endl;
        }

        file.close();
    }
    else
    {
        cerr << "Can not open file: " << output_file << endl;
    }
}

int main(int argc, char *argv[])
{
    string input_base = "../input/input_";
    string output_base = "../output/output_";
    // input_0为讲义样例，此处不做测试
    for(int i = 1; i < 11; i++)
    {
        int step_nums = -1;
        string way = "";
        Astar_search(input_base + to_string(i) + ".txt", step_nums, way);
        output(output_base + to_string(i) + ".txt", step_nums, way);
    }
    return 0;
}
