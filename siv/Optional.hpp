//------------------------------------------
//	Optional.hpp
//	Copyright (c) 2014 Reputeless
//	<reputeless@gmail.com>
//	Distributed under the MIT license.
//------------------------------------------

# pragma once
# include <type_traits>
# include <utility>
# include <new>
# include <cassert>
# include <initializer_list>
# include <stdexcept>
# include <functional>

# ifndef SIV_CPP11_IMPLEMENTED

#	ifndef SIV_ALIGNOF
#		define SIV_ALIGNOF(x) __alignof(x)
#	endif

#	ifndef SIV_ALIGNAS
#		define SIV_ALIGNAS(x) __declspec(align(x))
#	endif

#	define SIV_CONSTEXPR
#	define SIV_NOEXCEPT

# else

#	ifndef SIV_ALIGNOF
#		define SIV_ALIGNOF(x) alignof(x)
#	endif

#	ifndef SIV_ALIGNAS
#		define SIV_ALIGNAS(x) alignas(x)
#	endif

#	define SIV_CONSTEXPR constexpr
#	define SIV_NOEXCEPT noexcept

# endif

# define SIV_REQUIRES(R) static_assert(std::R::value==true, "");

namespace siv
{
	namespace detail
	{
		template<unsigned int Align>
		struct type_with_alignment {};

#define TYPE_WITH_ALIGNMENT_IMPL(A, Name)	\
		struct SIV_ALIGNAS(A) Name			\
		{							 		\
			char m[A]; 						\
			typedef Name type;				\
		};									\
		template<>							\
		struct type_with_alignment<A> : public Name {}

#define TYPE_WITH_ALIGNMENT(A) TYPE_WITH_ALIGNMENT_IMPL(A, aligned_with_##A)

		TYPE_WITH_ALIGNMENT(1);
		TYPE_WITH_ALIGNMENT(2);
		TYPE_WITH_ALIGNMENT(4);
		TYPE_WITH_ALIGNMENT(8);
		TYPE_WITH_ALIGNMENT(16);
		TYPE_WITH_ALIGNMENT(32);
		TYPE_WITH_ALIGNMENT(64);
		TYPE_WITH_ALIGNMENT(128);

#undef TYPE_WITH_ALIGNMENT
#undef TYPE_WITH_ALIGNMENT_IMPL

		template<typename T>
		class aligned_storage
		{
		private:

			union aligned_impl
			{
				char data[sizeof(T)];
				type_with_alignment<SIV_ALIGNOF(T)> dummy;
			};

			aligned_impl m_storage;

		public:

			const void* address() const
			{
				return static_cast<const void*>(&m_storage);
			}

			void* address()
			{
				return static_cast<void*>(&m_storage);
			}
		};
	}

	//
	//	In-place construction
	//
	struct in_place_t {};
	const SIV_CONSTEXPR in_place_t in_place{};

	//
	//	No-value state indicator
	//
	struct nullopt_helper {};
	typedef int nullopt_helper::* nullopt_t;
	const SIV_CONSTEXPR nullopt_t nullopt{};

	//
	//	Class bad_optional_access
	//
	class bad_optional_access : public std::logic_error
	{
	public:
		explicit bad_optional_access(const std::string& what_arg)
			: logic_error(what_arg) {}

		explicit bad_optional_access(const char* what_arg)
			: logic_error(what_arg) {}
	};

	//
	//	optional for object types
	//
	template<typename T>
	class optional
	{
	private:

		detail::aligned_storage<T> m_value;

		bool m_initialized = false;

	public:

		typedef typename std::remove_cv<T>::type cv_removed_type;
		static_assert(!std::is_same<cv_removed_type, nullopt_t>::value, "bad T");
		static_assert(!std::is_same<cv_removed_type, in_place_t>::value, "bad T");
		static_assert(!std::is_reference<T>::value, "the reference type is not supported");

		typedef optional<T> this_type;
		typedef T			value_type;
		typedef const T&	reference_const_type;
		typedef T&			reference_type;
		typedef const T*	pointer_const_type;
		typedef T*			pointer_type;
		typedef const T&	argument_type;
		typedef T&&			rvalue_reference_type;

		//
		//	Constructors 
		//
		SIV_CONSTEXPR optional() SIV_NOEXCEPT{}

		SIV_CONSTEXPR optional(nullopt_t) SIV_NOEXCEPT{}

		optional(const this_type& another)
		{
			SIV_REQUIRES(is_copy_constructible<value_type>);

			if (another)
			{
				construct(another.value());
			}
		}

		optional(this_type&& another) SIV_NOEXCEPT
		{
			SIV_REQUIRES(is_move_constructible<value_type>);

			if (another)
			{
				emplace(std::move(another.value()));
			}

			another = nullopt;
		}

		SIV_CONSTEXPR optional(const value_type& v)
		{
			SIV_REQUIRES(is_copy_constructible<value_type>);

			construct(v);
		}

		SIV_CONSTEXPR optional(rvalue_reference_type v)
		{
			SIV_REQUIRES(is_move_constructible<value_type>);

			emplace(v);
		}

		template <class... Args>
		SIV_CONSTEXPR explicit optional(in_place_t, Args&&... args)
		{
			static_assert(std::is_constructible<value_type, Args&&...>::value, "");

			destroy();

			::new (m_value.address()) value_type(std::forward<Args>(args)...);

			m_initialized = true;
		}

		template <class U, class... Args>
		SIV_CONSTEXPR explicit optional(in_place_t, std::initializer_list<U> ilist, Args&&... args)
		{
			destroy();

			::new (m_value.address()) value_type(ilist, std::forward<Args>(args)...);

			m_initialized = true;
		}

		//
		//	Destructor 
		//
		~optional()
		{
			destroy();
		}

		//
		//	Assignment 
		//
		this_type& operator=(nullopt_t) SIV_NOEXCEPT
		{
			destroy();

			return *this;
		}

		this_type& operator=(const this_type& another)
		{
			SIV_REQUIRES(is_copy_constructible<value_type>);
			SIV_REQUIRES(is_copy_assignable<value_type>);

			if		(m_initialized == true  && another.m_initialized == false) destroy();
			else if (m_initialized == false && another.m_initialized == true) construct(*another);
			else if (m_initialized == true  && another.m_initialized == true) m_value = another.m_value;
			return *this;
		}

		this_type& operator=(this_type&& another) SIV_NOEXCEPT
		{
			SIV_REQUIRES(is_move_constructible<value_type>);
			SIV_REQUIRES(is_move_assignable<value_type>);

			if		(m_initialized == true  && another.m_initialized == false) destroy();
			else if (another.m_initialized == true) construct(std::move(*another));
			return *this;
		}

		template <class U>
		this_type& operator=(U&& val)
		{
			static_assert(std::is_constructible<value_type, U>::value, "");
			static_assert(std::is_assignable<value_type&, U>::value, "");

			if (static_cast<bool>(*this))
			{
				value() = std::forward<U>(val);
			}
			else
			{
				emplace(std::forward<U>(val));
			}

			return *this;
		}

		template<typename... Args>
		void emplace(Args&&... args)
		{
			static_assert(std::is_constructible<value_type, Args&&...>::value, "");

			destroy();

			::new (m_value.address()) T(std::forward<Args>(args)...);

			m_initialized = true;
		}

		template <class U, class... Args>
		void emplace(std::initializer_list<U> ilist, Args&&... args)
		{
			static_assert(std::is_constructible<value_type, std::initializer_list<U>&, Args&&...>::value, "");

			destroy();

			::new (m_value.address()) T(ilist, std::forward<Args>(args)...);

			m_initialized = true;
		}

		//
		//	Swap
		//
		void swap(this_type& another) SIV_NOEXCEPT
		{
			SIV_REQUIRES(is_move_constructible<value_type>);

			this_type tmp(another);

			another = std::move(*this);

			*this = std::move(tmp);
		}

		//
		//	Observers 
		//
		SIV_CONSTEXPR pointer_const_type operator ->() const
		{
			return get_ptr();
		}

		pointer_type operator ->()
		{
			return get_ptr();
		}

		SIV_CONSTEXPR reference_const_type operator *() const
		{
			return *get_ptr();
		}

		reference_type operator *()
		{
			return *get_ptr();
		}

		SIV_CONSTEXPR explicit operator bool() const SIV_NOEXCEPT
		{
			return m_initialized;
		}

		SIV_CONSTEXPR reference_const_type value() const
		{
			if (!m_initialized)
			{
				throw bad_optional_access("bad access");
			}

			return *get_ptr();
		}

		reference_type value()
		{
			if (!m_initialized)
			{
				throw bad_optional_access("bad access");
			}

			return *get_ptr();
		}

		template <class U>
		SIV_CONSTEXPR value_type value_or(U&& v) const
#ifdef SIV_CPP11_IMPLEMENTED
			&
#endif
		{
			SIV_REQUIRES(is_copy_constructible<value_type>);
			static_assert(std::is_convertible<U&&, value_type>::value, "");

			if (static_cast<bool>(*this))
			{
				return **this;
			}

			return static_cast<value_type>(std::forward<U>(v));
		}

#ifdef SIV_CPP11_IMPLEMENTED
		template <class U>
		rvalue_reference_type value_or(U&& v) &&
		{
			SIV_REQUIRES(is_move_constructible<value_type>);
			static_assert(std::is_convertible<U&&, value_type>::value, "");

			if (static_cast<bool>(*this))
			{
				return std::move(**this);
			}

			return static_cast<value_type>(std::forward<U>(v));
		}
#endif

	private:

		void construct(reference_const_type v)
		{
			destroy();

			::new (m_value.address()) value_type(v);

			m_initialized = true;
		}

		void construct(rvalue_reference_type v)
		{
			destroy();

			::new (m_value.address()) value_type(std::forward<value_type>(v));

			m_initialized = true;
		}

		void destroy() SIV_NOEXCEPT
		{
			if (static_cast<bool>(*this))
			{
				get_ptr()->~T();
			}

			m_initialized = false;
		}

		pointer_const_type get_ptr() const
		{
			assert(static_cast<bool>(*this));

			return static_cast<pointer_const_type>(m_value.address());
		}

		pointer_type get_ptr()
		{
			assert(static_cast<bool>(*this));

			return static_cast<pointer_type>(m_value.address());
		}
	};

	//
	//	Relational operators
	//
	template <class T>
	SIV_CONSTEXPR bool operator==(const optional<T>& x, const optional<T>& y)
	{
		if (static_cast<bool>(x) != static_cast<bool>(y))
		{
			return false;
		}
		
		if (!x)
		{
			return true;
		}

		return *x == *y;
	}

	template <class T>
	SIV_CONSTEXPR bool operator!=(const optional<T>& x, const optional<T>& y)
	{
		return !(x == y);
	}

	template <class T>
	SIV_CONSTEXPR bool operator<(const optional<T>& x, const optional<T>& y)
	{
		if (!y)
		{
			return false;
		}
		else if (!x)
		{
			return true;
		}
		
		return *x < *y;
	}

	template <class T>
	SIV_CONSTEXPR bool operator>(const optional<T>& x, const optional<T>& y)
	{
		return y<x;
	}

	template <class T>
	SIV_CONSTEXPR bool operator<=(const optional<T>& x, const optional<T>& y)
	{
		return !(y < x);
	}

	template <class T>
	SIV_CONSTEXPR bool operator>=(const optional<T>& x, const optional<T>& y)
	{
		return !(x < y);
	}

	//
	//	Comparison with nullopt
	//
	template <class T>
	SIV_CONSTEXPR bool operator==(const optional<T>& x, nullopt_t) SIV_NOEXCEPT
	{
		return !x;
	}

	template <class T>
	SIV_CONSTEXPR bool operator==(nullopt_t, const optional<T>& x) SIV_NOEXCEPT
	{
		return !x;
	}

	template <class T>
	SIV_CONSTEXPR bool operator!=(const optional<T>& x, nullopt_t) SIV_NOEXCEPT
	{
		return static_cast<bool>(x);
	}

	template <class T>
	SIV_CONSTEXPR bool operator!=(nullopt_t, const optional<T>& x) SIV_NOEXCEPT
	{
		return static_cast<bool>(x);
	}

	template <class T>
	SIV_CONSTEXPR bool operator<(const optional<T>&, nullopt_t) SIV_NOEXCEPT
	{
		return false;
	}

	template <class T>
	SIV_CONSTEXPR bool operator<(nullopt_t, const optional<T>& x) SIV_NOEXCEPT
	{
		return static_cast<bool>(x);
	}

	template <class T>
	SIV_CONSTEXPR bool operator<=(const optional<T>& x, nullopt_t) SIV_NOEXCEPT
	{
		return !x;
	}

	template <class T>
	SIV_CONSTEXPR bool operator<=(nullopt_t, const optional<T>&) SIV_NOEXCEPT
	{
		return true;
	}

	template <class T>
	SIV_CONSTEXPR bool operator>(const optional<T>& x, nullopt_t) SIV_NOEXCEPT
	{
		return static_cast<bool>(x);
	}

	template <class T>
	SIV_CONSTEXPR bool operator>(nullopt_t, const optional<T>&) SIV_NOEXCEPT
	{
		return false;
	}

	template <class T>
	SIV_CONSTEXPR bool operator>=(const optional<T>&, nullopt_t) SIV_NOEXCEPT
	{
		return true;
	}

	template <class T>
	SIV_CONSTEXPR bool operator>=(nullopt_t, const optional<T>& x) SIV_NOEXCEPT
	{
		return !x;
	}

	//
	//	Comparison with T
	//
	template <class T>
	SIV_CONSTEXPR bool operator==(const optional<T>& x, const T& v)
	{
		return static_cast<bool>(x) ? (*x == v) : false;
	}

	template <class T>
	SIV_CONSTEXPR bool operator==(const T& v, const optional<T>& x)
	{
		return static_cast<bool>(x) ? (v == *x) : false;
	}

	template <class T>
	SIV_CONSTEXPR bool operator!=(const optional<T>& x, const T& v)
	{
		return static_cast<bool>(x) ? !(*x == v) : true;
	}

	template <class T>
	SIV_CONSTEXPR bool operator!=(const T& v, const optional<T>& x)
	{
		return static_cast<bool>(x) ? !(v == *x) : true;
	}

	template <class T>
	SIV_CONSTEXPR bool operator<(const optional<T>& x, const T& v)
	{
		return static_cast<bool>(x) ? (*x < v) : true;
	}

	template <class T>
	SIV_CONSTEXPR bool operator<(const T& v, const optional<T>& x)
	{
		return static_cast<bool>(x) ? (v < *x) : false;
	}

	template <class T>
	SIV_CONSTEXPR bool operator<=(const optional<T>& x, const T& v)
	{
		return !(x > v);
	}

	template <class T>
	SIV_CONSTEXPR bool operator<=(const T& v, const optional<T>& x)
	{
		return !(v > x);
	}
	
	template <class T>
	SIV_CONSTEXPR bool operator>(const optional<T>& x, const T& v)
	{
		return static_cast<bool>(x) ? (v< *x) : false;
	}
	
	template <class T>
	SIV_CONSTEXPR bool operator>(const T& v, const optional<T>& x)
	{
		return static_cast<bool>(x) ? (*x < v) : true;
	}

	template <class T>
	SIV_CONSTEXPR bool operator>=(const optional<T>& x, const T& v)
	{
		return !(x < v);
	}

	template <class T>
	SIV_CONSTEXPR bool operator>=(const T& v, const optional<T>& x)
	{
		return !(v < x);
	}

	//
	//	Specialized algorithms
	//
	template <class T>
	SIV_CONSTEXPR optional<typename std::decay<T>::type> make_optional(T&& v)
	{
		return optional<typename std::decay<T>::type>(std::forward<T>(v));
	}
}

namespace std
{
	//
	//	Specialized algorithms
	//
	template <class T>
	void swap(siv::optional<T>& left, siv::optional<T>& y)
#ifdef SIV_CPP11_IMPLEMENTED
		SIV_NOEXCEPT(SIV_NOEXCEPT(left.swap(y)))
#endif
	{
		left.swap(y);
	}

	//
	//	Hash support
	//
	template <class T>
	struct hash<siv::optional<T>>
	{
		std::size_t operator() (const siv::optional<T>& arg) const
		{
			if (arg)
			{
				return std::hash<T>{}(*arg);
			}

			return 0;
		}
	};
}

# undef SIV_REQUIRES
# undef SIV_ALIGNOF
# undef SIV_ALIGNAS

# ifndef SIV_CPP11_IMPLEMENTED
#	undef SIV_CONSTEXPR
#	undef SIV_NOEXCEPT
# endif
