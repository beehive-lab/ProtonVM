/*
 * Copyright (c) 2020-2021, APT Group, Department of Computer Science,
 * The University of Manchester.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

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