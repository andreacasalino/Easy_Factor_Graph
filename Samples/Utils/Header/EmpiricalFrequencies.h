/**
 * Author:    Andrea Casalino
 * Created:   01.01.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#ifndef EFG_SAMPLE_EMPIRICAL_FREQUENCY_H
#define EFG_SAMPLE_EMPIRICAL_FREQUENCY_H

#include <categoric/Group.h>
#include <Combination.h>

namespace EFG::sample {
    // //compute the empirical frequencies of a variable (var_sampled), considering a list of realizations (sample) taken as samples from a joint probability distribution (the one involving vars_in_sample) 
    // //the result is put into marginals
    std::vector<float> getEmpiricalMarginalFrequencies(categoric::VariablePtr var2Search, const std::vector<Combination>& samples, const categoric::Group& samplesGroup);

    // //similar to the above function, computing in single call the empirical distributions of all the variables in vars_to_search
    float getEmpiricalFrequencies(const Combination& comb2Search, const categoric::Group& combGroup, const std::vector<Combination>& samples, const categoric::Group& samplesGroup);
}

#endif
