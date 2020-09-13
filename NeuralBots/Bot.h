#pragma once
#include <assert.h>

#include "Drawing.h"
#include "PhysObj.h"
#include <time.h>
#include "NeuralNetwork/Include/NeuralNetwork.h"
#include "GenericAlgorithm/Include/GenericAlgorithm.h"


class World;

class Bot 
	: public PhysObj
{
public:
	Bot(const Vector2D& pos)
		: PhysObj(pos, 30, 50)
		, m_ViewAngle(0)
		, m_Color(0, 0, 0)
		, m_Dash(0)
		, m_ShootCooldown(0)
	{
		HSL hsl = HSL(frand(0, 400), 1.0f, 0.5f);
		m_Color = HSLToRGB(hsl);

		std::vector<uint16_t> layers = { 4, 6, 6, 4 };
		m_pBrain = new nn::NeuralNetwork(layers);
		m_pGenome = new ga::GenericAlgorithm(arma::randu(1));

		m_Inputs.resize(layers[0]);
	}

	Bot(const Vector2D& pos, Bot* pBot1, Bot* pBot2)
		: PhysObj(pos, 30, 50)
		, m_ViewAngle(0)
		, m_Color(0, 0, 0)
		, m_Dash(0)
		, m_ShootCooldown(0)
	{
		//if (pBot2 == NULL || pBot1 == NULL) return;
		assert(pBot1 && pBot2);

		HSL hsl = HSL(frand(0, 400), 1.0f, 0.5f);
		m_Color = pBot1->getColor();

		// creating brain
		m_pBrain = new nn::NeuralNetwork(pBot1->getBrain());
		m_pBrain->reproduce(pBot1->getBrain(), pBot2->getBrain());
		m_pBrain->setGeneration(m_pBrain->getGeneration() + 1);
		m_pGenome = new ga::GenericAlgorithm(*pBot1->getGenome(), *pBot2->getGenome());

		m_Inputs.resize(pBot1->m_Inputs.size());
	}

	~Bot();

	virtual void Draw(const Camera& camera, float dTime);
	void Shoot(World* pWorld);
	void respawn(World* pWorld);

	Vector2D getLookDir() { return Vector2D(0, 1).GetRotated(m_ViewAngle); }
	
	Vector2D getRight() { return Vector2D(-1, 0).GetRotated(m_ViewAngle); }

	virtual void Step(World* pWorld, float dTime);
	void Remove() override;
	inline double getShootLength();
	inline double getShootCooldown();

	nn::NeuralNetwork* getBrain() { return m_pBrain; }
	ga::GenericAlgorithm* getGenome() { return m_pGenome; }
	RGBColor getColor() { return m_Color; }

private:
	
	float GetAngleOffsetToNearestEnemy(World* pWorld, float& side, float& dist, float& enemyVision);
	float GetAngleOffsetToNearestDanger(World* pWorld, float& side);

	// fields
	nn::NeuralNetwork* m_pBrain;
	ga::GenericAlgorithm* m_pGenome;
	RGBColor m_Color;
	float m_ViewAngle;
	float m_Dash;
	int m_ShootCooldown;
	int m_ReflectionCooldown;

	std::vector<double> m_Inputs;
};