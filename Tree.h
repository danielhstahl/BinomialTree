#ifndef __TREE_H_INCLUDED__
#define __TREE_H_INCLUDED__
#include <iostream>
#include <cmath>
#include <unordered_map>
#include <vector>
template<typename Number>
auto computeTree(
    auto&,  //function(t, x) for the drift ->alpha=alpha/sigma
    auto&,  //function(t, x) for the volatility->sigma=sigma'(x)
    auto&, //inverse of the function g=\int 1/sig(x) dx
    auto&, //payoff function(x)
    auto&, //discount function(t, x, dt)
    int, //number of nodes
    double, //time
    Number&, //current value
    std::unordered_map<int, bool>& //nodes to exercise at
); 
template<typename Number>
auto computeTree(
    auto&,  //function(t, x) for the drift ->alpha=alpha/sigma
    auto&,  //function(t, x) for the volatility->sigma=sigma'(x)
    auto&, //inverse of the function g=\int 1/sig(x) dx
    auto&, //payoff function(x)
    auto&, //discount function(t, x, dt)
    int, //number of nodes
    double, //time
    Number& //current value
); 
#include "Tree.hpp"
#endif