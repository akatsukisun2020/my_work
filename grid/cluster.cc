#include "cluster.h"

CLUSTER::CLUSTER(){

}

CLUSTER::~CLUSTER(){

}

void merge_vector(vector<int>& array1, vector<int>& array2){
  if(array2.empty())
    return;
  for(int i=0;i<array2.size();++i){
    array1.push_back(array2[i]);
  }
}

//主要是合并计数count， 与各个点points
void CLUSTER::merge_grids(unordered_map<vector<int>,Cell>& grid){
  if(grid.empty())
    return;

  for(auto it=grid.begin();it!=grid.end();++it){
    auto iter = grid_.find(it->first);
    if(iter == grid_.end()){
      grid_.insert(make_pair(it->first, it->second));
    }
    else{
      (iter->second).count += (it->second).count;
      merge_vector((iter->second).points, (it->second).points);
    }
  }
}

// 遍历的过程的第一步只是处理响应的稠密的网格cell
// 并将所有的稀疏网格与噪声网格加入到noise中
void CLUSTER::bft_grid(Cell& cell, int cluster_id){
  if(density_atrribute(cell) != 0){ //不为稠密网络
    noise_.insert(cell->id); 
    return;
  }

  if(cell->cluid != 0) //说明已经被遍历过了的
    return;

  cell->cluid = cluster_id;

  const int delta[6][3] = {{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0 ,0, -1}};
  for(int i=0; i<6; ++i){
    vector<int> index(3);
    int flag = 0;
    for(int j=0; j<3; ++j){
      index[j] = (cell->id)[j] + delta[i][j];
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
        index[j] = (cell->id)[j] + delta[i][j];
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
        index[j] = (cell->id)[j] + delta[i][j];
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
      iter = cluid_count.find(key_cluid);
      if(iter == cluid_count.end() && key_cluid != 0){ //这里要排除周围
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
  if(grid_.empty()) 
    return -1;
  int cell_number = grid_.size();
  int N = 0, max_cell = 0;

  for(auto it=grid_.begin();it!=grid_.end();++it){
    N += (it->second).count;
    if((it->second).count>max_cell)
      max_cell = (it->second).count;
  }

  minpts_ = (N/cell_number + max_cell)/2;
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
