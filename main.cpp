#include <iostream>
#include "LambdaIt.hpp"
using namespace reverSilly;
using namespace std;
struct A
{
	int operator[](int a)
	{
		return a*2;
	}
};
int main()
{
	using LambdaIt::it;

	cout<< (it<int>).at(20);
	/**
	 * (2-it<int>)
	 * =>
	 * int func(int it)
	 * {
	 * 		return 2-it;
	 * }
	 *
	 * getEvaluator(2-it<int>)
	 * =>
	 * func(1)
	 */
	return 0;
}