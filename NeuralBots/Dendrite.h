#pragma once
#include <cstdlib>

class Dendrite
{
public:
	Dendrite()
		: m_Weight(NULL)
	{
		// randomizing weight
		m_Weight = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 2.0f - 1.0f;
	}

	void SetWeight(float value) { m_Weight = value; }
	float GetWeight() { return m_Weight; }

private:
	// fields
	float m_Weight;
};