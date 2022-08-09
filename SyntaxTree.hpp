//
// Created by Administrator on 2022-08-08.
//
#include <cstddef>
#include <utility>
#include <tuple>
#include <functional>

namespace reverSilly
{
	namespace LambdaIt
	{
		namespace
		{
			template<typename T>
			struct function_traits;

			template<typename R, typename ...Args>
			struct function_traits<std::function<R(Args...)>>
			{
				static constexpr size_t nargs=sizeof...(Args);
				using result_type=R;
				// 输入参数类型,i为从0开始的参数类型索引
				template <size_t i>
				struct arg
				{using type=typename std::tuple_element<i,std::tuple<Args...>>::type;};
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
				struct arg
				{
					using type=typename std::tuple_element<i,std::tuple<Args...>>::type;
				};
			};

			template<typename T,std::size_t...indices>
			auto get_first_n_impl(T&& thing,std::index_sequence<indices...>)
			{
				return std::tuple(std::get<indices>(std::forward<T>(thing))...);
			}

			template<std::size_t index,typename ...Args>
			auto get_first_n(Args&&...args)
			{
				return get_first_n_impl(std::forward_as_tuple(args...),std::make_index_sequence<index>{});
			}

			template<std::size_t n,typename T,std::size_t...indices>
			auto get_last_n_impl(T&& thing,std::index_sequence<indices...>)
			{
				return std::tuple(std::get<sizeof...(indices)-n+indices>(std::forward<T>(thing))...);
			}

			template<std::size_t index,typename ...Args>
			auto get_last_n(Args&&...args)
			{
				return get_last_n_impl<sizeof...(Args)>(std::forward_as_tuple(args...),std::make_index_sequence<index>{});
			}

			/**
			 * applies an member function with parameters in a tuple
			 */
//			template<typename T,typename R,typename ...Args,std::size_t ...indices>
//			R apply_impl(R(T::)(Args...),,std::index_sequence<indices...>)
//			{
//
//			};

//			template<typename F,typename T,typename ...Args>
//			struct apply_impl;
//			template<typename T,typename R,typename ...Args>
//			struct apply_impl<R(T::*)(Args...),T,Args...>
//			{
//				static R apply(R(T::*mem)(Args...),T* object,std::tuple<Args&&...>&&param)
//				{
//					return std::apply(std::mem_fn(mem),std::tuple_cat(std::tuple(object,param)));
//				}
//			};
//			template<typename T,typename R,typename ...Args>
//			struct apply_impl<R(T::*)(Args...)const,T,Args...>
//			{
//				static R apply(R(T::*mem)(Args...)const,T* object,std::tuple<Args&&...>&&param)
//				{
//					return std::apply(std::mem_fn(mem),std::tuple_cat(std::tuple(object,param)));
//				}
//			};
//
//			template<typename F,typename T,typename ...Args>
//			auto apply(F mem,T* object,std::tuple<Args...>&&param)
//			{
//				return apply_impl<F,T,Args...>::apply(mem,object,param);
//			};
		}

		template<typename T>
		concept IsExpression=std::is_member_function_pointer_v<decltype(&T::evaluate)>;
		
		template<typename L,typename R>
		struct Assign;
		template<typename T>
		struct IndirectAccess;
		
		template<typename Me>
		struct Expression
		{
			template<typename T>
			Assign<Me,T> operator=(const T &that);

			IndirectAccess<Me> operator->();
		};

		template<typename T>
		struct Variable:public Expression<Variable<T>>
		{
			T& evaluate()
			{
				return value;
			}
			operator T&()
			{
				return value;
			}
			[[no_unique_address]]T value;
		public:
			template<typename ...Args>
			constexpr Variable(Args&&...args):value{std::forward<Args>(args)...}{}
		};
		template<typename T>
		struct Unknown:public Expression<Unknown<T>>
		{
			T evaluate(T thing) const
			{
//				value=&thing;
//				return *value;
				return std::forward<T>(thing);
			}
//			T& evaluate()
//			{
//				return *value;
//			}
//			operator T&()
//			{
//				return *value;
//			}
//			T* value;
		};

		template<typename T>
		Unknown<T> it;

		template<typename Me,typename T,auto Op>
		struct UnaryOperator:public Expression<Me>
		{
			[[no_unique_address]]T operand;
			constexpr explicit UnaryOperator(const T&thing):operand{thing}{}
			template<typename ...Args>
			auto evaluate(Args&&...args)
			{
//				static_assert(IsExpression<UnaryOperator>);
				static_assert(IsExpression<T>);
				return Op(operand.evaluate(std::forward<Args>(args)...));
			}
		};
		
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

		template<typename T>
		struct Flip:public UnaryOperator<Flip<T>,T,OperationFunctors::Flip>
		{
			constexpr explicit Flip(const T &thing):UnaryOperator<Flip<T>,T,OperationFunctors::Flip>(thing){}
		};
		template<typename T>
		auto operator~(const T&thing){return Flip{thing};}
		
		template<typename T>
		struct Not:public UnaryOperator<Not<T>,T,OperationFunctors::Not>
		{
			constexpr explicit Not(const T &thing):UnaryOperator<Not<T>,T,OperationFunctors::Not>(thing){}
		};
		template<typename T>
		Not<T> operator!(const T&thing){return Not{thing};}
		
		template<typename T>
		struct AddressOf:public UnaryOperator<AddressOf<T>,T,OperationFunctors::AddressOf>
		{
			constexpr explicit AddressOf(const T &thing):UnaryOperator<AddressOf<T>,T,OperationFunctors::AddressOf>(thing){}
		};
		template<typename T>
		AddressOf<T> operator&(const T&thing){return AddressOf{thing};}
		
		template<typename T>
		struct Positive:public UnaryOperator<Positive<T>,T,OperationFunctors::Positive>
		{
			constexpr explicit Positive(const T &thing):UnaryOperator<Positive<T>,T,OperationFunctors::Positive>(thing){}
		};
		template<typename T>
		Positive<T> operator+(const T&thing){return Positive{thing};}
		
		template<typename T>
		struct Negative:public UnaryOperator<Negative<T>,T,OperationFunctors::Negative>
		{
			constexpr explicit Negative(const T &thing):UnaryOperator<Negative<T>,T,OperationFunctors::Negative>(thing){}
		};
		template<typename T>
		Negative<T> operator-(const T&thing){return Negative{thing};}
		
		template<typename T>
		struct Dereference:public UnaryOperator<Dereference<T>,T,OperationFunctors::Dereference>
		{
			constexpr explicit Dereference(const T &thing):UnaryOperator<Dereference<T>,T,OperationFunctors::Dereference>(thing){}
		};
		template<typename T>
		Dereference<T> operator*(const T&thing){return Dereference{thing};}
		
		template<typename T>
		struct PrefixIncrease:public UnaryOperator<PrefixIncrease<T>,T,OperationFunctors::PrefixIncrease>
		{
			constexpr explicit PrefixIncrease(const T &thing):UnaryOperator<PrefixIncrease<T>,T,OperationFunctors::PrefixIncrease>(thing){}
		};
		template<typename T>
		auto operator++(const T&thing,int){return PrefixIncrease{thing};}
		
		template<typename T>
		struct PostfixIncrease:public UnaryOperator<PostfixIncrease<T>,T,OperationFunctors::PostfixIncrease>
		{
			constexpr explicit PostfixIncrease(const T &thing):UnaryOperator<PostfixIncrease<T>,T,OperationFunctors::PostfixIncrease>(thing){}
		};
		template<typename T>
		auto operator++(const T&thing){return PostfixIncrease{thing};}
		
		template<typename T>
		struct PrefixDecrease:public UnaryOperator<PrefixDecrease<T>,T,OperationFunctors::PrefixDecrease>
		{
			constexpr explicit PrefixDecrease(const T &thing):UnaryOperator<PrefixDecrease<T>,T,OperationFunctors::PrefixDecrease>(thing){}
		};
		template<typename T>
		auto operator--(const T&thing,int){return PrefixDecrease{thing};}
		
		template<typename T>
		struct PostfixDecrease:public UnaryOperator<PostfixDecrease<T>,T,OperationFunctors::PostfixDecrease>
		{
			constexpr explicit PostfixDecrease(const T &thing):UnaryOperator<PostfixDecrease<T>,T,OperationFunctors::PostfixDecrease>(thing){}
		};
		template<typename T>
		auto operator--(const T&thing){return PostfixDecrease{thing};}
		
		template<typename T>
		struct IndirectAccess:public UnaryOperator<IndirectAccess<T>,T,OperationFunctors::IndirectAccess>
		{
			constexpr explicit IndirectAccess(const T &thing):UnaryOperator<IndirectAccess<T>,T,OperationFunctors::IndirectAccess>(thing){}
		};
		template<typename Me>
		IndirectAccess<Me> Expression<Me>::operator->()
		{
			return IndirectAccess<Me>{*this};
		}
		

		template<typename Me,typename L,typename R,auto Op>
		struct BinaryOperator:public Expression<Me>
		{
			[[no_unique_address]]L lhs;
			[[no_unique_address]]R rhs;
			constexpr BinaryOperator(const L& lhs,const R&rhs):lhs{lhs},rhs{rhs}{}
			template<typename ...Args>
			auto evaluate(Args&&...args)
			{
//				static_assert(IsExpression<BinaryOperator>);
				static_assert(IsExpression<L>);
				static_assert(IsExpression<R>);
				constexpr size_t lparamn{function_traits<decltype(L::evaluate)>::nargs},rparamn{function_traits<decltype(R::evaluate)>::nargs};
				static_assert(lparamn+rparamn==sizeof...(Args));
				auto l{[this](auto &&...args){return lhs.evaluate(std::forward<decltype(args)>(args)...);}};
				auto r{[this](auto &&...args){return rhs.evaluate(std::forward<decltype(args)>(args)...);}};
				return Op(std::apply(l,get_first_n<lparamn>(std::forward<Args>(args)...)),
			std::apply(r,get_last_n<rparamn>(std::forward<Args>(args)...)));
			}
		};
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
		template<typename L,typename R>
		struct LessThan:public BinaryOperator<LessThan<L,R>,L,R,OperationFunctors::LessThan>
		{
		public:
			constexpr LessThan(const L&lhs,const R&rhs):BinaryOperator<LessThan<L,R>,L,R,OperationFunctors::LessThan>(lhs,rhs){}
		};
		template<typename L,typename R>
		auto operator<(const L&lhs,const R&rhs){return LessThan(lhs,rhs);}
		
		template<typename L,typename R>
		struct GreaterThan:public BinaryOperator<GreaterThan<L,R>,L,R,OperationFunctors::GreaterThan>
		{
			constexpr GreaterThan(const L &lhs,const R &rhs):BinaryOperator<GreaterThan<L,R>,L,R,OperationFunctors::GreaterThan>{lhs,rhs}{}
		};
		template<typename L,typename R>
		auto operator>(const L&lhs,const R&rhs){return GreaterThan(lhs,rhs);}
		template<typename L,typename R>
		struct GreaterEqual:public BinaryOperator<GreaterEqual<L,R>,L,R,OperationFunctors::GreaterEqual>
		{
			constexpr GreaterEqual(const L &lhs,const R &rhs):BinaryOperator<GreaterEqual<L,R>,L,R,OperationFunctors::GreaterEqual>{lhs,rhs}{}
		};
		template<typename L,typename R>
		auto operator>=(const L&lhs,const R&rhs){return GreaterEqual(lhs,rhs);}
		
		template<typename L,typename R>
		struct LessEqual:public BinaryOperator<LessEqual<L,R>,L,R,OperationFunctors::LessEqual>
		{
			constexpr LessEqual(const L &lhs,const R &rhs):BinaryOperator<LessEqual<L,R>,L,R,OperationFunctors::LessEqual>{lhs,rhs}{}
		};
		template<typename L,typename R>
		auto operator<=(const L&lhs,const R&rhs){return LessEqual(lhs,rhs);}
		template<typename L,typename R>
		struct EqualTo:public BinaryOperator<EqualTo<L,R>,L,R,OperationFunctors::EqualTo>
		{
			constexpr EqualTo(const L &lhs,const R &rhs):BinaryOperator<EqualTo<L,R>,L,R,OperationFunctors::EqualTo>{lhs,rhs}{}
		};
		template<typename L,typename R>
		auto operator==(const L&lhs,const R&rhs){return EqualTo(lhs,rhs);}
		
		template<typename L,typename R>
		struct NotEqualTo:public BinaryOperator<NotEqualTo<L,R>,L,R,OperationFunctors::NotEqualTo>
		{
			constexpr NotEqualTo(const L &lhs,const R &rhs):BinaryOperator<NotEqualTo<L,R>,L,R,OperationFunctors::NotEqualTo>{lhs,rhs}{}
		};
		template<typename L,typename R>
		auto operator!=(const L&lhs,const R&rhs){return NotEqualTo(lhs,rhs);}
		
		template<typename L,typename R>
		struct CompareTo:public BinaryOperator<CompareTo<L,R>,L,R,OperationFunctors::CompareTo>
		{
			constexpr CompareTo(const L &lhs,const R &rhs):BinaryOperator<CompareTo<L,R>,L,R,OperationFunctors::CompareTo>{lhs,rhs}{}
		};
		template<typename L,typename R>
		auto operator<=>(const L&lhs,const R&rhs){return CompareTo(lhs,rhs);}

		template<typename L,typename R>
		struct Plus:public BinaryOperator<Plus<L,R>,L,R,OperationFunctors::Plus>
		{
			constexpr Plus(const L&lhs,const R&rhs):BinaryOperator<Plus<L,R>,L,R,OperationFunctors::Plus>(lhs,rhs){}
		};
		template<typename L,typename R>
		auto operator+(const L&lhs,const R&rhs){return Plus(lhs,rhs);}
		
		template<typename L,typename R>
		struct Minus:public BinaryOperator<Minus<L,R>,L,R,OperationFunctors::Minus>
		{
			constexpr Minus(const L &lhs,const R &rhs):BinaryOperator<Minus<L,R>,L,R,OperationFunctors::Minus>{lhs,rhs}{}
		};
		template<typename L,typename R>
		auto operator-(const L&lhs,const R&rhs){return Minus(lhs,rhs);}
		
		template<typename L,typename R>
		struct Multiply:public BinaryOperator<Multiply<L,R>,L,R,OperationFunctors::Multiply>
		{
			constexpr Multiply(const L &lhs,const R &rhs):BinaryOperator<Multiply<L,R>,L,R,OperationFunctors::Multiply>(lhs,rhs){}
		};
		template<typename L,typename R>
		auto operator*(const L&lhs,const R&rhs){return Multiply(lhs,rhs);}
		
		template<typename L,typename R>
		struct Divide:public BinaryOperator<Divide<L,R>,L,R,OperationFunctors::Divide>
		{
			constexpr Divide(const L &lhs,const R &rhs):BinaryOperator<Divide<L,R>,L,R,OperationFunctors::Divide>{lhs,rhs}{}
		};
		template<typename L,typename R>
		auto operator/(const L&lhs,const R&rhs){return Divide(lhs,rhs);}
		
		template<typename L,typename R>
		struct Modular:public BinaryOperator<Modular<L,R>,L,R,OperationFunctors::Modular>
		{
			constexpr Modular(const L &lhs,const R &rhs):BinaryOperator<Modular<L,R>,L,R,OperationFunctors::Modular>{lhs,rhs}{}
		};
		template<typename L,typename R>
		auto operator%(const L&lhs,const R&rhs){return Modular(lhs,rhs);}
		
		template<typename L,typename R>
		struct ExclusiveOr:public BinaryOperator<ExclusiveOr<L,R>,L,R,OperationFunctors::ExclusiveOr>
		{
			constexpr ExclusiveOr(const L &lhs,const R &rhs):BinaryOperator<ExclusiveOr<L,R>,L,R,OperationFunctors::ExclusiveOr>{lhs,rhs}{}
		};
		template<typename L,typename R>
		auto operator^(const L&lhs,const R&rhs){return ExclusiveOr(lhs,rhs);}
		
		template<typename L,typename R>
		struct And:public BinaryOperator<And<L,R>,L,R,OperationFunctors::And>
		{
			constexpr And(const L &lhs,const R &rhs):BinaryOperator<And<L,R>,L,R,OperationFunctors::And>{lhs,rhs}{}
		};
		template<typename L,typename R>
		auto operator&(const L&lhs,const R&rhs){return And(lhs,rhs);}
		
		template<typename L,typename R>
		struct Or:public BinaryOperator<Or<L,R>,L,R,OperationFunctors::Or>
		{
			constexpr Or(const L &lhs,const R &rhs):BinaryOperator<Or<L,R>,L,R,OperationFunctors::Or>{lhs,rhs}{}
		};
		template<typename L,typename R>
		auto operator|(const L&lhs,const R&rhs){return Or(lhs,rhs);}
		
		template<typename L,typename R>
		struct Assign:public BinaryOperator<Assign<L,R>,L,R,OperationFunctors::Assign>
		{
			constexpr Assign(const L &lhs,const R &rhs):BinaryOperator<Assign<L,R>,L,R,OperationFunctors::Assign>{lhs,rhs}{}
		};
		template<typename Me>
		template<typename T>
		Assign<Me,T> Expression<Me>::operator=(const T &that)
		{
			return Assign<Me,T>{*static_cast<Me*>(this),that};
		}
		
		template<typename L,typename R>
		struct PlusBy:public BinaryOperator<PlusBy<L,R>,L,R,OperationFunctors::PlusBy>
		{
			constexpr PlusBy(const L &lhs,const R &rhs):BinaryOperator<PlusBy<L,R>,L,R,OperationFunctors::PlusBy>{lhs,rhs}{}
		};
		template<typename L,typename R>
		auto operator+=(const L&lhs,const R&rhs){return PlusBy(lhs,rhs);}
		
		template<typename L,typename R>
		struct MinusBy:public BinaryOperator<MinusBy<L,R>,L,R,OperationFunctors::MinusBy>
		{
			constexpr MinusBy(const L &lhs,const R &rhs):BinaryOperator<MinusBy<L,R>,L,R,OperationFunctors::MinusBy>{lhs,rhs}{}
		};
		template<typename L,typename R>
		auto operator-=(const L&lhs,const R&rhs){return MinusBy(lhs,rhs);}
		
		template<typename L,typename R>
		struct MultiplyBy:public BinaryOperator<MultiplyBy<L,R>,L,R,OperationFunctors::MultiplyBy>
		{
			constexpr MultiplyBy(const L &lhs,const R &rhs):BinaryOperator<MultiplyBy<L,R>,L,R,OperationFunctors::MultiplyBy>{lhs,rhs}{}
		};
		template<typename L,typename R>
		auto operator*=(const L&lhs,const R&rhs){return MultiplyBy(lhs,rhs);}
		
		template<typename L,typename R>
		struct DividedBy:public BinaryOperator<DividedBy<L,R>,L,R,OperationFunctors::DividedBy>
		{
			constexpr DividedBy(const L &lhs,const R &rhs):BinaryOperator<DividedBy<L,R>,L,R,OperationFunctors::DividedBy>{lhs,rhs}{}
		};
		template<typename L,typename R>
		auto operator/=(const L&lhs,const R&rhs){return DividedBy(lhs,rhs);}
		
		template<typename L,typename R>
		struct ModularBy:public BinaryOperator<ModularBy<L,R>,L,R,OperationFunctors::ModularBy>
		{
			constexpr ModularBy(const L &lhs,const R &rhs):BinaryOperator<ModularBy<L,R>,L,R,OperationFunctors::ModularBy>{lhs,rhs}{}
		};
		template<typename L,typename R>
		auto operator%=(const L&lhs,const R&rhs){return ModularBy(lhs,rhs);}
		
		template<typename L,typename R>
		struct ExclusiveOrBy:public BinaryOperator<ExclusiveOrBy<L,R>,L,R,OperationFunctors::ExclusiveOrBy>
		{
			constexpr ExclusiveOrBy(const L &lhs,const R &rhs):BinaryOperator<ExclusiveOrBy<L,R>,L,R,OperationFunctors::ExclusiveOrBy>{lhs,rhs}{}
		};
		template<typename L,typename R>
		auto operator^=(const L&lhs,const R&rhs){return ExclusiveOrBy(lhs,rhs);}
		
		template<typename L,typename R>
		struct AndBy:public BinaryOperator<AndBy<L,R>,L,R,OperationFunctors::AndBy>
		{
			constexpr AndBy(const L &lhs,const R &rhs):BinaryOperator<AndBy<L,R>,L,R,OperationFunctors::AndBy>{lhs,rhs}{}
		};
		template<typename L,typename R>
		auto operator&=(const L&lhs,const R&rhs){return AndBy(lhs,rhs);}
		
		template<typename L,typename R>
		struct OrBy:public BinaryOperator<OrBy<L,R>,L,R,OperationFunctors::OrBy>
		{
			constexpr OrBy(const L &lhs,const R &rhs):BinaryOperator<OrBy<L,R>,L,R,OperationFunctors::OrBy>{lhs,rhs}{}
		};
		template<typename L,typename R>
		auto operator|=(const L&lhs,const R&rhs){return OrBy(lhs,rhs);}
		
		template<typename L,typename R>
		struct LeftShift:public BinaryOperator<LeftShift<L,R>,L,R,OperationFunctors::LeftShift>
		{
			constexpr LeftShift(const L &lhs,const R &rhs):BinaryOperator<LeftShift<L,R>,L,R,OperationFunctors::LeftShift>{lhs,rhs}{}
		};
		template<typename L,typename R>
		auto operator<<(const L&lhs,const R&rhs){return LeftShift(lhs,rhs);}
		
		template<typename L,typename R>
		struct RightShift:public BinaryOperator<RightShift<L,R>,L,R,OperationFunctors::RightShift>
		{
			constexpr RightShift(const L &lhs,const R &rhs):BinaryOperator<RightShift<L,R>,L,R,OperationFunctors::RightShift>{lhs,rhs}{}
		};
		template<typename L,typename R>
		auto operator>>(const L&lhs,const R&rhs){return RightShift(lhs,rhs);}
		
		template<typename L,typename R>
		struct RightShiftBy:public BinaryOperator<RightShiftBy<L,R>,L,R,OperationFunctors::RightShiftBy>
		{
			constexpr RightShiftBy(const L &lhs,const R &rhs):BinaryOperator<RightShiftBy<L,R>,L,R,OperationFunctors::RightShiftBy>{lhs,rhs}{}
		};
		template<typename L,typename R>
		auto operator>>=(const L&lhs,const R&rhs){return RightShiftBy(lhs,rhs);}
		
		template<typename L,typename R>
		struct LeftShiftBy:public BinaryOperator<LeftShiftBy<L,R>,L,R,OperationFunctors::LeftShiftBy>
		{
			constexpr LeftShiftBy(const L &lhs,const R &rhs):BinaryOperator<LeftShiftBy<L,R>,L,R,OperationFunctors::LeftShiftBy>{lhs,rhs}{}
		};
		template<typename L,typename R>
		auto operator<<=(const L&lhs,const R&rhs){return LeftShiftBy(lhs,rhs);}
		
		template<typename L,typename R>
		struct ShortAnd:public BinaryOperator<ShortAnd<L,R>,L,R,OperationFunctors::ShortAnd>
		{
			constexpr ShortAnd(const L &lhs,const R &rhs):BinaryOperator<ShortAnd<L,R>,L,R,OperationFunctors::ShortAnd>{lhs,rhs}{}
		};
		template<typename L,typename R>
		auto operator&&(const L&lhs,const R&rhs){return ShortAnd(lhs,rhs);}
		
		template<typename L,typename R>
		struct ShortOr:public BinaryOperator<ShortOr<L,R>,L,R,OperationFunctors::ShortOr>
		{
			constexpr ShortOr(const L &lhs,const R &rhs):BinaryOperator<ShortOr<L,R>,L,R,OperationFunctors::ShortOr>{lhs,rhs}{}
		};
		template<typename L,typename R>
		auto operator||(const L&lhs,const R&rhs){return ShortOr(lhs,rhs);}
		
		template<typename L,typename R>
		struct Comma:public BinaryOperator<Comma<L,R>,L,R,OperationFunctors::Comma>
		{
			constexpr Comma(const L &lhs,const R &rhs):BinaryOperator<Comma<L,R>,L,R,OperationFunctors::Comma>{lhs,rhs}{}
		};
		template<typename L,typename R>
		auto operator,(const L&lhs,const R&rhs){return Comma(lhs,rhs);}

		template<typename L,typename R>
		struct DereferenceIndirectAccess:public BinaryOperator<DereferenceIndirectAccess<L,R>,L,R,OperationFunctors::DereferenceIndirectAccess>
		{
			constexpr DereferenceIndirectAccess(const L &lhs,const R &rhs):BinaryOperator<DereferenceIndirectAccess<L,R>,L,R,OperationFunctors::DereferenceIndirectAccess>{lhs,rhs}{}
		};
		template<typename L,typename R>
		auto operator->*(const L&lhs,const R&rhs){return DereferenceIndirectAccess(lhs,rhs);}
	}
}