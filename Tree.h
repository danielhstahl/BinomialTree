#ifndef __TREE_H_INCLUDED__
#define __TREE_H_INCLUDED__
#include <iostream>
#include <cmath>
#include <unordered_map>
#include <vector>
class Tree{ //simulates SDE of type dX=alpha(t, X)dt+sigma(t, X)dW...I think I need to figure out dividends!
  private:
    int m;//number of steps
    double maturity;//maturity in years
    double dt;
    double x0;
    double finalValue;
    int numberOfPChanged=0;
    std::unordered_map<int, bool> exerciseNodes;
    std::vector<double> optionPrice;
  public:
    Tree(int, double, double);
    Tree(int, double, double, std::unordered_map<int, bool>&); //include exercise nodes...use this in conjuctions with a date module!
    Tree(int, double, double, bool);//american or not
    template< typename ALPHA, typename SIGMA, typename FINV, typename PAYOFF, typename DISCOUNT>
    void execute(ALPHA&& alpha,  SIGMA&& sigma,  FINV&& fInv, PAYOFF&& payoff, DISCOUNT&& discount){ //alpha=alpha/sigma,sigma=sigma'(x), finv=the inverse of the function g=\int 1/sig(x) dx
      double p=0;
      double t=(m-1)*dt;
      double alph;
      double sig;
      double underlyingX=0;
      double intrinsic=0;
      int mm=m;
      double sqrtDt=sqrt(dt);
      for(int i=0; i<mm; i++){
        underlyingX=fInv(x0+sqrtDt*(mm-1-2*i));
        optionPrice[i]=payoff(underlyingX);
      }
      mm--;
      for(int j=(m-2); j>=0; j--){
        t=j*dt;
        for(int i=0; i<mm; i++){
          underlyingX=fInv(x0+sqrtDt*(mm-1-2*i));
          alph=alpha(t, underlyingX);
          sig=sigma(t, underlyingX);
          p=(alph-sig*.5)*.5*sqrtDt+.5;
          if(p>1){
            p=1;
            numberOfPChanged++;
          }
          else if(p<0){
            p=0;
            numberOfPChanged++;
          }
          optionPrice[i]=(p*optionPrice[i]+(1-p)*optionPrice[i+1])*discount(t, underlyingX, dt);
          if(exerciseNodes.find(i)!=exerciseNodes.end()){ //if the key exists
            if(exerciseNodes.at(i)){ //if the option can be exercised
              intrinsic=payoff(underlyingX);
              if(optionPrice[i]<intrinsic){
                optionPrice[i]=intrinsic;
              }
            }
          }
        }
        optionPrice.pop_back();
        mm--;
      }
      finalValue=optionPrice[0];
    }
    double getPrice();
};
#endif
