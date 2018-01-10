#include "cluster.h"
#include <iostream>
using namespace std;

CLUSTER::CLUSTER(){

}

CLUSTER::CLUSTER(int id):id_(id){

}

CLUSTER::~CLUSTER(){

}

void merge_vector(vector<Pulse>& array1, vector<Pulse>& array2){
  if(array2.empty())
    return;
  for(int i=0;i<array2.size();++i){
    array1.push_back(array2[i]);
  }
}

void CLUSTER::set_id(int id){
  id_ = id;
}

size_t CLUSTER::get_id(){
  return id_;
}

void CLUSTER::reset_cell_number(){
  cell_number_ = 0;
}

void CLUSTER::add_cell_number(int number){
  cell_number_ += number;
}

//主要是合并计数count， 与各个点points
void CLUSTER::merge_grids(GRID_SHUFFLE_TYPE& shuffle_map){
  auto it = shuffle_map.find(id_);  //是不是给这个单元的
  if(it == shuffle_map.end())
    return;

  vector<Cell*>& array = it->second;
  for(int i=0;i<array.size();++i){
    Cell* cell_ptr = array[i];

    auto iter = grid_.find(cell_ptr->id);
    if(iter == grid_.end()){
      grid_.insert(make_pair(cell_ptr->id, *cell_ptr));
    }
    else{
      (iter->second).count += cell_ptr->count;
      merge_vector((iter->second).points, cell_ptr->points);
    }
  }

 // if(grid.empty())
 //   return;

 // for(auto it=grid.begin();it!=grid.end();++it){
 //   auto iter = grid_.find(it->first);
 //   if(iter == grid_.end()){
 //     grid_.insert(make_pair(it->first, it->second));
 //   }
 //   else{
 //     (iter->second).count += (it->second).count;
 //     merge_vector((iter->second).points, (it->second).points);
 //   }
 // }
}

GRID_TYPE CLUSTER::get_gridinfo(){
  for(auto it=grid_.begin(); it!=grid_.end(); ++it){  //重新构造id， 使得merge的时候， id唯一
    string cluid = to_string(id_) + "_" + to_string((it->second).cluid);
    (it->second).cluid = hash<string>()(cluid);
  }
  return grid_;
}

BOUNDARY_TYPE CLUSTER::get_boundaryinfo(){
  return boundary_;
}

//产生所有边界网格的集合
void CLUSTER::generate_boundary(const vector<int>& index){
  for(int i=0;i<DIM;++i){
    if(dim_weights[i]!=1 && index[i]!=0 && index[i]%DIM_VALUE[i]==0){  // 边界单元格-条件1
      boundary_.insert(index);
    }
    else if(dim_weights[i]!=1 && index[i]+1<UPPERBOUND[i] && (index[i]+1)%DIM_VALUE[i]==0){ // 边界单元格-条件2
      boundary_.insert(index);
    }
  }
}

// 遍历的过程的第一步只是处理响应的稠密的网格cell
// 并将所有的稀疏网格与噪声网格加入到noise中
void CLUSTER::bft_grid(Cell& cell, int cluster_id){
  if(density_atrribute(cell) != 0){ //不为稠密网络
    noise_.insert(cell.id); 
    return;
  }

  if(cell.cluid != 0) //说明已经被遍历过了的
    return;

  generate_boundary(cell.id); //对稠密网格进行判定

  cell.cluid = cluster_id;

  const int delta[6][3] = {{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0 ,0, -1}};
  for(int i=0; i<6; ++i){
    vector<int> index(3);
    int flag = 0;
    for(int j=0; j<3; ++j){
      index[j] = (cell.id)[j] + delta[i][j];
      if(index[j]<0 || index[j]>=CELL_NUM[j]) //越界
      {
        flag = 1;
        break;
      }
    }
    if(flag == 1) continue;
    
    auto it = grid_.find(index);
    if(it == grid_.end())
      continue;
    bft_grid(it->second, cluster_id); //貌似是深度优先遍历
  }
}


// 这是处理稀疏网格的第一步， 处理所有与稠密网格相连的稀疏网格
// 第二步是处理周围没有稠密网格的稀疏网格，2个不能连在一起，
// 因为有可能它周围的稀疏网格还没有被归类.
void CLUSTER::deal_noise_cell_1(){
  if(noise_.empty())
    return;

  for(auto it=noise_.begin(); it!=noise_.end();){
    auto tmp = grid_.find(*it);
    if(tmp == grid_.end())
      continue;
    Cell& cell = tmp->second;

    const int delta[6][3] = {{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0 ,0, -1}};
    vector<int> max_index;
    int max_count = 0;
    int max_cluid = 0;

    for(int i=0; i<6; ++i){
      vector<int> index(3);
      int flag = 0;
      for(int j=0; j<3; ++j){
        index[j] = (cell.id)[j] + delta[i][j];
        if(index[j]<0 || index[j]>=CELL_NUM[j]) //越界
        {
          flag = 1;
          break;
        }
      }
      if(flag == 1) continue;
      
      auto iter = grid_.find(index);
      if(iter == grid_.end())
        continue;
      if(density_atrribute(iter->second) == 0){
        if((iter->second).count > max_count){
          max_index = (iter->second).id;
          max_count = (iter->second).count;
          max_cluid = (iter->second).cluid;
        }
      }
    }
    if(!max_index.empty() && max_count!=0 && max_cluid!=0){ 
      cell.cluid = max_cluid;
      //TODO
      //这里可能还要涉及到删除这些网格， 已经处理过得网格
      auto erase_it = it;
      ++it;
      noise_.erase(erase_it);
    }
    else
      ++it;
  }
}

// 处理周围没有稠密网格的稀疏网格
void CLUSTER::deal_noise_cell_2(){
  if(noise_.empty())
    return;

  for(auto it=noise_.begin(); it!=noise_.end();){
    auto tmp = grid_.find(*it);
    if(tmp == grid_.end())
      continue;
    Cell& cell = tmp->second;

    const int delta[6][3] = {{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0 ,0, -1}};
    unordered_map<int, int> cluid_count;  //统计周围每一种簇的网格的个数 <cluster_id, count>

    for(int i=0; i<6; ++i){
      vector<int> index(3);
      int flag = 0;
      for(int j=0; j<3; ++j){
        index[j] = (cell.id)[j] + delta[i][j];
        if(index[j]<0 || index[j]>=CELL_NUM[j]) //越界
        {
          flag = 1;
          break;
        }
      }
      if(flag == 1) continue;
      
      auto iter = grid_.find(index);
      if(iter == grid_.end())
        continue;
      if(density_atrribute(iter->second) == 0){
        cout<<"error: belong to deal_noise_1"<<endl;
      }

      int key_cluid = cell.cluid;
      auto iter1 = cluid_count.find(key_cluid);
      if(iter1 == cluid_count.end() && key_cluid != 0){ //这里要排除周围
        cluid_count[key_cluid] = 1;
      }
      else{
        cluid_count[key_cluid]++;
      }
    }

    bool is_normal = false;
    int next_cluid = 0;
    for(auto it_count=cluid_count.begin(); it_count!=cluid_count.end(); ++it_count){ // 超过一半相同的id  
      if((it_count->second) > DIM && (it_count->first) != 0){
        is_normal = true;
        next_cluid = it_count->first;
        break;
      }
    }
    if(is_normal){ 
      cell.cluid = next_cluid;
      auto erase_it = it;
      ++it;
      noise_.erase(erase_it);
    }
    else
      ++it;
  }
}

void CLUSTER::do_cluster(){
  get_minPts();//先确定参数
  int cluster_id = 0;
  // 深度遍历，将所有的稠密网格进行聚类
  for(auto it=grid_.begin();it!=grid_.end();++it){
    cluster_id++;
    bft_grid(it->second, cluster_id);
  }

  deal_noise_cell_1();
  deal_noise_cell_2();
}

void CLUSTER::get_minPts(){
  int N = window_size * grid_number;
  minpts_ = N/cell_number_;

//  if(grid_.empty()) 
//    return;
//  int cell_number = grid_.size();
//  int N = 0, max_cell = 0;
//
//  for(auto it=grid_.begin();it!=grid_.end();++it){
//    N += (it->second).count;
//    if((it->second).count>max_cell)
//      max_cell = (it->second).count;
//  }
//
//  //minpts_ = (N/cell_number + max_cell)/2;
//  minpts_ = N/cell_number;
//  //minpts_ = 50;
}

//判定一个cell是不是稠密的或者稀疏的
//稠密： 返回0
//稀疏： 返回1
//噪声： 返回2
int CLUSTER::density_atrribute(Cell& cell){
  if(cell.count == 1)
    return 2;
  else if(cell.count < minpts_)
    return 1;
  else
    return 0;
}


void CLUSTER::print_result(){
  int total_number = window_size * grid_number;
  // 统计<cluid+label, count>
  // <cluid, count>
  unordered_map<string, int> result;
  for(auto it=grid_.begin();it != grid_.end();++it){
    Cell& cell = it->second;
//    if(cell.cluid <= 0)
//      continue;

    string id = to_string(cell.cluid);
    vector<Pulse>& points = cell.points;
    for(int i=0;i<points.size();++i){
      int label = points[i].label;
      string id_label = id + "_" + to_string(label);

      // 统计改聚类的所有数目
      auto it_id = result.find(id);
      if(it_id == result.end()){
        result.insert(make_pair(id, 1));
      }
      else{
        ++(it_id->second);
      }

      // 统计一种聚类中， 每种label的所有数目
      auto it_id_label = result.find(id_label);
      if(it_id_label == result.end()){
        result.insert(make_pair(id_label, 1));
      }
      else{
        ++(it_id_label->second);
      }
    }
  }

  // 百分比统计
  unordered_map<string, int> last_result;
  for(auto iter=result.begin();iter!=result.end();++iter){
    string key = iter->first;
    string::size_type idx = key.find("_");
    if(idx == string::npos)
      continue;

    string tmp = key.substr(0, idx);
    auto it_tmp = last_result.find(tmp);
    if(it_tmp == last_result.end()){
      last_result.insert(make_pair(tmp, iter->second));
    }
    else{
      if(it_tmp->second < iter->second)
        it_tmp->second = iter->second;
    }
  }

  cout<<"total_number : "<<total_number<<endl;
  cout<<endl;
  for(auto iter=result.begin();iter!=result.end();++iter){
    cout<<"result -- "<< iter->first<<" : "<<iter->second<<endl;
  }
  cout<<endl;
  for(auto iter=last_result.begin();iter!=last_result.end();++iter){
    string key = iter->first;
    float num1 = float(iter->second);
    float num2 = float(result[key]);
    cout<<"precent -- "<<key<<" : "<<num1/num2<<endl;
  }
  cout<<endl;
}

void CLUSTER::print_gridinfo(){
  cout<<"minpts ----------- "<<minpts_<<endl;
  for(auto it=grid_.begin();it != grid_.end();++it){
    Cell& cell = it->second;
    string index = to_string((it->first)[0]) +" "+
      to_string((it->first)[1]) +" "+ to_string((it->first)[2]);
    cout<<" grid_index : "<<index<<"     count : "<<(it->second).count<<
      "     cluid : "<<to_string((it->second).cluid)<<endl;
  }
}
