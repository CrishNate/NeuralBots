#pragma once
#include "math.h"
#include <vector>

class PhysObj
{
public:

	PhysObj(const Vector2D& pos, float width, float height)
		: m_Position(pos)
		, m_Radius(GetRectDiagScale(width, height))
		, m_Velocity(Vector2D(0, 0))
		, m_Orient(0)
		, m_AngularVelocity(0)
		, m_Static(false)
		, m_Width(width)
		, m_Height(height)
	{ }

	void SetPosition(const Vector2D& pos) { m_Position = pos; }
	Vector2D GetPosition() { return m_Position; }

	void SetVelocity(const Vector2D& vel) { m_Velocity = vel; }
	Vector2D GetVelocity() { return m_Velocity; }
	void ApplyForce(const Vector2D& vel) { m_Velocity += vel; }
	void ApplyImpulse(const Vector2D& impulse, const Vector2D& contactVector)
	{
		m_Velocity += impulse * 2.0f;
		m_AngularVelocity += Cross(contactVector, impulse);
	}

	void SetOrient(float orient) { m_Orient = orient; }
	void AddOrient(float orient) { m_Orient += orient; }
	float GetOrient() { return m_Orient; }

	void SetAngularVelocity(float angularVelocity) { m_AngularVelocity = angularVelocity; }
	float GetAngularVelocity() { return m_AngularVelocity; }

	float GetRadius() { return m_Radius; }
	float GetWidth() { return m_Width; }
	float GetHeight() { return m_Height; }

	void SetStatic() { m_Static = true; }
	bool IsStatic() { return m_Static; }

	virtual void Draw(const Camera& camera, float dTime)
	{ 
		DrawFilledRectC(m_Position.x, m_Position.y, m_Width, m_Height, RGBColor(100, 100, 100), camera);
	}

	virtual void RunPhys(float dTime) 
	{
		if (m_Static)
			return;

		m_Velocity += gravity * dTime / 100.0f;
		m_Position += m_Velocity * dTime / 100.0f;
		m_Orient += m_AngularVelocity * dTime / 100.0f;
	}

	virtual	void Remove()
	{
		delete this;
	}

protected:

	// fields

	Vector2D m_Position;
	Vector2D m_Velocity;

	float m_Orient;
	float m_AngularVelocity;
	bool m_Static;
	float m_Radius;
	float m_Width;
	float m_Height;
};

