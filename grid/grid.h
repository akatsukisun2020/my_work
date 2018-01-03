#ifndef _GRID_H
#define _GRID_H

#include <vector>
#include <map>
#include <queue>
#include <string>
#include <fstream>
#include <unordered_map>

using std::vector;
using std::map;
using std::queue;
using std::string;
using std::ifstream;
using std::unordered_map;

typedef struct Pulse{
  float time;
  float AOA;
  float RF;
  float PW;
} Pulse;

typedef struct Cell{
  vector<int> id; //维度上的id
  int count;
  int cluid;
  int type;
  vector<Pulse> points; // 真实数据
} Cell;


// 超参数配置
const int DIM = 3;
const int window_size = 100;
const int delta_k = 2; // 每次更新的窗口中的数据
int CELL_LEN[3] = {4, 4, 4}; //每个子网格的长度
int UPPERBOUND[3] = {200, 400, 400}; // 数值上界
int CELL_NUM[3];

void init_parameters();

class GRID{
  public:
    GRID();
    GRID(const string& name);
    ~GRID();

    // 最开始的初始化整个网格
    void init_grid();
    // 后面的更新若干个网格
    void update_grid();

    //void send_grid();
    unordered_map<vector<int>, Cell> get_gridinfo();

    void add_record_to_grid(const Pulse& record);
    void remove_record_from_grid();

  private:
    string filename_;
    ifstream fin;

    unordered_map<vector<int>, Cell> grid_;
    queue<Pulse> window_;
};

#endif
