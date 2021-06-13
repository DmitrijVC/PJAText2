#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <fstream>


using u16 = unsigned short int;
using i16 = short int;

using u32 = unsigned int;
using i32 = int;

using u64 = unsigned long long;
using i64 = long long;

using f32 = float;
using f64 = double;

using usize = size_t;

using String = std::string;
using StringStream = std::stringstream;

template <typename T>
using Vec = std::vector<T>;

template <typename K, typename V>
using HashMap = std::unordered_map<K, V>;

template <typename T, typename U>
using Pair = std::pair<T, U>;

using IFStream = std::ifstream;
using OFStream = std::ofstream;
