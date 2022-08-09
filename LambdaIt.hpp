//
// Created by Administrator on 2022-08-08.
//
#include <cstddef>
#include <utility>
#include <tuple>
#include <functional>

#include <iostream>

namespace reverSilly
{
	namespace LambdaIt
	{
		namespace
		{
			template<typename T>
			struct function_traits;

			template<typename R,typename ...Args>
			struct function_traits<std::function<R(Args...)>>
			{
				static constexpr size_t nargs=sizeof...(Args);
				using result_type=R;
				using tuple_args=std::tuple<Args...>;
				// 输入参数类型,i为从0开始的参数类型索引
				template <size_t i>
				struct arg
				{using type=typename std::tuple_element<i,tuple_args>::type;};
			};

			template<typename T,typename R, typename ...Args>
			struct function_traits<R(T::*)(Args...)>
			{
				static constexpr size_t nargs=sizeof...(Args);
				// 返回类型
				using result_type=R;
				using member_type=T;
				// 输入参数类型,i为从0开始的参数类型索引
				template <size_t i>
				struct arg{using type=typename std::tuple_element<i,std::tuple<Args...>>::type;};
			};
			template<typename T,typename R, typename ...Args>
			struct function_traits<R(T::*)(Args...)const>
			{
				static constexpr size_t nargs=sizeof...(Args);
				// 返回类型
				using result_type=R;
				using member_type=T;
				// 输入参数类型,i为从0开始的参数类型索引
				template <size_t i>
				struct arg{using type=typename std::tuple_element<i,std::tuple<Args...>>::type;};
			};

			template<std::size_t beg,typename T,std::size_t...indices>
			auto get_first_n_since_impl(T&& thing,std::index_sequence<indices...>)
			{
				return std::tuple(std::get<beg+indices>(std::forward<T>(thing))...);
			}

			template<std::size_t beg,std::size_t n,typename ...Args>
			auto get_first_n_since(Args&&...args)
			{
				return get_first_n_since_impl<beg>(std::forward_as_tuple(args...),std::make_index_sequence<n>{});
			}

			template<std::size_t n,typename ...Args>
			auto get_first_n(Args&&...args)
			{
				return get_first_n_since<0,n>(std::forward<Args>(args)...);
			}

			template<std::size_t n,typename ...Args>
			auto get_last_n(Args&&...args)
			{
				return get_first_n_since<sizeof...(Args)-n,n>(std::forward_as_tuple(args...),std::make_index_sequence<n>{});
			}
		}
		
		namespace OperationFunctors
		{
			/**
			 * unary functors
			 */
			constexpr auto Flip{[](auto i){return ~i;}};
			constexpr auto Not{[](auto i){return !i;}};
			constexpr auto AddressOf{[](auto i){return &i;}};
			constexpr auto Positive{[](auto i){return +i;}};
			constexpr auto Negative{[](auto i){return -i;}};
			constexpr auto Dereference{[](auto i){return *i;}};
			constexpr auto PrefixIncrease{[](auto i){return ++i;}};
			constexpr auto PostfixIncrease{[](auto i){return i++;}};
			constexpr auto PrefixDecrease{[](auto i){return --i;}};
			constexpr auto PostfixDecrease{[](auto i){return i--;}};
			constexpr auto IndirectAccess{[](auto i){return i.operator->();}};
		}
		namespace OperationFunctors
		{
			/**
			 * binary functors
			 */
			constexpr auto LessThan{[](auto i,auto j){return (i<j);}};
			constexpr auto GreaterThan{[](auto i,auto j){return (i>j);}};
			constexpr auto GreaterEqual{[](auto i,auto j){return i>=j;}};
			constexpr auto LessEqual{[](auto i,auto j){return i<=j;}};
			constexpr auto EqualTo{[](auto i,auto j){return i==j;}};
			constexpr auto NotEqualTo{[](auto i,auto j){return i!=j;}};
			constexpr auto CompareTo{[](auto i,auto j){return i<=>j;}};
			constexpr auto Plus{[](auto i,auto j){return i+j;}};
			constexpr auto Minus{[](auto i,auto j){return i-j;}};
			constexpr auto Multiply{[](auto i,auto j){return i*j;}};
			constexpr auto Divide{[](auto i,auto j){return i/j;}};
			constexpr auto Modular{[](auto i,auto j){return i%j;}};
			constexpr auto ExclusiveOr{[](auto i,auto j){return i^j;}};
			constexpr auto And{[](auto i,auto j){return i&j;}};
			constexpr auto Or{[](auto i,auto j){return i|j;}};
			constexpr auto Assign{[](auto i,auto j){return i=j;}};
			constexpr auto PlusBy{[](auto i,auto j){return i+=j;}};
			constexpr auto MinusBy{[](auto i,auto j){return i-=j;}};
			constexpr auto MultiplyBy{[](auto i,auto j){return i*=j;}};
			constexpr auto DividedBy{[](auto i,auto j){return i/=j;}};
			constexpr auto ModularBy{[](auto i,auto j){return i%=j;}};
			constexpr auto ExclusiveOrBy{[](auto i,auto j){return i^=j;}};
			constexpr auto AndBy{[](auto i,auto j){return i&=j;}};
			constexpr auto OrBy{[](auto i,auto j){return i|=j;}};
			constexpr auto LeftShift{[](auto i,auto j){return i<<j;}};
			constexpr auto RightShift{[](auto i,auto j){return (i>>j);}};
			constexpr auto RightShiftBy{[](auto i,auto j){return (i>>=j);}};
			constexpr auto LeftShiftBy{[](auto i,auto j){return i<<=j;}};
			constexpr auto ShortAnd{[](auto i,auto j){return i&&j;}};
			constexpr auto ShortOr{[](auto i,auto j){return i||j;}};
			constexpr auto Comma{[](auto i,auto j){return i,j;}};
			constexpr auto DereferenceIndirectAccess{[](auto i,auto j){return i->*j;}};
		}

//		template<typename T>
////		concept IsExpression=std::is_member_function_pointer_v<decltype(&T::evaluate)>;
////		^
////      | old one
//		concept IsExpression=true;


		template<typename T,auto Op>
		struct UnaryOperator;
		template<typename L,typename R,auto Op>
		struct BinaryOperator;
		template<typename T>
		struct Unknown;
		template<typename T>
		struct Variable;
		template<typename F,typename ...Args>
		struct Call;
		template<typename A,typename ...I>
		struct Subscript;

		template<typename T>
		auto getEvaluator(T&& thing);
		template<typename T>
		auto getEvaluator(Unknown<T>);
		template<typename T>
		auto getEvaluator(Variable<T>thing);
		template<typename L,typename R,auto Op>
		auto getEvaluator(BinaryOperator<L,R,Op> operation);
		template<typename T,auto Op>
		auto getEvaluator(UnaryOperator<T,Op>operation);
		template<typename F,typename ...Params>
		auto getEvaluator(Call<F,Params...>operation);
		template<typename A,typename ...I>
		auto getEvaluator(Subscript<A,I...>operation);
		
		template<typename T>
		using IndirectAccess=UnaryOperator<T,OperationFunctors::IndirectAccess>;
		template<typename L,typename R>
		using Assign=BinaryOperator<L,R,OperationFunctors::Assign>;
		
		template<typename Me>
		struct Expression
		{
			template<typename T>
			Assign<Me,T> operator=(const T &that);

			IndirectAccess<Me> operator->();

			template<typename ...Args>
			Call<Me,Args...> operator()(Args&&...args);

			template<typename ...I>
			Subscript<Me,I...> operator[](I&&...i);

			auto get()const
			{
				return getEvaluator(*reinterpret_cast<const Me*>(this));
			}

			template<typename ...Args>
			auto at(Args&&...args)const
			{
				return get()(std::forward<Args>(args)...);
			}

			operator auto() const
			{
				return get();
			}
		};

		template<typename T>
		struct Variable:public Expression<Variable<T>>
		{
			operator T&(){return value;}
			[[no_unique_address]]T value;

			template<typename ...Args>
			constexpr Variable(Args&&...args):value{std::forward<Args>(args)...}{}
		};
		template<typename T>
		struct Unknown:public Expression<Unknown<T>>{static constexpr std::size_t nParameterNeeded{1};};
		template<typename T>
		Unknown<T> it;

		template<typename F,typename ...Args>
		struct Call:public Expression<Call<F,Args...>>
		{
			[[no_unique_address]]F callee;
			[[no_unique_address]]std::tuple<Args...> arguments;
			Call(const F& func,const std::tuple<Args...>& args):callee{func},arguments{args}{}
		};
		template<typename Me>
		template<typename ...Args>
		Call<Me,Args...>Expression<Me>::operator()(Args&&...args)
		{
			return Call<Me,Args...>{*reinterpret_cast<Me*>(this),std::forward_as_tuple(args...)};
			//error: could not convert
			// 'reverSilly::LambdaIt::Call<reverSilly::LambdaIt::Unknown<std::function<void(int)> >, int&>((*(const reverSilly::LambdaIt::Unknown<std::function<void(int)> >*)((reverSilly::LambdaIt::Unknown<std::function<void(int)> >*)((reverSilly::LambdaIt::Expression<reverSilly::LambdaIt::Unknown<std::function<void(int)> > >*)this))), std::forward_as_tuple(_Elements&& ...) [with _Elements = {int&}]())'
			// from 'Call<[...],int&>'
			// to 'Call<[...],int>'
		}

		template<typename A,typename ...I>
		struct Subscript:public Expression<Subscript<A,I...>>
		{
			[[no_unique_address]]A array;
			[[no_unique_address]]std::tuple<I...>indices;
			Subscript(const A& arr,const std::tuple<I...>&ind):array{arr},indices{ind}{}
		};
		template<typename Me>
		template<typename ...I>
		Subscript<Me,I...> Expression<Me>::operator[](I &&...i)
		{
			return Subscript<Me,I...>{*reinterpret_cast<Me*>(this),std::forward_as_tuple(i...)};
		}

		template<typename T,auto Op>
		struct UnaryOperator:public Expression<UnaryOperator<T,Op>>
		{
			[[no_unique_address]]T operand;
			constexpr explicit UnaryOperator(const T&thing):operand{thing}{}
		};
		

		template<typename T>
		using Flip=UnaryOperator<T,OperationFunctors::Flip>;
		template<typename T>
		auto operator~(const T&thing){return Flip<T>{thing};}
		
		template<typename T>
		using Not=UnaryOperator<T,OperationFunctors::Not>;
		template<typename T>
		Not<T> operator!(const T&thing){return Not<T>{thing};}
		
		template<typename T>
		using AddressOf=UnaryOperator<T,OperationFunctors::AddressOf>;
		template<typename T>
		AddressOf<T> operator&(const T&thing){return AddressOf<T>{thing};}
		
		template<typename T>
		using Positive=UnaryOperator<T,OperationFunctors::Positive>;
		template<typename T>
		Positive<T> operator+(const T&thing){return Positive<T>{thing};}
		
		template<typename T>
		using Negative=UnaryOperator<T,OperationFunctors::Negative>;
		template<typename T>
		Negative<T> operator-(const T&thing){return Negative<T>{thing};}
		
		template<typename T>
		using Dereference=UnaryOperator<T,OperationFunctors::Dereference>;
		template<typename T>
		Dereference<T> operator*(const T&thing){return Dereference<T>{thing};}
		
		template<typename T>
		using PrefixIncrease=UnaryOperator<T,OperationFunctors::PrefixIncrease>;
		template<typename T>
		auto operator++(const T&thing){return PrefixIncrease<T>{thing};}
		
		template<typename T>
		using PostfixIncrease=UnaryOperator<T,OperationFunctors::PostfixIncrease>;
		template<typename T>
		auto operator++(const T&thing,int){return PostfixIncrease<T>{thing};}
		
		template<typename T>
		using PrefixDecrease=UnaryOperator<T,OperationFunctors::PrefixDecrease>;
		template<typename T>
		auto operator--(const T&thing){return PrefixDecrease<T>{thing};}
		
		template<typename T>
		using PostfixDecrease=UnaryOperator<T,OperationFunctors::PostfixDecrease>;
		template<typename T>
		auto operator--(const T&thing,int){return PostfixDecrease<T>{thing};}
		
		template<typename T>
		using IndirectAccess=UnaryOperator<T,OperationFunctors::IndirectAccess>;
		template<typename Me>
		IndirectAccess<Me> Expression<Me>::operator->()
		{
			return IndirectAccess<Me>{*this};
		}
		

		template<typename L,typename R,auto Op>
		struct BinaryOperator:public Expression<BinaryOperator<L,R,Op>>
		{
			[[no_unique_address]]L lhs;
			[[no_unique_address]]R rhs;
			constexpr BinaryOperator(const L& lhs,const R&rhs):lhs{lhs},rhs{rhs}{}
//			template<typename ...Args>
//			auto evaluate(Args&&...args)
//			{
////				static_assert(IsExpression<BinaryOperator>);
//				static_assert(IsExpression<L>);
//				static_assert(IsExpression<R>);
//				constexpr size_t lparamn{function_traits<decltype(L::evaluate)>::nargs},rparamn{function_traits<decltype(R::evaluate)>::nargs};
//				static_assert(lparamn+rparamn==sizeof...(Args));
//				auto l{[this](auto &&...args){return lhs.evaluate(std::forward<decltype(args)>(args)...);}};
//				auto r{[this](auto &&...args){return rhs.evaluate(std::forward<decltype(args)>(args)...);}};
//				return Op(
//						std::apply(l,get_first_n<lparamn>(std::forward<Args>(args)...)),
//						std::apply(r,get_last_n<rparamn>(std::forward<Args>(args)...)));
//			}
		};
		template<typename L,typename R>
		using LessThan=BinaryOperator<L,R,OperationFunctors::LessThan>;
		template<typename L,typename R>
		auto operator<(const L&lhs,const R&rhs){return LessThan<L,R>(lhs,rhs);}
		
		template<typename L,typename R>
		using GreaterThan=BinaryOperator<L,R,OperationFunctors::GreaterThan>;
		template<typename L,typename R>
		auto operator>(const L&lhs,const R&rhs){return GreaterThan<L,R>(lhs,rhs);}
		
		template<typename L,typename R>
		using GreaterEqual=BinaryOperator<L,R,OperationFunctors::GreaterEqual>;
		
		template<typename L,typename R>
		auto operator>=(const L&lhs,const R&rhs){return GreaterEqual<L,R>(lhs,rhs);}
		
		template<typename L,typename R>
		using LessEqual=BinaryOperator<L,R,OperationFunctors::LessEqual>;
		template<typename L,typename R>
		auto operator<=(const L&lhs,const R&rhs){return LessEqual<L,R>(lhs,rhs);}
		template<typename L,typename R>
		using EqualTo=BinaryOperator<L,R,OperationFunctors::EqualTo>;
		template<typename L,typename R>
		auto operator==(const L&lhs,const R&rhs){return EqualTo<L,R>(lhs,rhs);}
		
		template<typename L,typename R>
		using NotEqualTo=BinaryOperator<L,R,OperationFunctors::NotEqualTo>;
		template<typename L,typename R>
		auto operator!=(const L&lhs,const R&rhs){return NotEqualTo<L,R>(lhs,rhs);}
		
		template<typename L,typename R>
		using CompareTo=BinaryOperator<L,R,OperationFunctors::CompareTo>;
		template<typename L,typename R>
		auto operator<=>(const L&lhs,const R&rhs){return CompareTo<L,R>(lhs,rhs);}

		template<typename L,typename R>
		using Plus=BinaryOperator<L,R,OperationFunctors::Plus>;
		template<typename L,typename R>
		auto operator+(const L&lhs,const R&rhs){return Plus<L,R>(lhs,rhs);}
		
		template<typename L,typename R>
		using Minus=BinaryOperator<L,R,OperationFunctors::Minus>;
		template<typename L,typename R>
		auto operator-(const L&lhs,const R&rhs){return Minus<L,R>(lhs,rhs);}
		
		template<typename L,typename R>
		using Multiply=BinaryOperator<L,R,OperationFunctors::Multiply>;
		template<typename L,typename R>
		auto operator*(const L&lhs,const R&rhs){return Multiply<L,R>(lhs,rhs);}
		
		template<typename L,typename R>
		using Divide=BinaryOperator<L,R,OperationFunctors::Divide>;
		template<typename L,typename R>
		auto operator/(const L&lhs,const R&rhs){return Divide<L,R>(lhs,rhs);}
		
		template<typename L,typename R>
		using Modular=BinaryOperator<L,R,OperationFunctors::Modular>;
		template<typename L,typename R>
		auto operator%(const L&lhs,const R&rhs){return Modular<L,R>(lhs,rhs);}
		
		template<typename L,typename R>
		using ExclusiveOr=BinaryOperator<L,R,OperationFunctors::ExclusiveOr>;
		template<typename L,typename R>
		auto operator^(const L&lhs,const R&rhs){return ExclusiveOr<L,R>(lhs,rhs);}
		
		template<typename L,typename R>
		using And=BinaryOperator<L,R,OperationFunctors::And>;
		template<typename L,typename R>
		auto operator&(const L&lhs,const R&rhs){return And<L,R>(lhs,rhs);}
		
		template<typename L,typename R>
		using Or=BinaryOperator<L,R,OperationFunctors::Or>;
		template<typename L,typename R>
		auto operator|(const L&lhs,const R&rhs){return Or<L,R>(lhs,rhs);}
		
		template<typename L,typename R>
		using Assign=BinaryOperator<L,R,OperationFunctors::Assign>;
		template<typename Me>
		template<typename T>
		Assign<Me,T> Expression<Me>::operator=(const T &that)
		{
			return Assign<Me,T>{*static_cast<Me*>(this),that};
		}
		
		template<typename L,typename R>
		using PlusBy=BinaryOperator<L,R,OperationFunctors::PlusBy>;
		template<typename L,typename R>
		auto operator+=(const L&lhs,const R&rhs){return PlusBy<L,R>(lhs,rhs);}
		
		template<typename L,typename R>
		using MinusBy=BinaryOperator<L,R,OperationFunctors::MinusBy>;
		template<typename L,typename R>
		auto operator-=(const L&lhs,const R&rhs){return MinusBy<L,R>(lhs,rhs);}
		
		template<typename L,typename R>
		using MultiplyBy=BinaryOperator<L,R,OperationFunctors::MultiplyBy>;
		template<typename L,typename R>
		auto operator*=(const L&lhs,const R&rhs){return MultiplyBy<L,R>(lhs,rhs);}
		
		template<typename L,typename R>
		using DividedBy=BinaryOperator<L,R,OperationFunctors::DividedBy>;
		template<typename L,typename R>
		auto operator/=(const L&lhs,const R&rhs){return DividedBy<L,R>(lhs,rhs);}
		
		template<typename L,typename R>
		using ModularBy=BinaryOperator<L,R,OperationFunctors::ModularBy>;
		template<typename L,typename R>
		auto operator%=(const L&lhs,const R&rhs){return ModularBy<L,R>(lhs,rhs);}
		
		template<typename L,typename R>
		using ExclusiveOrBy=BinaryOperator<L,R,OperationFunctors::ExclusiveOrBy>;
		template<typename L,typename R>
		auto operator^=(const L&lhs,const R&rhs){return ExclusiveOrBy<L,R>(lhs,rhs);}
		
		template<typename L,typename R>
		using AndBy=BinaryOperator<L,R,OperationFunctors::AndBy>;
		template<typename L,typename R>
		auto operator&=(const L&lhs,const R&rhs){return AndBy<L,R>(lhs,rhs);}
		
		template<typename L,typename R>
		using OrBy=BinaryOperator<L,R,OperationFunctors::OrBy>;
		template<typename L,typename R>
		auto operator|=(const L&lhs,const R&rhs){return OrBy<L,R>(lhs,rhs);}
		
		template<typename L,typename R>
		using LeftShift=BinaryOperator<L,R,OperationFunctors::LeftShift>;
		template<typename L,typename R>
		auto operator<<(const L&lhs,const R&rhs){return LeftShift<L,R>(lhs,rhs);}
		
		template<typename L,typename R>
		using RightShift=BinaryOperator<L,R,OperationFunctors::RightShift>;
		template<typename L,typename R>
		auto operator>>(const L&lhs,const R&rhs){return RightShift<L,R>(lhs,rhs);}
		
		template<typename L,typename R>
		using RightShiftBy=BinaryOperator<L,R,OperationFunctors::RightShiftBy>;
		template<typename L,typename R>
		auto operator>>=(const L&lhs,const R&rhs){return RightShiftBy<L,R>(lhs,rhs);}
		
		template<typename L,typename R>
		using LeftShiftBy=BinaryOperator<L,R,OperationFunctors::LeftShiftBy>;
		template<typename L,typename R>
		auto operator<<=(const L&lhs,const R&rhs){return LeftShiftBy<L,R>(lhs,rhs);}
		
		template<typename L,typename R>
		using ShortAnd=BinaryOperator<L,R,OperationFunctors::ShortAnd>;
		template<typename L,typename R>
		auto operator&&(const L&lhs,const R&rhs){return ShortAnd<L,R>(lhs,rhs);}
		
		template<typename L,typename R>
		using ShortOr=BinaryOperator<L,R,OperationFunctors::ShortOr>;
		template<typename L,typename R>
		auto operator||(const L&lhs,const R&rhs){return ShortOr<L,R>(lhs,rhs);}
		
		template<typename L,typename R>
		using Comma=BinaryOperator<L,R,OperationFunctors::Comma>;
		template<typename L,typename R>
		auto operator,(const L&lhs,const R&rhs){return Comma<L,R>(lhs,rhs);}

		template<typename L,typename R>
		using DereferenceIndirectAccess=BinaryOperator<L,R,OperationFunctors::DereferenceIndirectAccess>;
		
		template<typename L,typename R>
		auto operator->*(const L&lhs,const R&rhs){return DereferenceIndirectAccess<L,R>(lhs,rhs);}

		namespace
		{
			template<typename T>
			struct nParameterNeeded
			{
				static std::size_t consteval apply()
				{
					return 0;
				}
			};

			template<typename T>
			struct nParameterNeeded<Unknown<T>>
			{
				static std::size_t consteval apply()
				{
					return 1;
				}
			};

			template<typename T,auto Op>
			struct nParameterNeeded<UnaryOperator<T,Op>>
			{
				static std::size_t consteval apply()
				{
					return nParameterNeeded<T>::apply();
				}
			};

			template<typename L,typename R,auto Op>
			struct nParameterNeeded<BinaryOperator<L,R,Op>>
			{
				static std::size_t consteval apply()
				{
					return nParameterNeeded<L>::apply()+nParameterNeeded<R>::apply();
				}
			};

			template<typename F,typename ...Args>
			struct nParameterNeeded<Call<F,Args...>>
			{
				static std::size_t consteval apply()
				{
					return (nParameterNeeded<F>::apply()+...+nParameterNeeded<Args>::apply());
				}
			};

			template<typename A,typename ...I>
			struct nParameterNeeded<Subscript<A,I...>>
			{
				static std::size_t consteval apply()
				{
					return (nParameterNeeded<A>::apply()+...+nParameterNeeded<I>::apply());
				}
			};

			template<typename T,std::size_t...indices>
			auto get_evaluators_impl(T &&thing,std::index_sequence<indices...>)
			{
				return std::tuple(getEvaluator(std::get<indices>(std::forward<T>(thing)))...);
			}

			template<typename ...Args>
			auto get_evaluators(const std::tuple<Args...> &args)
			{
				return get_evaluators_impl(args,std::make_index_sequence<sizeof...(Args)>{});
			}

			template<typename F>
			auto function_cat(F func)
			{
				return [&func]<typename R,typename ...Args>(std::function<R(Args...)>)mutable
				{
					return [func](Args...args)mutable
					{
						return std::tuple(func(args...));
					};
				}(std::function(func));
			}

			template<typename F1,typename F2,typename ...F>
			auto function_cat(F1 func1,F2 func2,F...appender)
			{
				if constexpr(sizeof...(F)!=0)
					return function_cat(function_cat(func1,func2),appender...);
				else
				{
					return [&func1,&func2]<typename R1,typename ...Arg1>(std::function<R1(Arg1...)>)mutable
					{
						return [&func1,&func2]<typename R2,typename ...Arg2>(std::function<R2(Arg2...)>)mutable
						{
//							if constexpr((std::is_void_v<R1>)&&(std::is_void_v<R2>))
//							{
//								return [func1,func2](Arg1...args1,Arg2...args2)mutable
//								{
//									func1(args1...);
//									func2(args2...);
//									return;
//								};
//							}else if constexpr(std::is_void_v<R1>)
//							{
//								return [func1,func2](Arg1...args1,Arg2...args2)mutable
//								{
//									func1(args1...);
//									return std::forward<R2>(func2(args2...));
//								};
//							}else if constexpr(std::is_void_v<R2>)
//							{
//								return [func1,func2](Arg1...args1,Arg2...args2)mutable
//								{
//									func2(args2...);
//									return std::forward<R1>(func1(args1...));
//								};
//							}else
							{
								return [func1,func2](Arg1...args1,Arg2...args2)mutable
								{
									return std::tuple(std::forward<R1>(func1(args1...)),
									                  std::forward<R2>(func2(args2...)));
								};
							}
						}(std::function(func2));
					}(std::function(func1));
				}
			}

			template<typename ...T>
			auto function_tuple_cat(std::tuple<T...>&&thing)
			{
				return std::apply([](auto... args){return function_cat<T...>(args...);},thing);
			}
		}
		template<typename T>
		auto getEvaluator(T&& thing)
		{
			static_assert(!std::is_base_of_v<Expression<T>,T>);
			return [thing]()mutable{return std::forward<T>(thing);};
		}

		template<typename T>
		auto getEvaluator(Unknown<T>)
		{
			return [](const T& thing){return thing;};
		};

		template<typename T>
		auto getEvaluator(Variable<T>thing)
		{
			return [thing]()->const T&{return thing.value;};
		};

		template<typename T,auto Op>
		auto getEvaluator(UnaryOperator<T,Op>operation)
		{
			auto evaluator{getEvaluator(operation.operand)};
			{std::function fn(evaluator);}
			return [&evaluator]<typename R,typename ...Arg>(std::function<R(Arg...)>)mutable
			{
				return [evaluator](Arg...args)mutable
				{
					return Op(evaluator(std::forward<Arg>(args)...));
				};
			}(std::function(evaluator));
		};

		template<typename L,typename R,auto Op>
		auto getEvaluator(BinaryOperator<L,R,Op> operation)
		{
			auto LEval{getEvaluator(operation.lhs)};
			auto REval{getEvaluator(operation.rhs)};
			return [&LEval,&REval]<typename R1,typename ...Arg1>(std::function<R1(Arg1...)>)mutable
			{
				return [&LEval,&REval]<typename R2,typename ...Arg2>(std::function<R2(Arg2...)>)mutable
				{
					return [LEval,REval](Arg1...arg1,Arg2...arg2)mutable
					{
						return Op(LEval(arg1...),REval(arg2...));
					};
				}(std::function(REval));
			}(std::function(LEval));
		};

		template<typename F,typename ...Params>
		auto getEvaluator(Call<F,Params...>operation)
		{
			auto FEval{getEvaluator(operation.callee)};
			auto ArgEval{function_tuple_cat(get_evaluators(operation.arguments))};
			auto Eval{function_cat(FEval,ArgEval)};
			return [&Eval]<typename R,typename ...Args>(std::function<R(Args...)>)mutable
			{
				return [Eval](Args...args)mutable
				{
					return std::apply([](auto i,auto j){return std::apply(i,j);},Eval(args...));
				};
			}(std::function(Eval));
		};

		template<typename A,typename ...I>
		auto getEvaluator(Subscript<A,I...>operation)
		{
			auto AEval{getEvaluator(operation.array)};
			auto IEval{function_tuple_cat(get_evaluators(operation.indices))};
			auto Eval{function_cat(AEval,IEval)};
			return [&Eval]<typename R,typename ...Args>(std::function<R(Args...)>)mutable
			{
				return [Eval](Args...args)mutable
				{
					return std::apply
					(
						[](auto i,auto j)
						{
							return std::apply
							(
								[&i](auto ...args)
								{
									if constexpr(std::is_fundamental_v<decltype(i)>)
//										return i[args...];
//										static_assert(false);
;
									else
										return (i.operator[](args...));
								},
							j);
						},
					Eval(args...)
					);
				};
			}(std::function(Eval));
		};
	}
}