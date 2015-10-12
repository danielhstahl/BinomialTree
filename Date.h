 #ifndef __DATE_H_INCLUDED__
#define __DATE_H_INCLUDED__
#include <cmath>
#include <cstdlib>
#include <string>
#include <ctime>
#include <iostream>
//#include <sstream>
class Date {
	private:
		struct std::tm * primitiveTM;
		//struct primitiveTM;
    std::time_t primitive;
    std::string scale="day";
    void setPrimitive(std::string&);
	public:
		Date();
    Date(std::string&);
    Date(char const[]);
	  std::time_t getPrimitive();
    Date operator=(std::string&);
    Date operator=(char const[]);
		double dateDiff(Date&);
		void setScale(std::string&);
    void setScale(char const[]);
    int size();
    std::string type();
    friend double operator-(Date&, Date&);
};

#endif
