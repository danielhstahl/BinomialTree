#ifndef __BTREE_H_INCLUDED__
#define __BTREE_H_INCLUDED__
#include "FunctionalUtilities.h"


namespace btree{
    template<typename Number, typename Width, typename Height>
    struct Node {
        //Number AmericanPrice;
        Number Price; 
        Width width;
        Height height;
        bool CheckExercise;
        Node(){
            Price=0;
            width=0;
            height=0;
            CheckExercise=true;
        }
        Node(Number&& price_, const Width& width_, const Height& height_, bool check){
            Price=price_;
            width=width_;
            height=height_;
            CheckExercise=check;
        }
    };
    
    /**helper function to contrain P to zero and one*/
    template<typename Val>
    auto betweenZeroAndOne(Val&& val){
        return val>1?1:val<0?0:val;
    }

    template<typename AlphOutput, typename SigOutput, typename SqrtDT>
    auto computeP(AlphOutput&& alph, SigOutput&& sig, const SqrtDT& sqrtdt){
        return betweenZeroAndOne((alph-sig*.5)*.5*sqrtdt+.5);
    }
    template<typename P,  typename Price, typename Discount>
    auto computeExpectation(P&& p, const Price& upper, const Price& lower, Discount&& discount){
        return (p*upper+(1.0-p)*lower)*discount;
    }

    template<typename SqrtDT, typename Number, typename Height>
    auto computeSkeleton(const Number& x0, const Height& height, const SqrtDT& sqrtdt){
        return x0+sqrtdt*height;
    }

    template<typename Val>
    auto max(const Val& val1, const Val& val2){
        return val1>val2?val1:val2;
    }

    template<typename Node, typename DT, typename Number, typename SqrtDT, typename Alpha, typename Sigma, typename Discount, typename FINV, typename Payoff>
    auto iterateTree(
        Node&& upper, 
        const Node& lower, 
        const DT& dt, 
        const SqrtDT& sqrtdt, 
        const Number& y0, 
        const Alpha& alpha, 
        const Sigma& sigma, 
        const Discount& discount, 
        const FINV& fInv, 
        const Payoff& payoff
    ){
        if(upper.width==0){
            return upper;
        }
        upper.width--;
        upper.height--; //note that the difference in height between upper and lower is 2
        auto t=upper.width*dt;
        auto underlying=fInv(t, computeSkeleton(y0, upper.height, sqrtdt), dt, upper.width);
        upper.Price=computeExpectation(
            computeP(
                alpha(t, underlying, dt, upper.width),
                sigma(t, underlying, dt, upper.width),
                sqrtdt
            ),
            upper.Price,
            lower.Price,
            discount(t, underlying, dt, upper.width)
        );
        upper.Price=upper.CheckExercise?max(upper.Price, payoff(t, underlying, dt, upper.width)):upper.Price;
        return std::move(upper);
    }
    

    
    template<typename Alpha, typename Sigma, typename FInv, typename Payoff, typename Discount, typename Number, typename N>
    auto computePrice(
        const Alpha& alpha, //function(t, x, dt, width) for the drift ->alpha=alpha/sigma
        const Sigma& sigma, //function(t, x, dt, width) for the volatilty ->sigma=sigma'(x)
        const FInv& fInv, //function(t, y, dt, width) inverse of the function g=\int 1/sig(x) dx 
        const Payoff& payoff, //payoff function(t, x, dt, width)
        const Discount& discount, //discount function(t, x, dt, width)
        const Number& y0, //current value of "y": \int 1/sig(x) dx
        const N& numberOfTimePeriods, //eg, 1 for a simple binomial tree with 2 terminal nodes
        const Number& maturity,//time till maturity (in years)
        bool isAmerican //if option is american.  Else, european
    ){
        const Number dt=maturity/numberOfTimePeriods;
        const Number sqrtdt=sqrt(dt);
        auto getNodes=[&](auto&& nodes){
            return futilities::for_each_exclude_last(nodes, [&](auto&& curr, const auto& next, const auto& index){
             return iterateTree(std::move(curr), next, dt, sqrtdt, y0, alpha, sigma, discount, fInv, payoff);
            });
        };
        return futilities::recurse_move( 
            futilities::for_each_parallel(0, numberOfTimePeriods+1, [&](const auto& index){
                auto height=numberOfTimePeriods-2*index;
                return Node<Number, N, N>(
                    payoff(
                        maturity, 
                        fInv(maturity, computeSkeleton(y0, height, sqrtdt), dt, numberOfTimePeriods),
                        dt, 
                        numberOfTimePeriods
                    ),
                    numberOfTimePeriods,
                    height,
                    isAmerican
                );
            }),
            getNodes,
            [&](auto&& val){
                return val.size()>1;
            }
        )[0].Price;
    } 
    /*by default, american*/
    template<typename Alpha, typename Sigma, typename FInv, typename Payoff, typename Discount, typename Number, typename N>
    auto computePrice(
        const Alpha& alpha, //function(t, x, dt, width) for the drift ->alpha=alpha/sigma
        const Sigma& sigma, //function(t, x, dt, width) for the volatilty ->sigma=sigma'(x)
        const FInv& fInv, //function(t, y, dt, width) inverse of the function g=\int 1/sig(x) dx 
        const Payoff& payoff, //payoff function(t, x, dt, width)
        const Discount& discount, //discount function(t, x, dt, width)
        const Number& y0, //current value of "y": \int 1/sig(x) dx
        const N& numberOfTimePeriods, //eg, 1 for a simple binomial tree with 2 terminal nodes
        const Number& maturity//time till maturity (in years)
    ){
        return computePrice(alpha, sigma, fInv, payoff, discount, y0, numberOfTimePeriods, maturity, true);
    }

}

#endif