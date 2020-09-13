#include "PhysObj.h"
#include "Drawing.h"

class Bot;

class Bullet 
	: public PhysObj
{
public:

	Bullet(const Vector2D& pos, const Vector2D& dir)
		: PhysObj(pos, 2, 2)
	{
		m_Velocity = dir * 200.0f;
	}

	virtual void Draw(const Camera& camera, float dTime)
	{
		DrawFilledCircle(m_Position.x, m_Position.y, 10, RGBColor(255, 0, 0), camera);
		DrawFilledCircle(m_Position.x, m_Position.y, 5, RGBColor(255, 255, 255), camera);
	}

	void SetOwner(Bot* pBot) { m_pOwner = pBot; }
	Bot* GetOwner() { return m_pOwner; }

private:

	Bot* m_pOwner;
};