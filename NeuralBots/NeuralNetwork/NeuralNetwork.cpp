#include "Include\NeuralNetwork.h"
#include <iostream>
#include <assert.h>
#include <armadillo_bits/constants_old.hpp>


#include "../math.h"
#include "../Drawing.h"
#include "../GenericAlgorithm/Include/GenericAlgorithm.h"


namespace nn
{
	NeuralNetwork::NeuralNetwork(uint16_t inputLayer, uint16_t hiddenLayer, uint16_t outputLayer, float learnRate)
		: learnRate_(learnRate)
		, layersCount_(3)
	{
		layers_		= { inputLayer, hiddenLayer, outputLayer };
		neurons_	= new arma::mat[3];
		bias_		= new arma::mat[3];
		weights_	= new arma::mat[2];

		weights_[0] = (arma::randu(hiddenLayer, inputLayer) * 2.0f - 1.0f);	// input - hidden
		weights_[1] = (arma::randu(outputLayer, hiddenLayer) * 2.0f - 1.0f); // hidden - output

		bias_[0] = (arma::randu(inputLayer,	1) * 2.0f - 1.0f);
		bias_[1] = (arma::randu(hiddenLayer, 1) * 2.0f - 1.0f);
		bias_[2] = (arma::randu(outputLayer, 1) * 2.0f - 1.0f);
	}

	NeuralNetwork::NeuralNetwork(const std::vector<uint16_t>& layers, float learnRate)
		: learnRate_(learnRate)
		, layersCount_(layers.size())
	{
		assert(layers.size() > 2);

		layers_		= layers;
		neurons_	= new arma::mat[layersCount_];
		bias_		= new arma::mat[layersCount_];
		weights_	= new arma::mat[layersCount_ - 1];

		// filling random weights
		for (size_t i = 1; i < layersCount_; i++)
			weights_[i - 1] = (arma::randu(layers[i], layers[i - 1]) * 2.0f - 1.0f);

		// filling random biases
		for (size_t i = 0; i < layersCount_; i++)
			bias_[i] = (arma::randu(layers[i], 1) * 2.0f - 1.0f);
	}

	NeuralNetwork::NeuralNetwork(NeuralNetwork* neural_network)
		: learnRate_(neural_network->learnRate_)
		, layersCount_(neural_network->layersCount_)
	{
		layers_.assign(neural_network->layers_.begin(), neural_network->layers_.end());
		neurons_	= new arma::mat[layersCount_];
		bias_		= new arma::mat[layersCount_];
		weights_	= new arma::mat[layersCount_ - 1];

		// filling random weights
		for (size_t i = 1; i < layersCount_; i++)
			weights_[i - 1] = arma::zeros(layers_[i], layers_[i - 1]);

		// filling random biases
		for (size_t i = 0; i < layersCount_; i++)
			bias_[i] = arma::zeros(layers_[i]);
	}

	NeuralNetwork::~NeuralNetwork()
	{ 
		layers_.clear();
		delete[] bias_;
		delete[] neurons_;
		delete[] weights_;
	}

	void NeuralNetwork::activationFunction(double& x)
	{
		x = atan(x);
	}

	void NeuralNetwork::train(const arma::mat& inputs, const arma::mat& targets)
	{
		assert(inputs.n_rows == layers_[0] || targets.n_rows == layers_[layersCount_ - 1]);

		query(inputs);

		// error for output layer
		arma::mat error = targets - neurons_[layersCount_ - 1];
		error_coef = abs(arma::accu(error));

		for (size_t i = layersCount_ - 1; i > 0; i--)
		{
			// error for each layer
			if (i != layersCount_ - 1)
				error = weights_[i].t() * error;

			// (error % neurons_[i] % (1 - neurons_[i])) - delta
			weights_[i - 1] += learnRate_ * (error % neurons_[i] % (1 - neurons_[i])) * neurons_[i - 1].t();
			bias_[i] += learnRate_ * (error % neurons_[i] % (1 - neurons_[i]));
		}
	}

	void NeuralNetwork::query(const arma::mat& inputs)
	{
		assert(inputs.n_rows == layers_[0]);

		neurons_[0] = inputs;
		for (size_t i = 1; i < layersCount_; i++)
		{
			neurons_[i] = weights_[i - 1] * neurons_[i - 1] + bias_[i];
			neurons_[i].for_each([](arma::mat::elem_type& val) { activationFunction(val); });
		}
	}

	void NeuralNetwork::backQuery(const arma::mat& inputs)
	{
		assert(inputs.n_rows == layers_[layersCount_ - 1]);

		neurons_[layersCount_ - 1] = inputs;

		for (int32_t i = layersCount_ - 1; i > 0; i--)
		{
			neurons_[i - 1] = weights_[i - 1].t() * neurons_[i];
			neurons_[i - 1].for_each([](arma::mat::elem_type& val) { activationFunction(val); });
		}
	}

	const arma::mat& NeuralNetwork::result()
	{
		return neurons_[layersCount_ - 1];
	}

	const arma::Mat<double>& nn::NeuralNetwork::input()
	{
		return neurons_[0];
	}

	const arma::Mat<double>& nn::NeuralNetwork::getLayer(uint16_t inx)
	{
		return neurons_[inx];
	}

	uint16_t NeuralNetwork::getGeneration()
	{
		return generation_;
	}

	void NeuralNetwork::setGeneration(uint16_t generation)
	{
		generation_ = generation;
	}

	double NeuralNetwork::getFitness()
	{
		return fitness_;
	}

	void NeuralNetwork::setFitness(double fitness)
	{
		fitness_ = fitness;
	}

	void NeuralNetwork::addFitness(double value)
	{
		fitness_ += value;
	}

	void NeuralNetwork::save(const std::string& fileName)
	{

	}

	void NeuralNetwork::load(const std::string& fileName)
	{

	}
	
	void NeuralNetwork::reproduce(const NeuralNetwork* brain1, const NeuralNetwork* brain2)
	{
		assert(brain1->layersCount_ == brain2->layersCount_);
		assert(layersCount_ == brain1->layersCount_);

		for (int i = 0; i < layersCount_ - 1; ++i)
		{
			ga::GenericAlgorithm::selection(brain1->weights_[i], brain2->weights_[i], weights_[i], true);
		}

		for (int i = 0; i < layersCount_; ++i)
		{
			ga::GenericAlgorithm::selection(brain1->bias_[i], brain2->bias_[i], bias_[i], true);
		}
	}

	void NeuralNetwork::draw(int x, int y, float neuronScale, float spacingX, float spacingY)
	{
		float multX = (neuronScale * 2 + spacingX);
		float multY = (neuronScale * 2 + spacingY);
		//DrawFilledRect(x - m_Layers.size() * multX / 2, y - 250, m_Layers.size() * multX, 500, RGBColor(0.3f * 255, 0.4f * 255, 0.6f * 255));
		//DrawTextQL("Generation: " + std::to_string(m_Generation), 0, 20, 0, RGBColor{ 255, 255, 255 });

		// Draw Dendrites
		for (int i = 1; i < layersCount_; i++)
		{
			int prevLayerNeurCount = i > 0 ? layers_[i - 1] : 0;
		
			for (int j = 0; j < layers_[i]; j++)
			{
				int x1 = x + (i - layersCount_ / 2.0f + 0.5) * multX;
				int y1 = y + (j - neurons_[i].n_rows / 2.0f + 0.5) * multY;
		
				float value = neurons_[i][j];
				
				// Draw Dendrites
				for (int k = 0; k < layers_[i - 1]; k++)
				{
					float weight = weights_[i - 1].at(j, k);
					
					 int x2 = x + ((i - 1) - layersCount_ / 2.0f + 0.5) * multX;
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
					 	fabs(weight) * 2.0f,
					 	RGBColor(255 * std::fmax(0, -weight), 0, 255 * fmax(0, weight), 0)
					 );
				}
			}
		}

		// Draw Neurons
		for (int i = 0; i < layersCount_; i++)
		{
			for (int j = 0; j < neurons_[i].n_rows; j++)
			{
				int x1 = x + (i - layersCount_ / 2.0f + 0.5) * multX;
				int y1 = y + (j - neurons_[i].n_rows / 2.0f + 0.5) * multY;
				float value = neurons_[i][j];

				// Draw Neurons
				DrawFilledCircle(x1, y1, neuronScale, RGBColor(0, 0, 0));
				DrawFilledCircle(x1, y1, neuronScale - 2, RGBColor(255 * std::abs(value), 255 * value, 255 * value));
			}

		}
	}
} // namespace nn