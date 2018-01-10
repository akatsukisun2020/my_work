#include "grid.h"
#include <iostream>
#include <sstream>
#include <cstdio>
#include <cmath>
#include <algorithm>

using namespace std;

// 超参数配置
int CELL_LEN[3] = {2, 2, 2}; //每个子网格的长度
int UPPERBOUND[3] = {180, 100, 100}; // 数值上界
int CELL_NUM[3];
int DIM_VALUE[3];

void init_parameters(){
  for(int i=0;i<3;++i){
    CELL_NUM[i] = ceil(UPPERBOUND[i] / CELL_LEN[i]);
    UPPERBOUND[i] = CELL_NUM[i] * CELL_LEN[i];
    DIM_VALUE[i] = floor(UPPERBOUND[i] / dim_weights[i]); // 上层网络分隔
  }
}

// 数据预处理函数， 主要是对雷达信号数据做一些类似规范化的操作
void pre_proccess(Pulse& pulse){
  pulse.PW *= 10;//脉宽精度 0.1
  pulse.RF *= 10;
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

    istringstream istr;
    string str;
    getline(fin, str);
    istr.str(str);
    istr>>pulse.time>>pulse.AOA>>pulse.RF>>pulse.PW>>pulse.label;

//    cout<<"pulse : "<<pulse.time<<"  "<<pulse.AOA<<"  "<<pulse.RF<<"  "<<
//      pulse.PW<<"  "<<pulse.label<<endl;

    if(fin.eof()) // 文件末尾
      return -1;

    pre_proccess(pulse);
    add_record_to_grid(pulse);
  }
  return 0;
  //send_grid();  //在真实的网络环境中， 可能要用到send进行发送!
}

int GRID::update_grid(){
  for(int i=0;i<delta_k;++i){
    Pulse pulse;
    istringstream istr;
    string str;
    getline(fin, str);
    istr.str(str);
    istr>>pulse.time>>pulse.AOA>>pulse.RF>>pulse.PW>>pulse.label;

    if(fin.eof()) // 文件末尾
      return -1;

    pre_proccess(pulse);
    add_record_to_grid(pulse);
  }
  return 0;
}

GRID_TYPE GRID::get_gridinfo(){
  return grid_;
}

// 产生顺序编号的id形式 
int generate_id(vector<int>& key){
  int result = 0;
  int value = 1;
  for(int i=0;i<key.size();++i){
    result += key[i]*value; //注意先后顺序
    value *= dim_weights[i];
  }
  return result;
}

//作用是将所有的cell进行按照切分的网格进行归类
void GRID::shuffle_grid(GRID_SHUFFLE_TYPE& shuffle_map){
  for(auto it=grid_.begin();it!=grid_.end();++it){
    vector<int> index(DIM);
    const vector<int>& key = it->first;
    for(int i=0;i<DIM;++i){
      index[i] = ceil(key[i]/DIM_VALUE[i]);
    }
    //size_t hash_key = HashFunc()(index); // 这里的hash_key 表示的唯一的局部网格代号
    int hash_key = generate_id(index); // 这里的hash_key 表示的唯一的局部网格代号
    auto iter = shuffle_map.find(hash_key);
    if(iter == shuffle_map.end()){
      shuffle_map.insert(make_pair(hash_key, vector<Cell*>()));
    }
    Cell* cell_ptr = &(it->second);
    shuffle_map[hash_key].push_back(cell_ptr);

    // 这个iter是null， 所以， 会导致coredump
    //(iter->second).push_back(cell_ptr);  
  }
}

int GRID::get_cell_number()const{
  return grid_.size();
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
