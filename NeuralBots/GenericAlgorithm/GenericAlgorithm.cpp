#include "Include/GenericAlgorithm.h"
#include "..\math.h"


namespace ga
{
    GenericAlgorithm::GenericAlgorithm(const arma::mat& genome)
        : m_Genome(genome)
    {
    }

    GenericAlgorithm::GenericAlgorithm(const arma::mat& genome1, const arma::mat& genome2)
        : m_Genome(genome1)
    {
        selection(genome1, genome2, m_Genome);
    }

    GenericAlgorithm::GenericAlgorithm(const GenericAlgorithm& genome1, const GenericAlgorithm& genome2)
        : m_Genome(genome1.m_Genome)
    {
        selection(genome1.m_Genome, genome2.m_Genome, m_Genome);
    }

    void GenericAlgorithm::selection(const arma::mat& mat1, const arma::mat& mat2, arma::mat& out, bool includeNegative)
    {
        for (int i = 0; i < out.n_elem; ++i)
        {
            out.at(i) = rand() % 2 == 0 ? mat1.at(i) : mat2.at(i);
        }
        mutate(out, includeNegative);
    }

    void GenericAlgorithm::mutate(arma::mat& genome, bool includeNegative)
    {
        genome.for_each( [includeNegative](arma::mat::elem_type& val){ mutation(val, includeNegative); } );
    }

    void GenericAlgorithm::mutation(double& value, bool includeNegative)
    {
        if (includeNegative)
        {
            if (rand() % 1000 <= 2)
                value *= -1;
						
            if (rand() % 1000 <= 4)
                value = frand(-1, 1);
        }
        else
        {
            if (rand() % 1000 <= 4)
                value = frand(0, 1);
        }
						
        if (rand() % 1000 <= 6)
        {
            value *= frand(0, 1) + 1;
            value = Clamp(-1, 1, value);
        }
    }
}
