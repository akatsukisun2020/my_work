#ifndef _CLUSTER_H
#define _CLUSTER_H

#include "grid.h"

#include <unordered_set>

using std::unordered_set;

typedef unordered_set<vector<int>, HashFunc, EqualKey> NOISE_TYPE;

const int grid_number = 3;

// 超参数配置
extern int CELL_LEN[3]; //每个子网格的长度
extern int UPPERBOUND[3]; // 数值上界
extern int CELL_NUM[3];


class CLUSTER{
  public:
    CLUSTER();
    ~CLUSTER();

    void merge_grids(GRID_TYPE& grid);
    void do_cluster();
    void get_minPts();
    int density_atrribute(Cell& cell);
    void bft_grid(Cell& cell, int cluster_id);
    void deal_noise_cell_1();
    void deal_noise_cell_2();

    void print_result();
    void print_gridinfo();

  private:
    int minpts_;
    GRID_TYPE grid_;
    NOISE_TYPE noise_; //噪声集合
};

#endif
