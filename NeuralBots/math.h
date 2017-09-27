// Impulse Engine Math : IEMat
#ifndef MASSLIB_H
#define MASSLIB_H

#include <cmath> // abs, sqrt
#include <cassert> // assert
#include <algorithm> // max, min

typedef unsigned short Uint16;
typedef unsigned int Uint32;

#define M_PI 3.14159265358979323846
#define M_2PI 6.28318530717958647692
#define M_PI2 1.57079632679489661923
#define EPSILON 0.00000000001f

struct Vector2D
{
	union
	{
		float m[1][1];
		float v[2];

		struct
		{
			float x;
			float y;
		};
	};


	Vector2D()
	{ }


	Vector2D(float x_, float y_)
		: x(x_)
		, y(y_)
	{ }


	void Set(float x_, float y_)
	{
		x = x_;
		y = y_;
	}


	Vector2D operator-(void) const
	{
		return Vector2D(-x, -y);
	}


	Vector2D operator*(float s) const
	{
		return Vector2D(x * s, y * s);
	}


	Vector2D operator/(float s) const
	{
		return Vector2D(x / s, y / s);
	}


	void operator*=(float s)
	{
		x *= s;
		y *= s;
	}


	void operator/=(float s)
	{
		x /= s;
		y /= s;
	}


	bool operator==(const Vector2D& rhs)
	{
		return x == rhs.x && y == rhs.y;
	}


	bool operator!=(const Vector2D& rhs)
	{
		return !(x == rhs.x && y == rhs.y);
	}


	Vector2D operator+(const Vector2D& rhs) const
	{
		return Vector2D(x + rhs.x, y + rhs.y);
	}


	Vector2D operator*(const Vector2D& rhs) const
	{
		return Vector2D(x * rhs.x, y * rhs.y);
	}


	Vector2D operator+(float s) const
	{
		return Vector2D(x + s, y + s);
	}


	void operator+=(const Vector2D& rhs)
	{
		x += rhs.x;
		y += rhs.y;
	}


	Vector2D operator-(const Vector2D& rhs) const
	{
		return Vector2D(x - rhs.x, y - rhs.y);
	}


	void operator-=(const Vector2D& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
	}


	float LenSqr(void) const
	{
		return x * x + y * y;
	}


	float Distance(const Vector2D& rhs) const
	{
		return std::sqrt(std::pow(rhs.x - x, 2) + std::pow(rhs.y - y, 2));
	}


	float Len(void) const
	{
		return std::sqrt(x * x + y * y);
	}


	void Rotate(float radians)
	{
		float c = std::cos(radians);
		float s = std::sin(radians);

		float xp = x * c - y * s;
		float yp = x * s + y * c;

		x = xp;
		y = yp;
	}

	void Perpend()
	{
		x = -y;
		y = x;
	}

	void Perpend2()
	{
		x = y;
		y = -x;
	}

	Vector2D GetRotated(float radians)
	{
		float c = std::cos(radians);
		float s = std::sin(radians);

		float xp = x * c - y * s;
		float yp = x * s + y * c;

		return Vector2D(xp, yp);
	}

	void Normalize(void)
	{
		float len = Len();

		if (len > EPSILON)
		{
			float invLen = 1.0f / len;
			x *= invLen;
			y *= invLen;
		}
	}

	Vector2D GetNormalize(void)
	{
		float len = Len();

		if (len > EPSILON)
		{
			float invLen = 1.0f / len;
			float x1 = x * invLen;
			float y1 = y * invLen;
			return Vector2D(x1, y1);
		}
		return Vector2D(x, y);
	}
};

inline Vector2D operator*(float s, const Vector2D& v)
{
	return Vector2D(s * v.x, s * v.y);
}

struct Mat2
{
	union
	{
		struct
		{
			float m00, m01;
			float m10, m11;
		};

		float m[2][2];
		float v[4];
	};

	Mat2() {}
	Mat2(float radians)
	{
		float c = std::cos(radians);
		float s = std::sin(radians);

		m00 = c; m01 = -s;
		m10 = s; m11 = c;
	}

	Mat2(float a, float b, float c, float d)
		: m00(a), m01(b)
		, m10(c), m11(d)
	{
	}

	void Set(float radians)
	{
		float c = std::cos(radians);
		float s = std::sin(radians);

		m00 = c; m01 = -s;
		m10 = s; m11 = c;
	}

	Mat2 Abs(void) const
	{
		return Mat2(std::abs(m00), std::abs(m01), std::abs(m10), std::abs(m11));
	}

	Vector2D AxisX(void) const
	{
		return Vector2D(m00, m10);
	}

	Vector2D AxisY(void) const
	{
		return Vector2D(m01, m11);
	}

	Mat2 Transpose(void) const
	{
		return Mat2(m00, m10, m01, m11);
	}

	const Vector2D operator*(const Vector2D& rhs) const
	{
		return Vector2D(m00 * rhs.x + m01 * rhs.y, m10 * rhs.x + m11 * rhs.y);
	}

	const Mat2 operator*(const Mat2& rhs) const
	{
		// [00 01]  [00 01]
		// [10 11]  [10 11]

		return Mat2(
			m[0][0] * rhs.m[0][0] + m[0][1] * rhs.m[1][0],
			m[0][0] * rhs.m[0][1] + m[0][1] * rhs.m[1][1],
			m[1][0] * rhs.m[0][0] + m[1][1] * rhs.m[1][0],
			m[1][0] * rhs.m[0][1] + m[1][1] * rhs.m[1][1]
			);
	}
};

inline Vector2D Min(const Vector2D& a, const Vector2D& b)
{
	return Vector2D(std::min(a.x, b.x), std::min(a.y, b.y));
}

inline Vector2D Max(const Vector2D& a, const Vector2D& b)
{
	return Vector2D(std::max(a.x, b.x), std::max(a.y, b.y));
}

inline float DotProduct(const Vector2D& a, const Vector2D& b)
{
	return a.x * b.x + a.y * b.y;
}

inline float DistSqr(const Vector2D& a, const Vector2D& b)
{
	Vector2D c = a - b;
	return DotProduct(c, c);
}

inline float Dist(const Vector2D& a, const Vector2D& b)
{
	Vector2D c = a - b;
	return std::sqrt(DotProduct(c, c));
}

inline Vector2D Cross(const Vector2D& v, float a)
{
	return Vector2D(a * v.y, -a * v.x);
}

inline Vector2D Cross(float a, const Vector2D& v)
{
	return Vector2D(-a * v.y, a * v.x);
}

inline float Cross(const Vector2D& a, const Vector2D& b)
{
	return a.x * b.y - a.y * b.x;
}

inline Vector2D Perpend(const Vector2D& a)
{
	return Vector2D(-a.y, a.x);
}

// Comparison with tolerance of EPSILON
inline bool Equal(float a, float b)
{
	// <= instead of < for NaN comparison safety
	return std::abs(a - b) <= EPSILON;
}

inline float Sqr(float a)
{
	return a * a;
}

inline float Clamp(float min, float max, float a)
{
	if (a < min) return min;
	if (a > max) return max;
	return a;
}

inline int Round(float a)
{
	return (int)(a + 0.5f);
}


inline float ToDegrees(float a)
{
	return a * 180 / M_PI;
}


inline float ToRadians(float a)
{
	return a * M_PI / 180;
}

inline float Random(float l, float h)
{
	float a = (float)rand();
	a /= RAND_MAX;
	a = (h - l) * a + l;
	return a;
}

inline bool BiasGreaterThan(float a, float b)
{
	const float k_biasRelative = 0.95f;
	const float k_biasAbsolute = 0.01f;
	return a >= b * k_biasRelative + a * k_biasAbsolute;
}

inline float GetCatet(float hipotenuze, float angle)
{
	return hipotenuze * cos(angle);
}

inline float GetCatet2(float hipotenuze, float angle)
{
	return hipotenuze * sin(angle);
}

inline float GetSpherePartRadius(float radius, float height)
{
	return sqrt(radius * radius - height * height);
}

inline float GetRectDiagScale(float width, float height)
{
	return sqrt(pow(width, 2) + pow(height, 2));
}

inline float GetDistanceToSphere(const Vector2D& startPos, const Vector2D& dir, const Vector2D& spherePos, float radius)
{
	Vector2D vec = spherePos - startPos;
	Vector2D vecN = vec.GetNormalize();
	float dist = vec.Len();
	float angle = acos(DotProduct(vecN, dir));
	float height = GetCatet2(dist, angle);
	if (height > radius)
		return -1;

	float perpendDist = GetCatet(dist, angle);
	float spherePartRadius = GetSpherePartRadius(radius, height);

	if (isinf(spherePartRadius) || perpendDist < 0)
		return -1;

	Vector2D point1 = startPos + dir * (perpendDist - spherePartRadius);

	return Dist(startPos, point1);
}


inline float frand(float start, float end)
{
	return start + (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * (end - start);
}

// Gravity
const float gravityScale = 1.0f;
const Vector2D gravity(0, 10.0f * gravityScale);
// Itterations per second
const float dt = 1.0f / 60.0f;

#endif /* MASSLIB_H */