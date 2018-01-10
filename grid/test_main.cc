#include "merge.h"
#include "cluster.h"
#include "grid.h"
#include <iostream>
#include <string>

using namespace std;

int main(){
  const string file1 = "./data_dir/data1.txt";
  const string file2 = "./data_dir/data2.txt";
  const string file3 = "./data_dir/data3.txt";

  GRID grid1(file1), grid2(file2), grid3(file3);

  init_parameters();
  if(grid1.init_grid()==0 &&
    grid2.init_grid()==0 &&
    grid3.init_grid()==0){
    int size = grid1.get_cell_number() + grid2.get_cell_number() + grid3.get_cell_number();

    GRID_SHUFFLE_TYPE shuffle1, shuffle2, shuffle3;
    grid1.shuffle_grid(shuffle1);
    grid2.shuffle_grid(shuffle2);
    grid3.shuffle_grid(shuffle3);

    vector<CLUSTER> array(shuffle1.size());
    for(int i=0;i<array.size();++i){
      CLUSTER& cluster = array[i];
      cluster.set_id(i);
      cluster.merge_grids(shuffle1);
      cluster.merge_grids(shuffle2);
      cluster.merge_grids(shuffle3);
      cluster.reset_cell_number();
      cluster.add_cell_number(size);
      cluster.do_cluster();
    }

    MERGE merge(shuffle1.size());
    for(int i=0;i<array.size();++i){
      CLUSTER& cluster = array[i];
      GRID_TYPE grid = cluster.get_gridinfo();
      BOUNDARY_TYPE boundary = cluster.get_boundaryinfo();
      size_t id = cluster.get_id();
      merge.add_to_window(id, grid, boundary);
    }
    merge.do_merge();

    merge.print_result();
    merge.print_gridinfo();
  }


  // 循环读取数据 
  while(grid1.update_grid()==0 &&
    grid2.update_grid()==0 &&
    grid3.update_grid()==0){
    int size = grid1.get_cell_number() + grid2.get_cell_number() + grid3.get_cell_number();

    GRID_SHUFFLE_TYPE shuffle1, shuffle2, shuffle3;
    grid1.shuffle_grid(shuffle1);
    grid2.shuffle_grid(shuffle2);
    grid3.shuffle_grid(shuffle3);

    vector<CLUSTER> array(shuffle1.size());
    for(int i=0;i<array.size();++i){
      CLUSTER& cluster = array[i];
      cluster.set_id(i);
      cluster.merge_grids(shuffle1);
      cluster.merge_grids(shuffle2);
      cluster.merge_grids(shuffle3);
      cluster.reset_cell_number();
      cluster.add_cell_number(size);
      cluster.do_cluster();
    }

    MERGE merge(shuffle1.size());
    for(int i=0;i<array.size();++i){
      CLUSTER& cluster = array[i];
      GRID_TYPE grid = cluster.get_gridinfo();
      BOUNDARY_TYPE boundary = cluster.get_boundaryinfo();
      size_t id = cluster.get_id();
      merge.add_to_window(id, grid, boundary);
    }
    merge.do_merge();

    merge.print_result();
    merge.print_gridinfo();
  }
}
