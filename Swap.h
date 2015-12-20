#ifndef __SWAP_H_INCLUDED__
#define __SWAP_H_INCLUDED__
#include <vector>
#include <cmath>
#include <iostream> //for debugging
#include "Date.h"
#include "Double.h"
class Swap {
	private:
		std::vector<double> libor;
    double tenor;
    int n;
    std::vector<double> bonds;
    struct ForwardValue{
      Date beginDate;
      Date endDate;
      Double value;
    };
	public:
		Swap(const std::vector<double>& ,  double);
		double getPrice(double, double); //returns price for a given time to maturity and swap rate
		double getPrice(double, double,  double); //returns price at times when interval isn't exact
    double getRate(double);
    double getBondPrice(double);
    double getBondYield(double);
};

#endif
