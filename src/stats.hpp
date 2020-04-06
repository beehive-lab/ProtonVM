#ifndef STATS_HPP
#define STATS_HPP

#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

template <class Type> Type median(vector<Type> data) {
    if(data.empty()) {
        return 0;
    } else {
        sort(data.begin(), data.end());
        if(data.size() % 2 == 0) {
            return (data[data.size()/2 - 1] + data[data.size()/2]) / 2;
        } else {
           return double(data[data.size()/2]);
        }
    }
}

#endif