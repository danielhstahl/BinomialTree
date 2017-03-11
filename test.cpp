#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "FunctionalUtilities.h"
#include "BTree.h"
#include "BlackScholes.h"
#include <chrono>
/*Black Scholes drift and Put payoffs */
double alpha(double sig, double alph){
  return alph/sig; //alpha/sigma
}
double sigma(double sig){
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
double discount(double dt, double r){
  return exp(-r*dt);
}
double finv(double x, double sig){
  return exp(sig*x);
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

TEST_CASE("Test BlackScholes", "[BTree]"){
    const double r=.03;
    const double sig=.3;
    const double S0=50;
    const double T=1;
    const double k=50;
    auto t1 = std::chrono::high_resolution_clock::now();
    auto AlphaDivSigma=[&](double t, double underlying, double dt, int width ){return alpha(sig, r);};
    auto SigmaPrime=[&](double t, double underlying, double dt, int width){return sigma(sig);};
    auto FInv=[&](double t, double x, double dt, int width){return finv(x, sig);};
    auto Payoff=[&](double t, double underlying, double dt, int width){return payoff(underlying, k);};
    auto Discount=[&](double t, double underlying, double dt, int width){return discount(dt, r);};
    auto functionsSet=btree::setFunctions(
       AlphaDivSigma,
       SigmaPrime,
       FInv,
       Payoff,
       Discount
    );
    auto ApproxPrice=functionsSet(log(S0)/sig, 5000, T, false);
    /*auto periodsSet=btree::setNumberOfPeriods(
      5000,
      functionsSet
    );
    auto currValueSet=btree::setCurrValue(log(S0)/sig, periodsSet);
    auto maturitySet=btree::setMaturity(T, currValueSet);
    auto ApproxPrice=maturitySet(false);*/
    
    /*btree::computePrice(
       [&](double t, double underlying, double dt, int width ){return alpha(sig, r);},
       [&](double t, double underlying, double dt, int width){return sigma(sig);}, 
       [&](double t, double x, double dt, int width){return finv(x, sig);}, 
       [&](double t, double underlying, double dt, int width){return payoff(underlying, k);}, 
       [&](double t, double underlying, double dt, int width){return discount(dt, r);},
       log(S0)/sig,
       5000,
       T,
       false
    );*/
	  auto t2 = std::chrono::high_resolution_clock::now();
    std::cout<<"It took "
    << std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count()
    << " milliseconds\n";
    REQUIRE(ApproxPrice==Approx(
        BSPut(S0, exp(-r*T), k, sig*sqrt(T))
    ).epsilon(.0001));
}
TEST_CASE("Test CIR", "[BTree]"){
    const double r=.03;
    const double sig=.3;
    const double initialR=2.0*sqrt(r)/sig;
    const double T=1.0;
    const double a=1;
    const double b=.05;
    auto bondcir=[](double r, double a, double b, double sigma, double tau){
        double h=sqrt(a*a+2*sigma*sigma);
        double expt=exp(tau*h)-1;
        double den=2*h+(a+h)*expt;
        double AtT=(2*a*b)/(sigma*sigma);
        AtT=AtT*log(2*h*exp((a+h)*tau*.5)/den);
        double BtT=2*expt/den;
        return exp(AtT-r*BtT);
    };
    REQUIRE(btree::computePrice(
       [&](double t, double underlying, double dt, int width ){return alphaR(t, underlying, a, b, sig);},
       [&](double t, double underlying, double dt, int width){return sigmaR(t, underlying, sig);}, 
       [&](double t, double x, double dt, int width){return finvR(x, sig);}, 
       [&](double t, double underlying, double dt, int width){return payoffR(underlying);}, 
       [&](double t, double underlying, double dt, int width){return discountR(t, underlying, dt);},
       initialR,
       5000,
       T,
       false
    )==Approx(
        bondcir(r, a, b,sig, T)
    ).epsilon(.0001));
}

