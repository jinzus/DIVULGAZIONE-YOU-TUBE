#include<iostream>
using namespace std;

#ifndef EXPONENTIATEBOSS_H_
#define EXPONENTIATEBOSS_H_

namespace youtubetutorial {
	class ExponentiateBoss
	{
		int size;
		int X{ 1000000 };
		double* exp{ nullptr };
	public:
		ExponentiateBoss();
		ExponentiateBoss(int size);
		friend ostream& operator<<(ostream& o, ExponentiateBoss& e);
		~ExponentiateBoss();
	};
}

#endif
