#ifndef _CLUSTER_H
#define _CLUSTER_H

#include "grid.h"

#include <unordered_set>

using std::unordered_set;

typedef unordered_set<vector<int>, HashFunc, EqualKey> NOISE_TYPE;

const int grid_number = 3;

class CLUSTER{
  public:
    CLUSTER();
    ~CLUSTER();

    void merge_grids(GRID_TYPE, Cell>& grid);
    void do_cluster();
    void get_minPts();
    int density_atrribute(Cell& cell);
    void bft_grid(Cell& cell, int cluster_id);
    void deal_noise_cell_1();
    void deal_noise_cell_2();

    void print_result();

  private:
    int minpts_;
    GRID_TYPE grid_;
    NOISE_TYPE noise_; //噪声集合
};

#endif
