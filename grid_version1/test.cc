#include <iostream>
#include <unordered_map>
#include <vector>
#include <algorithm>
using namespace std;

struct HashFunc{
  size_t operator()(const vector<int>& key)const
  {
    string str;
    for(int i=0;i<key.size();++i){
      str += to_string(key[i]);
    }
    return hash<string>()(str);
  }
};

struct EqualKey{
  bool operator()(const vector<int>& key1, const vector<int>& key2)const
  {
    if(key1.size() != key2.size())
      return false;
    for(int i=0; i<key1.size(); ++i){
      if(key1[i] != key2[i])
        return false;
    }
    return true;
  }
};

int main(){
  unordered_map<int, int> map1;
  unordered_map<vector<int>, int, HashFunc, EqualKey> map2;
  vector<int> arr = {1, 2};
  vector<int> arr1 = {1, 2, 3};
  vector<int> arr2 = { 2, 3};

  HashFunc hashfun;
  cout<<hashfun(arr)<<endl;
  cout<<hashfun(arr1)<<endl;
  cout<<hashfun(arr2)<<endl;

  map1.insert(make_pair(1, 2));
  map2.insert(make_pair(arr, 3));
}
