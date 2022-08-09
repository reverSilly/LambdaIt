#include <iostream>
#include "SyntaxTree.hpp"
using namespace reverSilly::LambdaIt;
using namespace std;
int main()
{
	auto i=(-it<int>);
	cout<<i.evaluate(1);
	/**
	 * (Variable<int>(2)-it<int>)
	 * =>
	 * int func(int it)
	 * {
	 * 		return 2-it;
	 * }
	 *
	 * (Variable<int>(2)-it<int>).evaluate(1)
	 * =>
	 * func(1)
	 */
	return 0;
}
