#pragma once
#include <armadillo>

namespace ga
{
    class GenericAlgorithm
    {
    public:
        GenericAlgorithm(const arma::mat& genome);
        GenericAlgorithm(const arma::mat& genome1, const arma::mat& genome2);
        GenericAlgorithm(const GenericAlgorithm& genome1, const GenericAlgorithm& genome2);

        static void selection(const arma::mat& mat1, const arma::mat& mat2, arma::mat& out, bool includeNegative = false);
        static void mutate(arma::mat& genome, bool includeNegative);
        static void mutation(double& value, bool includeNegative);

        const arma::mat& getGenome() const { return m_Genome; }
        
    private:
        
        arma::mat m_Genome;
    };
}