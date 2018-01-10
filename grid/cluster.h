#ifndef _CLUSTER_H
#define _CLUSTER_H

#include "grid.h"

#include <unordered_set>

using std::unordered_set;

typedef unordered_set<vector<int>, HashFunc, EqualKey> NOISE_TYPE;
typedef unordered_set<vector<int>, HashFunc, EqualKey> BOUNDARY_TYPE;  // 用hash_set要好些， 因为可能有重复， 去重

const int grid_number = 3; //统计子单元的个数

// 超参数配置
extern int CELL_LEN[3]; //每个子网格的长度
extern int UPPERBOUND[3]; // 数值上界
extern int CELL_NUM[3];
extern int DIM_VALUE[3];

class CLUSTER{
  public:
    CLUSTER();
    CLUSTER(int);
    ~CLUSTER();

    void merge_grids(GRID_SHUFFLE_TYPE& grid);
    GRID_TYPE get_gridinfo();
    BOUNDARY_TYPE get_boundaryinfo();
    void reset_cell_number();
    void add_cell_number(int);
    void set_id(int);
    size_t get_id();

    void do_cluster();
    void get_minPts();
    int density_atrribute(Cell& cell);
    void bft_grid(Cell& cell, int cluster_id);
    void deal_noise_cell_1();
    void deal_noise_cell_2();
    void generate_boundary(const vector<int>&);

    void print_result();
    void print_gridinfo();

  private:
    size_t id_; //用于标识局部网格 ， 是通过generate_id函数的规则进行生成的
    int cell_number_;
    int minpts_;
    GRID_TYPE grid_;
    NOISE_TYPE noise_; //噪声集合
    BOUNDARY_TYPE boundary_; // 边界周密网格索引
};

#endif
