#include <iostream>
#include <cmath>
#include "Tree.h"
#include <chrono> //for accurate multithreading time using std::chrono

/*Black Scholes drift and Put payoffs */
double alpha(double t, double x, double sig, double alph){
  return alph/sig; //alpha/sigma
}
double sigma(double t, double x, double sig){
  return sig; //sigma'(x)
}
double payoff(double x, double k){
  if(k>x){
    return k-x;
  }
  else{
    return 0;
  }
}
double discount(double t, double x, double dt, double r){
  return exp(-r*dt);
}
double finv(double x, double sig){
  return exp(sig*x)/sig;
}
/*End Black Scholes */

/*Begin interest rate "option" (bond pricing) assuming CIR underlying*/
double alphaR(double t, double x, double a, double b, double sig){
  return (a*(b-x))/(sig*sqrt(x));
}
double sigmaR(double t, double x, double sig){
  return (.5*sig)/sqrt(x);
}
double payoffR(double x){
  return 1;
}
double discountR(double t, double x, double dt){
  return exp(-x*dt);
}
double finvR(double x, double sig){
  return sig*sig*x*x*.25;
}


/*End interest rate "option" */
int main(){
  //double alph=.1;
  double sig=.3;
  double k=50;
  double stockPrice=log(50*sig)/sig;

  //std::cout<<stockPrice<<std::endl;
  double r=.03;
  double initialR=2.0*sqrt(r)/sig;
  double t=1;
  double a=1;
  double b=.05;//long run r;
  /*Tree for BS*/
  Tree treeBS(1000, t, stockPrice, true);//american
  Tree treeCIR(1000, t, initialR);//european
  treeBS.execute([&](double t, double x){return alpha(t, x, sig, r);}, [&](double t, double x){return sigma(t, x, sig);}, [&](double x){return finv(x, sig);}, [&](double x){return payoff(x, k);}, [&](double t, double x, double dt){return discount(t, x, dt, r);}); //ALPHA&& alpha,  SIGMA&& sigma,  FINV&& fInv, PAYOFF&& payoff, DISCOUNT&& discount

  /*Tree for bond pricing */
  auto start = std::chrono::system_clock::now();
  treeCIR.execute([&](double t, double x){return alphaR(t, x, a, b, sig);}, [&](double t, double x){return sigmaR(t, x, sig);}, [&](double x){return finvR(x, sig);}, [&](double x){return payoffR(x);}, [&](double t, double x, double dt){return discountR(t, x, dt);}); //ALPHA&& alpha,  SIGMA&& sigma,  FINV&& fInv, PAYOFF&& payoff, DISCOUNT&& discount
  auto end=std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()-start);
  std::cout<<"Time it took: "<<end.count()/1000.0<<std::endl;
  std::cout<<treeBS.getPrice()<<std::endl;
  std::cout<<treeCIR.getPrice()<<std::endl;
}
