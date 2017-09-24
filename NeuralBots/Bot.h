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
#define FOOT_WIDTH 10
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
		int outputNeuronsCount = 5;

		std::vector<int> layers = { inputNeuronsCount, 12, outputNeuronsCount };
		m_pBrain = new NeuralNetwork(10, layers);
	}


	Bot(const Vector2D& pos, Bot* pBot1, Bot* pBot2)
		: PhysObj(pos, 30, 50)
		, m_ViewAngle(0)
		, m_Color(0, 0, 0)
		, m_Anim(0)
		, m_Side(1.0f)
		, m_Ammo(10)
		, m_ShootCooldown(0)
	{
		HSL hsl = HSL(frand(0, 400), 1.0f, 0.5f);
		m_Color = pBot1->GetColor();

		// creating brain
		int inputNeuronsCount = 9;
		int outputNeuronsCount = 5;

		std::vector<int> layers = { inputNeuronsCount, 12, outputNeuronsCount };
		m_pBrain = new NeuralNetwork(pBot1->GetBrain());
		m_pBrain->Mutate();
	}


	virtual void Draw(const Camera& camera, float dTime);

	Vector2D GetLookDir()
	{
		return Vector2D(1, 0).GetRotated(m_ViewAngle);
	}

	virtual void Step(World* pWorld, float dTime);

	NeuralNetwork* GetBrain() { return m_pBrain; }
	RGBColor GetColor() { return m_Color; }

private:
	
	float GetAngleOffsetToNearestEnemy(World* pWorld, int& side);
	float GetAngleOffsetToNearestDanger(World* pWorld, int& side);

	// fields
	float m_ViewAngle;
	RGBColor m_Color;
	float m_Anim;
	float m_ReflectAnim;

	float m_Side;
	NeuralNetwork* m_pBrain;

	int m_Ammo;
	int m_ShootCooldown;
	int m_ReflectionCooldown;
};