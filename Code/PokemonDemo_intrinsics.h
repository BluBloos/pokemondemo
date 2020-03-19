#define Pi32 3.14159265359f

#include <math.h>

inline int FloatToInt(float Value)
{
	//TODO: Intrisnic
	//apparently there is a lot more complexity than just this.
	int Result = (int)(Value + 0.5f);
	if (Value < 0.0f) { Result = (int)(Value - 0.5f); }
	return Result;
}

inline int SquareInt(int Value)
{
	return (Value * Value);
}

inline float FloorFloat(float Value)
{
	return floorf(Value);
}

inline signed int FloorFloatToInt(float Value)
{
	return ( (int) floorf(Value) );
}

inline float Sin(float Angle)
{
	return sinf(Angle);
}

inline float Pow(float base, float power)
{
	return powf(base, power);
}

inline float Abs(float Value)
{
	float Result = (Value < 0.0f)? Value * -1.0f:Value;
	return Result;
}

vector2f Lerp2f(vector2f v1, vector2f v2, float a)
{
	vector2f Result = {};
	Result.X = (1.0f - a) * v1.X + v2.X * a;
	Result.Y = (1.0f - a) * v1.Y + v2.Y * a;
	return Result;
}