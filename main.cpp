#include <iostream>
#include "LambdaIt.hpp"
using namespace reverSilly::LambdaIt;
using namespace std;
void func()
{
	cout<<20;
}
int main()
{
	auto i=(&(it<int&> =it<int>)).get();
	int j=20;
	cout<<*i(j,50)<<' '<<j;
	/**
	 * getEvaluator(2-it<int>)
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