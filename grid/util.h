#ifndef _UTIL_H
#define _UTIL_H

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


#endif
