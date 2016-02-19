#include "Movement.h"
#include "Application.h"

void charMovement(bool& b_reverse, float f_speed, float& f_rotate, double f_timer)
{
	if (Application::IsKeyPressed('W') || Application::IsKeyPressed('A') || Application::IsKeyPressed('S') || Application::IsKeyPressed('D'))
	{
		if (f_rotate < 3.f && b_reverse == false)
		{
			f_rotate += (float)(f_speed * f_timer);
		}
		else
		{
			b_reverse = true;
		}
		if (f_rotate > -3.f && b_reverse == true)
		{
			f_rotate -= (float)(f_speed * f_timer);
		}
		else
		{
			b_reverse = false;
		}
	}
	else
	{
		if (f_rotate > 0.f)
			f_rotate -= (float)(f_speed * f_timer);
		else if (f_rotate < 0.f)
			f_rotate += (float)(f_speed * f_timer);
		if (f_rotate < .5f && f_rotate > -.5f)
			f_rotate = 0.f;
	}
}