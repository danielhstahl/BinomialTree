//template< typename ALPHA, typename SIGMA, typename FINV, typename PAYOFF, typename DISCOUNT>
template<typename Number>
auto computeTree(auto& alpha,  auto& sigma,  auto& fInv, auto& payoff, auto& discount, int m, double t, const Number& x0, std::unordered_map<int, bool>& exerciseNodes){ //alpha=alpha/sigma,sigma=sigma'(x), finv=the inverse of the function g=\int 1/sig(x) dx
    //double t=(m-1)*dt;
    double dt=t/(m-1);
    std::vector<Number> optionPrice(m);
    int mm=m;
    double sqrtDt=sqrt(dt);
    for(int i=0; i<mm; i++){
        auto underlyingX=fInv(t, x0+sqrtDt*(mm-1-2*i), dt, mm);
        optionPrice[i]=payoff(t, underlyingX, dt, mm);
    }
    mm--;
    for(int j=(m-2); j>=0; j--){
        t=j*dt;
        for(int i=0; i<mm; i++){
            auto underlyingX=fInv(t, x0+sqrtDt*(mm-1-2*i), dt, mm);
            auto alph=alpha(t, underlyingX, dt, mm);
            auto sig=sigma(t, underlyingX, dt, mm);
            auto p=(alph-sig*.5)*.5*sqrtDt+.5;
            if(p>1){
                p=1;
                //numberOfPChanged++;
            }
            else if(p<0){
                p=0;
                //numberOfPChanged++;
            }
            optionPrice[i]=(p*optionPrice[i]+(1-p)*optionPrice[i+1])*discount(t, underlyingX, dt, mm);
            if(exerciseNodes.find(i)!=exerciseNodes.end()){ //if the key exists
                if(exerciseNodes[i]){ //if the option can be exercised
                    auto intrinsic=payoff(t, underlyingX, dt, mm);
                    if(optionPrice[i]<intrinsic){
                        optionPrice[i]=intrinsic;
                    }
                }
            }
        }
        optionPrice.pop_back();
        mm--;
    }
    return optionPrice[0];
}

template<typename Number> //american option
auto computeTree(auto& alpha,  auto& sigma,  auto& fInv, auto& payoff, auto& discount, int m, double t, const Number& x0){ //alpha=alpha/sigma,sigma=sigma'(x), finv=the inverse of the function g=\int 1/sig(x) dx
    double dt=t/(m-1);
    std::vector<Number> optionPrice(m);
    int mm=m;
   // int numberOfPChanged=0;
    double sqrtDt=sqrt(dt);
    for(int i=0; i<mm; i++){
        Number underlyingX=fInv(t, x0+sqrtDt*(mm-1-2*i), dt, mm);
        optionPrice[i]=payoff(t, underlyingX, dt, mm);       
    }
    mm--;
    for(int j=(m-2); j>=0; j--){
        t=j*dt;
        for(int i=0; i<mm; i++){
            Number underlyingX=fInv(t, x0+sqrtDt*(mm-1-2*i), dt, mm);
            auto alph=alpha(t, underlyingX, dt, mm);
            auto sig=sigma(t, underlyingX, dt, mm);
            auto p=(alph-sig*.5)*.5*sqrtDt+.5;
            if(p>1.0){
                p=1.0;
                //numberOfPChanged++;
            }
            else if(p<0.0){
                p=0.0;
                //numberOfPChanged++;
            }
            optionPrice[i]=(p*optionPrice[i]+(1-p)*optionPrice[i+1])*discount(t, underlyingX, dt, mm);
            auto intrinsic=payoff(t, underlyingX, dt, mm);
            if(optionPrice[i]<intrinsic){
                optionPrice[i]=intrinsic;
            }
        }
        optionPrice.pop_back();
        mm--;
    }
    return optionPrice[0];
}