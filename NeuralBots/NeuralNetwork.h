#pragma once
#include "Layer.h"
#include <iostream>
#include <time.h>
#include "Drawing.h"

#define NeuronScale 10
#define DistanceBetweenNeuronsX 100
#define DistanceBetweenNeuronsY 25

inline float Sigmoid(float x)
{
	return 1.0f / (1.0f + exp(-x));
}

class NeuralNetwork
{
public:
	NeuralNetwork(float learningRate, const std::vector<int>& layers)
		: m_LearningRate(learningRate)
	{
		// cannot be less than 2 layers
		if (layers.size() < 2)
			return;

		CreateNNTree(layers);
	}

	// Clone Neural Network
	NeuralNetwork(NeuralNetwork* nn)
		: m_Generation(nn->GetGeneration())
	{
		std::vector<int> layers;
		for (int i = 0; i < nn->LayerCount(); i++)
			layers.push_back(nn->GetLayer(i)->NeuronCount());

		CreateNNTree(layers);

		for (int i = 0; i < m_Layers.size(); i++)
		{
			Layer* pLayer = m_Layers[i];

			for (int j = 0; j < pLayer->NeuronCount(); j++)
			{
				Neuron* pNeuron = pLayer->GetNeuron(j);
				pNeuron->SetBias(nn->GetLayer(i)->GetNeuron(j)->GetBias());

				for (int k = 0; k < pNeuron->DendriteCount(); k++)
				{
					Dendrite* pDendrite = pNeuron->GetDendrite(k);
					float weight = nn->GetLayer(i)->GetNeuron(j)->GetDendrite(k)->GetWeight();
					pDendrite->SetWeight(weight);
				}
			}
		}
	}

	bool Reproduce(NeuralNetwork* pParent1, NeuralNetwork* pParent2)
	{
		if (pParent1->LayerCount() != pParent2->LayerCount())
			return false;

		for (int i = 0; i < pParent1->LayerCount(); i++)
		{
			Layer* pLayer1 = pParent1->GetLayer(i);
			Layer* pLayer2 = pParent2->GetLayer(i);

			if (pLayer1->NeuronCount() != pLayer2->NeuronCount())
				return false;

			if (i == 1)
				continue;

			for (int j = 0; j < pLayer1->NeuronCount(); j++)
			{
				Neuron* pNeuron1 = pLayer1->GetNeuron(j);
				Neuron* pNeuron2 = pLayer2->GetNeuron(j);

				if (pNeuron1->DendriteCount() != pNeuron2->DendriteCount())
					return false;

				for (int k = 0; k < pNeuron1->DendriteCount(); k++)
				{
					float weight = frand(0, 1) > 0 ? pNeuron1->GetDendrite(k)->GetWeight() : pNeuron2->GetDendrite(k)->GetWeight();
					m_Layers[i]->GetNeuron(j)->GetDendrite(k)->SetWeight(weight);
				}
			}
		}

		Mutate();

		return true;
	}

	void CreateNNTree(const std::vector<int>& layers)
	{
		// creating layers
		for (int i = 0; i < layers.size(); i++)
		{
			Layer* pLayer = new Layer();
			m_Layers.push_back(pLayer);

			// creating neurons
			for (int j = 0; j < layers[i]; j++)
			{
				Neuron* pNeuron = new Neuron();
				pLayer->AddNeuron(pNeuron);

				if (i == 0)
					pNeuron->SetBias(0);
				else
					// creating dendrites
					for (int d = 0; d < layers[i - 1]; d++)
						pNeuron->AddDendrite(new Dendrite());
			}
		}
	}

	void Mutate()
	{
		for (int i = 1; i < m_Layers.size(); i++)
		{
			Layer* pLayer = m_Layers[i];

			for (int j = 0; j < pLayer->NeuronCount(); j++)
			{
				Neuron* pNeuron = pLayer->GetNeuron(j);

				float bias = pNeuron->GetBias();
				for (int k = 0; k < pNeuron->DendriteCount(); k++)
				{
					Dendrite* pDendrite = pNeuron->GetDendrite(k);

					float weight = pNeuron->GetDendrite(k)->GetWeight();
					float randNum = rand() % 1000;

					if (randNum <= 1)
						weight *= frand(-1, 1);

					if (randNum <= 2)
						weight *= -1;
					
					if (randNum <= 4)
						weight = frand(-1, 1);
					
					if (randNum <= 6)
						weight *= (frand(0, 1) + 1);

					if (randNum <= 8)
						bias *= (frand(0, 1) + 1);

					pDendrite->SetWeight(weight);
				}

				pNeuron->SetBias(bias);
			}
		}
	}


	std::vector<float> Run(const std::vector<float>& input)
	{
		// input data size must equal to input layer neurons count
		if (input.size() != m_Layers[0]->NeuronCount()) return std::vector<float>();

		for (int i = 0; i < m_Layers.size(); i++)
		{
			Layer* pLayer = m_Layers[i];

			for (int j = 0; j < pLayer->NeuronCount(); j++)
			{
				Neuron* neuron = pLayer->GetNeuron(j);

				// setting values to input layer
				if (i == 0)
					neuron->SetValue(input[j]);
				else
				{
					neuron->SetValue(0);
					for (int np = 0; np < m_Layers[i - 1]->NeuronCount(); np++)
					{
						float weight = neuron->GetDendrite(np)->GetWeight();
						float prevValue = neuron->GetValue();
						float prevLayerNeuronValue = m_Layers[i - 1]->GetNeuron(np)->GetValue();
						neuron->SetValue(prevValue + prevLayerNeuronValue * weight);
					}

					neuron->SetValue(std::tanh(neuron->GetValue() + neuron->GetBias()));
				}
			}
		}

		Layer* pOutputLayer = m_Layers[m_Layers.size() - 1];
		int numOutput = pOutputLayer->NeuronCount();
		std::vector<float> output;
		for (int i = 0; i < pOutputLayer->NeuronCount(); i++)
			output.push_back(pOutputLayer->GetNeuron(i)->GetValue());

		return output;
	}

	bool Train(std::vector<float> input, std::vector<float> output)
	{
		if ((input.size() != m_Layers[0]->NeuronCount()) || (output.size() != m_Layers[m_Layers.size() - 1]->NeuronCount())) return false;

		Run(input);

		for (int i = 0; i < m_Layers[m_Layers.size() - 1]->NeuronCount(); i++)
		{
			Neuron* pNeuron = m_Layers[m_Layers.size() - 1]->GetNeuron(i);

			pNeuron->SetDelta(pNeuron->GetValue() * (1.0f - pNeuron->GetValue()) * (output[i] - pNeuron->GetValue()));

			// calculating delta for all hidden layers
			for (int j = m_Layers.size() - 2; j > 0; j--)
			{
				for (int k = 0; k < m_Layers[j]->NeuronCount(); k++)
				{
					Neuron* pN = m_Layers[j]->GetNeuron(k);

					pN->SetDelta(pN->GetValue() *
						(1 - pN->GetValue()) *
						m_Layers[j + 1]->GetNeuron(i)->GetDendrite(k)->GetWeight() *
						m_Layers[j + 1]->GetNeuron(i)->GetDelta());
				}
			}
		}

		for (int i = m_Layers.size() - 1; i > 0; i--)
		{
			for (int j = 0; j < m_Layers[i]->NeuronCount(); j++)
			{
				Neuron* n = m_Layers[i]->GetNeuron(j);
				n->SetBias(n->GetBias() + (m_LearningRate * n->GetDelta()));

				for (int k = 0; k < n->DendriteCount(); k++)
					n->GetDendrite(k)->SetWeight(n->GetDendrite(k)->GetWeight() + (m_LearningRate * (m_Layers[i - 1]->GetNeuron(k)->GetValue() * n->GetDelta())));
			}
		}

		return true;
	}

	/*
		Compare two networks, 
		returns 1 if first neural network fitness is greather then second
		-1 if first fitness is less then second
		0 if its fitnesses is similar
	*/
	int Compare(NeuralNetwork* nn)
	{
		if (m_Fitness > nn->GetFitness())
		{
			return 1;
		}
		else if (m_Fitness < nn->GetFitness())
		{
			return -1;
		}
		else
		{
			return 0;
		}
	}

	void Draw(int x, int y)
	{
		float multX = (NeuronScale * 2 + DistanceBetweenNeuronsX);
		float multY = (NeuronScale * 2 + DistanceBetweenNeuronsY);
		//DrawFilledRect(x - m_Layers.size() * multX / 2, y - 250, m_Layers.size() * multX, 500, RGBColor(0.3f * 255, 0.4f * 255, 0.6f * 255));
		//DrawTextQL("Generation: " + std::to_string(m_Generation), 0, 20, 0, RGBColor{ 255, 255, 255 });

		// Draw Dendrites
		for (int i = 0; i < m_Layers.size(); i++)
		{
			Layer* pLayer = m_Layers[i];
			Layer* pLayerPrev = i > 0 ? m_Layers[i - 1] : NULL;
			std::vector<Neuron*> neurons = pLayer->GetNeurons();
			int prevLayerNeurCount = i > 0 ? pLayerPrev->NeuronCount() : 0;

			for (int j = 0; j < neurons.size(); j++)
			{
				Neuron* pNeuron = neurons[j];

				int x1 = x + (i - m_Layers.size() / 2.0f + 0.5) * multX;
				int y1 = y + (j - neurons.size() / 2.0f + 0.5) * multY;

				// Draw Dendrites
				std::vector<Dendrite*> dendrites = pNeuron->GetDendrites();
				for (int k = 0; k < dendrites.size(); k++)
				{
					float weight = dendrites[k]->GetWeight();
					int x2 = x + ((i - 1) - m_Layers.size() / 2.0f + 0.5) * multX;
					int y2 = y + (k - prevLayerNeurCount / 2.0f + 0.5) * multY;

					int cX = (x1 + x2) / 2;
					int cY = (y1 + y2) / 2;
					float dirX = (x2 - (x1));
					float dirY = (y2 - (y1));
					float angle = std::atan2(dirY, dirX);
					//float angle = atan2(vector2.y, vector2.x) - atan2(vector1.y, vector1.x);;
					if (angle < 0) angle += 2 * M_PI;
					angle = angle * 180 / M_PI + 180;

					DrawLineThinkT(
						x1, y1, 
						x2, y2, 
						weight, 
						RGBColor(255 * -std::fmin(0, weight), 0, 255 * fmax(0, weight), 255 * abs(weight))
						);
				}
			}
		}

		// Draw Neurons
		for (int i = 0; i < m_Layers.size(); i++)
		{
			Layer* pLayer = m_Layers[i];
			Layer* pLayerPrev = i > 0 ? m_Layers[i - 1] : NULL;
			std::vector<Neuron*> neurons = pLayer->GetNeurons();
			int prevLayerNeurCount = i > 0 ? pLayerPrev->NeuronCount() : 0;

			for (int j = 0; j < neurons.size(); j++)
			{
				Neuron* pNeuron = neurons[j];

				int x1 = x + (i - m_Layers.size() / 2.0f + 0.5) * multX;
				int y1 = y + (j - neurons.size() / 2.0f + 0.5) * multY;
				float value = pNeuron->GetValue();

				// Draw Neurons
				DrawFilledCircle(x1, y1, NeuronScale , RGBColor( 0, 0, 0 ));
				DrawFilledCircle(x1, y1, NeuronScale - 4, RGBColor( 255 * std::abs(value), 255 * value, 255 * value ));
			}

		}
	}

	//void DrawCircle(int x, int y, int width, int height)
	//{
	//	unsigned int inputNeuronsCount = m_Layers[0]->NeuronCount();
	//	unsigned int hiddenNeuronsCount = m_Layers[1]->NeuronCount();
	//	unsigned int outputsNeuronsCount = m_Layers[2]->NeuronCount();
	//	const std::vector<Neuron*> inputNeurons = m_Layers[0]->GetNeurons();
	//	const std::vector<Neuron*> hiddenNeurons = m_Layers[1]->GetNeurons();
	//	const std::vector<Neuron*> outputNeurons = m_Layers[2]->GetNeurons();

	//	// Draw Dendrite
	//	for (int i = 0; i < hiddenNeuronsCount; i++)
	//	{
	//		float x1, y1;
	//		float div = M_2PI / hiddenNeuronsCount;
	//		x1 = cos(i * div) * (width / 2 - NeuronScale) + x + width / 2;
	//		y1 = sin(i * div) * (width / 2 - NeuronScale) + y + height / 2;

	//		unsigned int dendritesC = hiddenNeurons[i]->DendriteCount();
	//		const std::vector<Dendrite*> dendrites = hiddenNeurons[i]->GetDendrites();

	//		for (int j = 0; j < inputNeuronsCount; j++)
	//		{
	//			float x2, y2;
	//			float offset = fmin(width, (NeuronScale * 2 + DistanceBetweenNeuronsX) * inputNeuronsCount) / inputNeuronsCount;
	//			x2 = (-(inputNeuronsCount / 2.0f) + j + 0.5) * offset + x + width / 2;
	//			y2 = height - NeuronScale + y;

	//			float weight = dendrites[j]->GetWeight();
	//			DrawLineThinkT(
	//				x1, y1,
	//				x2, y2,
	//				weight,
	//				RGBColor(255 * -std::fmin(0, weight), 0, 255 * fmax(0, weight)));
	//		}
	//	}

	//	// Draw Neurons
	//	for (int i = 0; i < inputNeuronsCount; i++)
	//	{
	//		float x1, y1;
	//		float offset = fmin(width, (NeuronScale * 2 + DistanceBetweenNeuronsX) * inputNeuronsCount) / inputNeuronsCount;
	//		x1 = (-(inputNeuronsCount / 2.0f) + i + 0.5) * offset + x + width / 2;
	//		y1 = height - NeuronScale + y;

	//		float value = inputNeurons[i]->GetValue();
	//		DrawFilledCircle(x1, y1, NeuronScale, RGBColor(0, 0, 0));
	//		DrawFilledCircle(x1, y1, NeuronScale - 4, RGBColor(255 * std::abs(value), 255 * value, 255 * value));
	//	}

	//	for (int i = 0; i < hiddenNeuronsCount; i++)
	//	{
	//		float x1, y1;
	//		float div = M_2PI / hiddenNeuronsCount;
	//		x1 = cos(i * div) * (width / 2 - NeuronScale) + x + width / 2;
	//		y1 = sin(i * div) * (width / 2 - NeuronScale) + y + height / 2;

	//		float value = hiddenNeurons[i]->GetValue();
	//		DrawFilledCircle(x1, y1, NeuronScale, RGBColor(0, 0, 0));
	//		DrawFilledCircle(x1, y1, NeuronScale - 4, RGBColor(255 * std::abs(value), 255 * value, 255 * value));
	//	}

	//	for (int i = 0; i < outputsNeuronsCount; i++)
	//	{
	//		float x1, y1;
	//		float offset = fmin(width, (NeuronScale * 2 + DistanceBetweenNeuronsX) * outputsNeuronsCount) / outputsNeuronsCount;
	//		x1 = (-(outputsNeuronsCount / 2.0f) + i + 0.5) * offset + x + width / 2;
	//		y1 = NeuronScale + y;

	//		float value = inputNeurons[i]->GetValue();
	//		DrawFilledCircle(x1, y1, NeuronScale, RGBColor(0, 0, 0));
	//		DrawFilledCircle(x1, y1, NeuronScale - 4, RGBColor(255 * std::abs(value), 255 * value, 255 * value));
	//	}
	//}

	int GetDrawHeight()
	{
		int max = 0;
		for (Layer* pLayer : m_Layers)
		{
			if (max < pLayer->NeuronCount())
				max = pLayer->NeuronCount();
		}

		return (NeuronScale * 2 + DistanceBetweenNeuronsY) * static_cast<float>(max);
	}

	Layer* GetLayer(int layer) { return m_Layers[layer]; }
	std::vector<Layer*> GetLayers() { return m_Layers; }

	// generation
	void AddGeneration(int toAdd) { m_Generation += toAdd; }
	void SetGeneration(int generation) { m_Generation = generation; }
	int GetGeneration() { return m_Generation; }

	// fitness
	void AddFitness(float toAdd) { m_Fitness += toAdd; }
	void SetFitness(float fitness) { m_Fitness = fitness; }
	float GetFitness() { return m_Fitness; }

	int LayerCount()
	{
		return m_Layers.size();
	}

private:
	// fields
	std::vector<Layer*> m_Layers;
	float m_LearningRate;
	int m_Generation;
	float m_Fitness;
};