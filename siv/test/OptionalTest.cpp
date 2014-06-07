//------------------------------------------
//	OptionalTest.cpp
//	Copyright (c) 2014 Reputeless
//	<reputeless@gmail.com>
//	Distributed under the MIT license.
//------------------------------------------

# include <iostream>
# include <cassert>
# include <vector>
# include <unordered_set>
# include <algorithm>
# include <string>
# include <siv/Optional.hpp>

// make_optional
void Test0()
{
	assert(siv::make_optional(0) == 0);
	assert(siv::make_optional(1) == 1);
	assert(siv::make_optional(0) != 1);
	assert(siv::make_optional(1) != 0);

	assert(siv::make_optional<std::vector<int>>({ 1, 2, 3 }) == std::vector<int>({ 1, 2, 3 }));
	assert(siv::make_optional<std::string>(std::string(5, 'a')) == std::string(5, 'a'));
}

// nullopt
void Test1()
{
	siv::optional<int> o1;
	siv::optional<int> o2{};
	siv::optional<int> o3{ siv::nullopt };

	assert(o1 == siv::nullopt);
	assert(o2 == siv::nullopt);
	assert(o3 == siv::nullopt);
	assert(!o1);
	assert(!o2);
	assert(!o3);
	assert(o1 == o2);
	assert(o2 == o3);
}

// mixed equality
void Test2()
{
	{
		siv::optional<int> oN;
		siv::optional<int> o0{ 0 };
		siv::optional<int> o1{ 1 };

		assert(o0 == 0);
		assert(0 == o0);
		assert(o1 == 1);
		assert(1 == o1);
		assert(o1 != 0);
		assert(0 != o1);
		assert(o0 != 1);
		assert(1 != o0);

		assert(!(0 == oN));
		assert(!(1 == oN));
		assert(!(oN == 0));
		assert(!(oN == 1));
		assert(0 != oN);
		assert(1 != oN);
		assert(oN != 0);
		assert(oN != 1);
	}

	{
		std::string one{ "one" }, two{ "two" };
		siv::optional<std::string> oN;
		siv::optional<std::string> oOne{ "one" };
		siv::optional<std::string> oTwo{ "two" };

		assert(oOne == one);
		assert(one == oOne);
		assert(oTwo == two);
		assert(two == oTwo);
		assert(oTwo != one);
		assert(one != oTwo);
		assert(oOne != two);
		assert(two != oOne);

		assert(!(one == oN));
		assert(!(two == oN));
		assert(!(oN == one));
		assert(!(oN == two));
		assert(one != oN);
		assert(two != oN);
		assert(oN != one);
		assert(oN != two);
	}
}

// mixed order
void Test3()
{
	siv::optional<int> oN;
	siv::optional<int> o0{ 0 };
	siv::optional<int> o1{ 1 };

	assert((oN < 0));
	assert((oN < 1));
	assert(!(o0 < 0));
	assert((o0 < 1));
	assert(!(o1 < 0));
	assert(!(o1 < 1));

	assert((oN <= 0));
	assert((oN <= 1));
	assert((o0 <= 0));
	assert((o0 <= 1));
	assert(!(o1 <= 0));
	assert((o1 <= 1));

	assert(!(oN > 0));
	assert(!(oN > 1));
	assert(!(o0 > 0));
	assert(!(o0 > 1));
	assert((o1 > 0));
	assert(!(o1 > 1));

	assert(!(oN >= 0));
	assert(!(oN >= 1));
	assert((o0 >= 0));
	assert(!(o0 >= 1));
	assert((o1 >= 0));
	assert((o1 >= 1));


	assert(!(0 < oN));
	assert(!(1 < oN));
	assert(!(0 < o0));
	assert(!(1 < o0));
	assert((0 < o1));
	assert(!(1 < o1));

	assert(!(0 <= oN));
	assert(!(1 <= oN));
	assert((0 <= o0));
	assert(!(1 <= o0));
	assert((0 <= o1));
	assert((1 <= o1));

	assert((0 > oN));
	assert((1 > oN));
	assert(!(0 > o0));
	assert((1 > o0));
	assert(!(0 > o1));
	assert(!(1 > o1));

	assert((0 >= oN));
	assert((1 >= oN));
	assert((0 >= o0));
	assert((1 >= o0));
	assert(!(0 >= o1));
	assert((1 >= o1));
}

// assignment
void Test4()
{
	siv::optional<int> oi;
	oi = siv::optional<int>{100};
	assert(*oi == 100);

	oi = siv::nullopt;
	assert(!oi);

	oi = 200;
	assert(*oi == 200);

	oi = {};
	assert(!oi);

	siv::optional<int> oj{ 300 };
	oi = std::move(oj);
	assert(oi == 300);

	siv::optional<int> ok{ std::move(oi) };
	assert(ok == 300);
}

// hashing
void Test5()
{
	std::hash<int> hi;
	std::hash<std::string> hs;
	std::hash<siv::optional<int>> hoi;
	std::hash<siv::optional<std::string>> hos;

	assert(hi(0) == hoi(siv::optional<int>{0}));
	assert(hi(1) == hoi(siv::optional<int>{1}));
	assert(hi(1234) == hoi(siv::optional<int>{1234}));

	assert(hs("") == hos(siv::optional<std::string>{""}));
	assert(hs("#") == hos(siv::optional<std::string>{"#"}));
	assert(hs("Siv3D") == hos(siv::optional<std::string>{"Siv3D"}));

	std::unordered_set<siv::optional<std::string>> set;
	assert(set.find({ "Siv3D" }) == set.end());

	set.insert({ "#" });
	assert(set.find({ "Siv3D" }) == set.end());

	set.insert({ "Siv3D" });
	assert(set.find({ "Siv3D" }) != set.end());
}

// value_or
void Test6()
{
	siv::optional<int> oi = 100;
	assert(oi.value_or(200) == 100);

	oi = siv::nullopt;
	assert(oi.value_or(300) == 300);

	siv::optional<std::string> os{ "AAA" };
	assert(os.value_or("BBB") == "AAA");

	os = {};
	assert(os.value_or("BBB") == "BBB");
}

// optional optional
void Test7()
{
	{
		siv::optional<siv::optional<int>> ooi;
		assert(ooi == siv::nullopt);
		assert(!ooi);
	}

	{
		siv::optional<siv::optional<int>> ooi{ siv::in_place };
		assert(ooi != siv::nullopt);
		assert(bool(ooi));
		assert(*ooi == siv::nullopt);
	}

	{
		siv::optional<siv::optional<int>> ooi{ siv::in_place, siv::nullopt };
		assert(ooi != siv::nullopt);
		assert(bool(ooi));
		assert(*ooi == siv::nullopt);
		assert(!*ooi);
	}

	{
		siv::optional<siv::optional<int>> ooi{ siv::optional<int>{} };
		assert(ooi != siv::nullopt);
		assert(bool(ooi));
		assert(*ooi == siv::nullopt);
		assert(!*ooi);
	}

	{
		siv::optional<int> oi;
		auto ooi = siv::make_optional(oi);
		static_assert(std::is_same<siv::optional<siv::optional<int>>, decltype(ooi)>::value, "");
	}
}

// const propagation
void Test8()
{
	siv::optional<int> o1{ 0 };
	static_assert(std::is_same<decltype(*o1), int&>::value, "");

	const siv::optional<int> o2{ 0 };
	static_assert(std::is_same<decltype(*o2), const int&>::value, "");

	siv::optional<const int> o3{ 0 };
	static_assert(std::is_same<decltype(*o3), const int&>::value, "");

	siv::optional<const int> o4{ 0 };
	static_assert(std::is_same<decltype(*o4), const int&>::value, "");
}

// bad_optional_access
void Test9()
{
	try
	{
		siv::optional<int> ovN;
		siv::optional<int> oi{ 1 };
		int& r = oi.value();
		assert(r == 1);

		try
		{
			ovN.value();
			assert(false);
		}
		catch (siv::bad_optional_access const&)
		{

		}
	}
	catch (...)
	{
		assert(false);
	}
}

// optional initialization
void Test10()
{
	std::string s = "Siv3D";
	siv::optional<std::string> o1{ s };
	siv::optional<std::string> o2 = s;
	siv::optional<std::string> o3{ "Siv3D" };
	siv::optional<std::string> o4 = std::string{ "Siv3D" };
}

// in_place
void Test11()
{
	struct Widget
	{
		int a = 0;

		int b = 0;

		Widget() = default;

		Widget(int _a, int _b) : a{ _a }, b{ _b } {}
	};

	{
		siv::optional<Widget> ow{ siv::in_place, 1, 2 };
		assert(ow);
		assert(ow->a == 1);
		assert(ow->b == 2);
	}

	{
		siv::optional<std::vector<int>> ov{ siv::in_place, { 10, 20, 30 } };
		assert(bool(ov));
		assert((*ov)[2] == 30);

		std::vector<int> expected = { 10, 20, 30 };
		assert(ov == expected);
	}
};

// emplace
void Test12()
{
	struct Widget
	{
		int a = 0;

		int b = 0;

		Widget() = default;

		Widget(int _a, int _b) : a{ _a }, b{ _b } {}
	};

	siv::optional<Widget> ow;
	ow.emplace(100, 200);
	assert(ow);
	assert(ow->a == 100);
	assert(ow->b == 200);
};

// swap
void Test13()
{
	siv::optional<int> oA;
	siv::optional<int> oB{ 100 };
	siv::optional<int> oC{ 200 };

	oA.swap(oB);
	assert(oA == 100);
	assert(!oB);

	std::swap(oB, oC);
	assert(oB == 200);
	assert(!oC);
};

// alignment
void Test14()
{
	struct Aa
	{
		double a;
		int b;
		char c;
	};

	struct Bb
	{
		__m128 a;
		double b;
	};

	static_assert(__alignof(siv::optional<char>) == __alignof(char), "");
	static_assert(__alignof(siv::optional<short>) == __alignof(short), "");
	static_assert(__alignof(siv::optional<int>) == __alignof(int), "");
	static_assert(__alignof(siv::optional<long long>) == __alignof(long long), "");
	static_assert(__alignof(siv::optional<float>) == __alignof(float), "");
	static_assert(__alignof(siv::optional<double>) == __alignof(double), "");
	static_assert(__alignof(siv::optional<__m128>) == __alignof(__m128), "");
	static_assert(__alignof(siv::optional<Aa>) == __alignof(Aa), "");
	static_assert(__alignof(siv::optional<Bb>) == __alignof(Bb), "");
}

int main()
{
	{
		siv::optional<int> oi;

		assert(!oi);

		oi = 100;

		assert(oi);

		if (oi)
		{
			std::cout << *oi << '\n';
		}
	}

	{
		siv::optional<int> oi = 200;

		assert(oi);

		if (oi)
		{
			std::cout << oi.value() << '\n';
		}

		oi = siv::nullopt;

		assert(!oi);
	}

	{
		siv::optional<int> oi = siv::nullopt;

		assert(!oi);

		std::cout << oi.value_or(300) << '\n';
	}

	{
		siv::optional<int> oi = 300;

		assert(oi == 300);

		assert(oi != 999);

		assert(oi > 100);

		assert(oi != siv::nullopt);

		assert(oi > siv::nullopt);
	}

	Test0();

	Test1();

	Test2();

	Test3();

	Test4();

	Test5();

	Test6();

	Test7();

	Test8();

	Test9();

	Test10();

	Test11();

	Test12();

	Test13();

	Test14();
}
