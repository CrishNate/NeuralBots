#pragma once
#include "Drawing.h"
#include "PhysObj.h"
#include <time.h>
#include <math.h>
#include "NeuralNetwork.h"
#include "Bullet.h"

#define BODY_SCALE 30
#define HAND_SCALE 10
#define LEG_HEIGHT 7
#define FOOT_WIDTH 5
#define EYE_SCALE 5


class World;

class Bot : public PhysObj
{
public:
	Bot(const Vector2D& pos)
		: PhysObj(pos, 30, 50)
		, m_ViewAngle(0)
		, m_Color(0, 0, 0)
		, m_Anim(0)
		, m_Side(1.0f)
		, m_Ammo(10)
		, m_ShootCooldown(0)
	{
		HSL hsl = HSL(frand(0, 400), 1.0f, 0.5f);
		m_Color = HSLToRGB(hsl);

		// creating brain
		int inputNeuronsCount = 9;
		int outputNeuronsCount = 4;

		std::vector<int> layers = { inputNeuronsCount, 12, outputNeuronsCount };
		m_pBrain = new NeuralNetwork(10, layers);
	}

	virtual void Draw(const Camera& camera, float dTime)
	{
		m_Anim += dTime * m_Velocity.x / 50.0f * dTime;
		if (m_Anim > M_2PI)
			m_Anim -= M_2PI;
		if (m_Anim < -M_2PI)
			m_Anim += M_2PI;

		DrawFilledRectC(m_Position.x, m_Position.y - LEG_HEIGHT, BODY_SCALE, BODY_SCALE, RGBColor(100, 100, 150), camera);

		// Eye
		Vector2D offset = Vector2D(10, 0).GetRotated(m_ViewAngle);
		DrawFilledRectC(m_Position.x + offset.x, m_Position.y - LEG_HEIGHT - 5, 1.5f * EYE_SCALE, EYE_SCALE, m_Color, camera);

		// Foots
		Vector2D offsetL = Vector2D(-cos(m_Anim) * 10 * m_Side, -fmax(0, sin(m_Anim) * 5)) * fmin(1, abs(m_Velocity.x / 5.0f));
		Vector2D offsetR = Vector2D(cos(m_Anim) * 10 * m_Side, -fmax(0, -sin(m_Anim) * 5)) * fmin(1, abs(m_Velocity.x / 5.0f));

		DrawFilledRectC(m_Position.x + offsetL.x, m_Position.y + BODY_SCALE / 2 + LEG_HEIGHT + offsetL.y, 1.5f * FOOT_WIDTH, 6, RGBColor(80, 80, 120), camera);
		DrawFilledRectC(m_Position.x + offsetR.x, m_Position.y + BODY_SCALE / 2 + LEG_HEIGHT + offsetR.y, 1.5f * FOOT_WIDTH, 6, RGBColor(80, 80, 120), camera);

		Vector2D handpos = Vector2D(25, 0).GetRotated(m_ViewAngle);
		// Hand
		DrawFilledRectC(m_Position.x + handpos.x, m_Position.y + handpos.y, HAND_SCALE, HAND_SCALE, RGBColor(80, 80, 120), camera);
		DrawFilledRectC(m_Position.x + handpos.x, m_Position.y + handpos.y, HAND_SCALE / 4.0f, HAND_SCALE / 4.0f, m_Color, camera);
	
		// eye dir
		offset = Vector2D(50, 0).GetRotated(m_ViewAngle);
		DrawLineThink(
			m_Position.x + offset.x / 1.5,
			m_Position.y - LEG_HEIGHT + offset.y / 1.5,
			m_Position.x + offset.x,
			m_Position.y - LEG_HEIGHT + offset.y, 2,
			RGBColor(255, 255, 255), camera);
	}

	Vector2D GetLookDir()
	{
		return Vector2D(1, 0).GetRotated(m_ViewAngle);
	}

	virtual void Step(World* pWorld, float dTime);

	NeuralNetwork* GetBrain() { return m_pBrain; }

private:
	
	float GetAngleOffsetToNearestEnemy(World* pWorld, int& side);
	float GetAngleOffsetToNearestDanger(World* pWorld, int& side);

	// fields
	float m_ViewAngle;
	RGBColor m_Color;
	float m_Anim;

	float m_Side;
	NeuralNetwork* m_pBrain;

	int m_Ammo;
	int m_ShootCooldown;
};