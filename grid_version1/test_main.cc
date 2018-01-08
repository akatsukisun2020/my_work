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
    GRID_TYPE grid_info1 = grid1.get_gridinfo();
    GRID_TYPE grid_info2 = grid2.get_gridinfo();
    GRID_TYPE grid_info3 = grid3.get_gridinfo();

    CLUSTER cluster;
    cluster.merge_grids(grid_info1);
    cluster.merge_grids(grid_info2);
    cluster.merge_grids(grid_info3);
    cluster.do_cluster();
    cluster.print_result();
    cluster.print_gridinfo();
  }

  while(grid1.update_grid()==0 &&
    grid2.update_grid()==0 &&
    grid3.update_grid()==0){
    GRID_TYPE grid_info1 = grid1.get_gridinfo();
    GRID_TYPE grid_info2 = grid2.get_gridinfo();
    GRID_TYPE grid_info3 = grid3.get_gridinfo();

    CLUSTER cluster;
    cluster.merge_grids(grid_info1);
    cluster.merge_grids(grid_info2);
    cluster.merge_grids(grid_info3);
    cluster.do_cluster();
    cluster.print_result();
    cluster.print_gridinfo();
  }
}
