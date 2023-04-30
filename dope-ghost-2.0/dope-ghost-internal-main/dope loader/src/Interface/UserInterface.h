#pragma once

/*@TODO: "dynamic" way of drawing gui modules etc..
		 an actual object oriented way of doing the gui,
*/

class UserInterface
{
public:
	UserInterface(float width, float height)
		: m_Width(width), m_Height(height)
	{
	}

	void Display();

private:
	float m_Width;
	float m_Height;
};