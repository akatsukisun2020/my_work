#ifndef _CLUSTER_H
#define _CLUSTER_H

#include "grid.h"

#include <unordered_set>

using std::unordered_set;

const int grid_number = 3;

class CLUSTER{
  public:
    CLUSTER();
    ~CLUSTER();

    void merge_grids(unordered_map<vector<int>, Cell>& grid);
    void do_cluster();
    void get_minPts();
    int density_atrribute(Cell& cell);
    void bft_grid(Cell& cell, int cluster_id);
    void deal_noise_cell_1();
    void deal_noise_cell_2();

    void merge_A_B(vector<int>&, vector<int>&);

  private:
    int minpts_;
    unordered_map<vector<int>, Cell> grid_;
    unordered_set<vector<int>> noise_; //噪声集合
};

#endif
