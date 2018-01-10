#ifndef _MERGE_H
#define _MERGE_H

#include "cluster.h"


typedef vector<queue<GRID_TYPE>> GRID_WINDOW_TYPE;
typedef vector<queue<BOUNDARY_TYPE>> BOUNDARY_WINDOW_TYPE;

class MERGE{
  public:
    MERGE();
    MERGE(int);
    ~MERGE();

    void do_merge();
    void merge_grid(GRID_TYPE&);
    void merge_boundary(BOUNDARY_TYPE&);
    void bfs_boundary(map<size_t, size_t>&); //有序， 用map可能要好一些
    
    void add_to_window(size_t, GRID_TYPE&, BOUNDARY_TYPE&);

    void print_result();
    void print_gridinfo();

  private:
    int min_;
    GRID_WINDOW_TYPE grid_window_;
    BOUNDARY_WINDOW_TYPE boundary_window_;
    GRID_TYPE grid_;
    BOUNDARY_TYPE boundary_;
};


#endif
