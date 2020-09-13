/*
* A sample source file for the code formatter preview
*/

#pragma once

#ifndef NEURAL_NETWORK_H_
#define NEURAL_NETWORK_H_

#include <cstdint>
#include <armadillo>
#include <vector>

namespace nn 
{
	class NeuralNetwork
	{
	public:
		NeuralNetwork(uint16_t inputLayer, uint16_t hiddenLayer, uint16_t outputLayer, float learnRate = 0.1f);
		NeuralNetwork(const std::vector<uint16_t>& layers, float learnRate = 0.1f);
		NeuralNetwork(NeuralNetwork* neural_network);
		~NeuralNetwork();

		void train(const arma::Mat<double>& inputs, const arma::Mat<double>& targets);
		void query(const arma::Mat<double>& inputs);
		void backQuery(const arma::mat& inputs);
		static void activationFunction(double& x);
		void save(const std::string& fileName);
		void load(const std::string& fileName);
		void reproduce(const NeuralNetwork* brain1, const NeuralNetwork* brain2);

		const arma::Mat<double>& result();
		const arma::Mat<double>& input();
		const arma::Mat<double>& getLayer(uint16_t inx);

		double error_coef;

		uint16_t getGeneration();
		void setGeneration(uint16_t generation);

		double getFitness();
		void setFitness(double fitness);
		void addFitness(double value);

		void draw(int x, int y, float neuronScale = 10.0f, float spacingX = 100.0f, float spacingY = 25.0f);

	protected:

	private:
		uint16_t generation_;
		double fitness_;
		
		std::vector<uint16_t> layers_;
		uint16_t layersCount_;
		float learnRate_;
		arma::mat* neurons_;
		arma::mat* bias_;
		arma::mat* weights_;
	};
} // namespace nn

#endif  // NEURAL_NETWORK_H_