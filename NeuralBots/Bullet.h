#include "PhysObj.h"
#include "Drawing.h"

#define BEAM_LENGTH 10000

class Bullet : public PhysObj
{
public:

	Bullet(const Vector2D& pos, const Vector2D& dir)
		: PhysObj(pos, 2, 2)
	{
		m_Velocity = dir * 200.0f;
	}

	virtual void Draw(const Camera& camera, float dTime)
	{
		DrawFilledRectRC(m_Position.x, m_Position.y, 3, 2, m_Orient, RGBColor(100, 100, 100), camera);

		Vector2D vel = m_Velocity;
		Vector2D point = m_Position;
		std::vector<Vector2D> points = { m_Position };

		for (int i = 0; i < 100; i++)
		{
			vel += gravity / 10.0f;
			point += vel / 10.0f;

			points.push_back(point);
		}

		DrawLinesThinkT(points, 1, RGBColor(255, 0, 0), camera);
	}

private:
};