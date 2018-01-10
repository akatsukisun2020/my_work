#ifndef _GRID_H
#define _GRID_H

#include <vector>
#include <map>
#include <queue>
#include <string>
#include <fstream>
#include <unordered_map>

#include "util.h"

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
  int label; // for test
} Pulse;

typedef struct Cell{
  vector<int> id; //维度上的id
  int count;
  size_t cluid; //归并时要用到, 通过hash生成归并的唯一id
  int type;
  vector<Pulse> points; // 真实数据
} Cell;

typedef unordered_map<vector<int>, Cell, HashFunc, EqualKey> GRID_TYPE;
typedef unordered_map<int, vector<Cell*>> GRID_SHUFFLE_TYPE; //放置的是指针

bool operator==(const Pulse& p1, const Pulse& p2);

// 超参数配置
const int DIM = 3;
const int window_size = 500;
const int delta_k = 100; // 每次更新的窗口中的数据
const int dim_weights[3] = {2, 2, 2}; // 每个维度上的切分个数权值

void init_parameters();

class GRID{
  public:
    GRID();
    GRID(const string& name);
    ~GRID();

    // 最开始的初始化整个网格
    int init_grid();
    // 后面的更新若干个网格
    int update_grid();

    //void send_grid();
    GRID_TYPE get_gridinfo();
    void shuffle_grid(GRID_SHUFFLE_TYPE&);
    int get_cell_number()const;

    void add_record_to_grid(const Pulse& record);
    void remove_record_from_grid();

  private:
    string filename_;
    ifstream fin;

    GRID_TYPE grid_;
    queue<Pulse> window_;
};

#endif
