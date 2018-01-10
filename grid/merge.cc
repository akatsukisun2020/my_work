#include "merge.h"
#include <iostream>

using namespace std;

MERGE::MERGE():min_(0){

}

MERGE::MERGE(int number):
  min_(0), grid_window_(number), boundary_window_(number) //初始化个数
{

}

MERGE::~MERGE(){

}

void MERGE::add_to_window(size_t id, GRID_TYPE& grid, BOUNDARY_TYPE& boundary){
  queue<GRID_TYPE>& win1 = grid_window_[id];
  queue<BOUNDARY_TYPE>& win2 = boundary_window_[id];

  win1.push(grid);
  win2.push(boundary);

  int min = grid_window_[0].size();
  for(int i=1;i<grid_window_.size();++i){ // 更新 min_
    int size = grid_window_.size();
    if(size < min)
      min = size;
  }
  min_ = min;
}

void MERGE::merge_grid(GRID_TYPE& grid_info){
  for(auto it=grid_info.begin(); it!=grid_info.end(); ++it){
    grid_.insert(make_pair(it->first, it->second)); //不可能重复， 这
  }
}

void MERGE::merge_boundary(BOUNDARY_TYPE& boundary_info){
  for(auto it=boundary_info.begin(); it!=boundary_info.end(); ++it){
    boundary_.insert(*it);
  }
}

//处理之后 -- key > value
//而且key不重复， 因为重复的都能够直接合并掉 
void MERGE::bfs_boundary(map<size_t, size_t>& associated_cell_map){
  for(auto it=boundary_.begin(); it!=boundary_.end(); ++it){
    const int delta[6][3] = {{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0 ,0, -1}};
    for(int i=0; i<6; ++i){
      vector<int> index(3);
      int flag = 0;
      for(int j=0; j<3; ++j){
        index[j] = (*it)[j] + delta[i][j];
        if(index[j]<0 || index[j]>=CELL_NUM[j]) //越界
        {
          flag = 1;
          break;
        }
      }
      if(flag == 1) continue;
      auto iter = boundary_.find(index);
      if(iter == boundary_.end())
        continue;

      size_t value1 = grid_[(*it)].cluid, value2 = grid_[(*iter)].cluid;
      size_t key = max(value1, value2), value = min(value1, value2);

      auto tmp = associated_cell_map.find(key);
      if(tmp==associated_cell_map.end() || 
          tmp!=associated_cell_map.end() && associated_cell_map[key]>value){
        associated_cell_map[key] = value; //改变的条件， 要么不存在， 要么存在，但是太大了  
      }
    }
  }
}

void deal_map(map<size_t, size_t>& associated_cell_map){
  if(associated_cell_map.empty())
    return;

  auto it = associated_cell_map.begin();
  ++it;
  for(; it!=associated_cell_map.end(); ++it){
    if(it->first > it->second){
      cout<<"error : key should be greater than value "<<endl;
      continue;
    }
    size_t value = it->second;
    auto iter = associated_cell_map.find(value);
    if(iter != associated_cell_map.end() && iter->first>iter->second){
      it->second = iter->second;
    }
  }
}

void MERGE::do_merge(){
  //cout<<"in do_merge () begin : min_ -- "<<min_<<endl;
  if(min_ < 1) // 每种list中的个数的最小值
    return;

  grid_.clear();
  boundary_.clear();

  // merge data
  for(int i=0;i<grid_window_.size();++i){
    GRID_TYPE& grid_info = grid_window_[i].front();
    BOUNDARY_TYPE& boundary_info = boundary_window_[i].front();
    merge_grid(grid_info);
    merge_boundary(boundary_info);
    grid_window_[i].pop();
    boundary_window_[i].pop();
    --min_; //窗口最小number控制语义
  }

  map<size_t, size_t> associated_cell_map;
  bfs_boundary(associated_cell_map);
  deal_map(associated_cell_map);

  for(auto it=grid_.begin(); it!=grid_.end(); ++it){
    size_t cluid = (it->second).cluid;
    if(associated_cell_map.find(cluid) != associated_cell_map.end()){
      (it->second).cluid = associated_cell_map[cluid]; //更新cluid 
    }
  }
  //cout<<"in do_merge () end "<<endl;
}


void MERGE::print_result(){
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

  cout<<"total_number : "<<total_number<<"  grid size : "<<grid_.size()<<endl;
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

void MERGE::print_gridinfo(){
  //cout<<"minpts ----------- "<<minpts_<<endl;
  for(auto it=grid_.begin();it != grid_.end();++it){
    Cell& cell = it->second;
    string index = to_string((it->first)[0]) +" "+
      to_string((it->first)[1]) +" "+ to_string((it->first)[2]);
    cout<<" grid_index : "<<index<<"     count : "<<(it->second).count<<
      "     cluid : "<<to_string((it->second).cluid)<<endl;
  }
}
