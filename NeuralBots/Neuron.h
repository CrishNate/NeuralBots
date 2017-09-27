#pragma once
#include "Dendrite.h"
#include <vector>

class Neuron
{
public:
	Neuron()
		: m_Value(0)
	{
		// generating random value for bias
		m_Bias = frand(-1, 1);
	}

	void SetBias(float bias) { m_Bias = bias; }
	float GetBias() { return m_Bias; }

	void SetDelta(float delta) { m_Delta = delta; }
	float GetDelta() { return m_Delta; }

	void SetValue(float value) { m_Value = value; }
	float GetValue() { return m_Value; }

	int DendriteCount() { return m_Dendrites.size(); }

	bool AddDendrite(Dendrite* neuron)
	{
		m_Dendrites.push_back(neuron);
		return true;
	}

	Dendrite* GetDendrite(int dendriteInx) { return m_Dendrites[dendriteInx]; }

	std::vector<Dendrite*>& GetDendrites() { return m_Dendrites; }

private:

	// fields
	float m_Bias;
	float m_Delta;
	float m_Value;
	std::vector<Dendrite*> m_Dendrites;
};
