#include "Tree.h"

Tree::Tree(int m_, double t, double x0_){
  m=m_;
  maturity=t;
  dt=t/((double)m-1.0);
  optionPrice=std::vector<double>(m);
  x0=x0_;
  //exerciseNodes(m, 0);//default to all zero
}
Tree::Tree(int m_, double t, double x0_, std::unordered_map<int, bool> &exercise){
  m=m_;
  maturity=t;
  dt=t/((double)m-1.0);
  exerciseNodes=exercise;
  optionPrice=std::vector<double>(m);
  x0=x0_;
}
Tree::Tree(int m_, double t, double x0_, bool isAmerican){
  m=m_;
  maturity=t;
  dt=t/((double)m-1.0);
  if(isAmerican){
    for(int i=0; i<m; i++){
      exerciseNodes.insert({{i, true}});
    }
  }
  optionPrice=std::vector<double>(m);
  x0=x0_;

}
double Tree::getPrice(){
  return finalValue;
}
//double Tree::computeP(double x){

//}
