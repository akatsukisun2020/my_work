#include "cluster.h"
#include "grid.h"
#include <iostream>
#include <string>

using namespace std;

int main(){
  const string file1 = "./data_dir/data1.txt";

  GRID grid1(file1);

  init_parameters();
  grid1.init_grid();
}
