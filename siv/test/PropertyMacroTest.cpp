//------------------------------------------
//	PropertyMacroTest.cpp
//	Copyright (c) 2014 Reputeless
//	<reputeless@gmail.com>
//	Distributed under the MIT license.
//------------------------------------------

# include <iostream>
# include <cassert>
# include <siv/PropertyMacro.hpp>

class Size
{
private:

	int m_width = 0, m_height = 0;

public:

	void setWidth(int width)
	{
		m_width = width;
	}

	void setHeight(int height)
	{
		m_height = height;
	}

	Property_Get(int, width) const
	{ 
		return m_width;
	}

	Property_Get(int, height) const
	{
		return m_height;
	}
};

static_assert(sizeof(Size) == 8, "sizeof(Size) != 8");

int main()
{
	Size s;

	assert(s.width == 0);

	assert(s.height == 0);

	s.setWidth(100);

	s.setHeight(200);

	assert(s.width == 100);

	assert(s.height == 200);

	s.setWidth(s.width * 2);

	s.setHeight(s.height * 2);

	assert(s.width == 200);

	assert(s.height == 400);
}
