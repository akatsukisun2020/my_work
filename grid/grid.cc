#include "grid.h"
#include <iostream>
#include <cstdio>
#include <cmath>
#include <algorithm>

using namespace std;

// 超参数配置
int CELL_LEN[3] = {4, 4, 4}; //每个子网格的长度
int UPPERBOUND[3] = {200, 200, 200}; // 数值上界
int CELL_NUM[3];

void init_parameters(){
  for(int i=0;i<3;++i){
    CELL_NUM[i] = ceil(UPPERBOUND[i] / CELL_LEN[i]);
    UPPERBOUND[i] = CELL_NUM[i] * CELL_LEN[i];
  }
}

bool operator==(const Pulse& p1, const Pulse& p2){
  return p1.time==p2.time && p1.AOA==p2.AOA && p1.RF==p2.RF && p1.PW==p2.PW && p1.label==p2.label;
}

GRID::GRID(){

}

// 构造函数中进行流的打开并初始化
// 析构函数中关闭
GRID::GRID(const string& name):filename_(name){
  fin.open(filename_.c_str());
}

GRID::~GRID(){
  fin.close();
}

//TODO 这里要注意的是， 每次读完之后， 要更新offset。
int GRID::init_grid(){
  for(int i=0;i<window_size;++i){
    Pulse pulse;
    int tmp;
    fin>>pulse.time>>pulse.AOA>>pulse.RF>>pulse.PW>>pulse.label;
    fin>>tmp; //'\n'

    if(fin.eof()) // 文件末尾
      return -1;

    add_record_to_grid(pulse);
  }
  return 0;
  //send_grid();  //在真实的网络环境中， 可能要用到send进行发送!
}

int GRID::update_grid(){
  for(int i=0;i<delta_k;++i){
    Pulse pulse;
    int tmp;
    fin>>pulse.time>>pulse.AOA>>pulse.RF>>pulse.PW;
    fin>>tmp; //'\n'

    if(fin.eof()) // 文件末尾
      return -1;

    add_record_to_grid(pulse);
  }
  return 0;
}

 GRID_TYPE GRID::get_gridinfo(){
  return grid_;
}

void GRID::add_record_to_grid(const Pulse& record){
  if(window_.size()>window_size)
    remove_record_from_grid();// 删除一个元素
  window_.push(record);

  vector<int> index(3);
  index[0] = floor(record.AOA / CELL_LEN[0]);
  index[1] = floor(record.RF / CELL_LEN[1]);
  index[2] = floor(record.PW / CELL_LEN[2]);

  auto iter = grid_.find(index);
  if(iter != grid_.end()){
    (iter->second).count++;
    (iter->second).points.push_back(record);
  }
  else{
    Cell cell;
    cell.id = index;
    cell.count = 1;
    cell.cluid = 0;
    cell.points.push_back(record);
    grid_.insert(make_pair(index, cell));
  }
}

void GRID::remove_record_from_grid(){
  if(window_.empty())
      return;

  Pulse record = window_.front();
  window_.pop();
  vector<int> index(3);
  index[0] = floor(record.AOA / CELL_LEN[0]);
  index[1] = floor(record.RF / CELL_LEN[1]);
  index[2] = floor(record.PW / CELL_LEN[2]);

  auto iter = grid_.find(index);
  if(iter != grid_.end()){
    (iter->second).count--;
    if((iter->second).count == 0)
      grid_.erase(iter);
    else{
      auto tmp = find((iter->second).points.begin(), (iter->second).points.end(), record);
      if(tmp != (iter->second).points.end())
        (iter->second).points.erase(tmp);
    }
  }
  else{
    cout<<"GRID::remove_record_from_grid error"<<endl;
  }
}
