#pragma once
#include "Neuron.h"

class Layer
{
public:
	Layer() { }

	int NeuronCount() { return m_Neurons.size(); }

	bool AddNeuron(Neuron* neuron)
	{
		m_Neurons.push_back(neuron);
		return true;
	}

	Neuron* GetNeuron(int neuronInx) { return m_Neurons[neuronInx]; }

	std::vector<Neuron*> GetNeurons() { return m_Neurons; }

private:
	// fields
	std::vector<Neuron*> m_Neurons;
};