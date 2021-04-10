#include<iostream>
#include "ExponentiateBoss.h"
using namespace std;

namespace youtubetutorial{
	bool is_even(int n)
	{
		auto check = [n]() {return (float)n / 2 - n / 2; };
		if (check() == 0) return true;
		else return false;
	}
	double exponentiate(int U)
	{
		auto get_e = [U]() {return 1 + (double)1 / U; };
		double e = get_e();
		for (int i = 1; i < U; i++)
		{
			e *= get_e();
		}
		return e;
	}
	double* fill_array(int size, int UPPER)
	{
		double* ptr = new double[size];
		double lower = 1;
		double upper = 1;
		double e = exponentiate(UPPER);
		if (is_even(size))
		{
			ptr[size / 2] = 1;
			for (int i = 1; i < size / 2; i++)
			{
				upper *= e;
				ptr[size / 2 + i] = upper;
				lower/=e;
				ptr[size / 2 - i] = lower;

			}
			lower /= e;
			ptr[0] = lower;
		}
		else
		{
			ptr[size / 2] = 1;
			for (int i = 1; i <= size / 2; i++)
			{
				upper *= e;
				ptr[size / 2 + i] = upper;
				lower /= e;
				ptr[size / 2 - i] = lower;
			}
		}
		return ptr;
	}
	ExponentiateBoss::ExponentiateBoss() {};
	ExponentiateBoss::ExponentiateBoss(int size) 
	{
		this->size = size;
		exp = fill_array(size, X);
	};
	ExponentiateBoss::~ExponentiateBoss() 
	{
		delete[] exp;
		exp = nullptr;
	}
	ostream& operator<<(ostream& o, ExponentiateBoss& e)
	{
		if (is_even(e.size))
		{
			for (int i = -e.size / 2; i < e.size / 2; i++)
			{
				o << "e(" << i << ")=" << e.exp[i + e.size / 2] << endl;
			}
		}
		else {
			for (int i = -e.size / 2; i < e.size / 2 + 1; i++)
			{
				o << "e(" << i << ")=" << e.exp[i + e.size / 2] << endl;
			}
		}
		return o;
	}
}