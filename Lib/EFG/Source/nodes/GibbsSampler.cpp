/**
 * Author:    Andrea Casalino
 * Created:   01.01.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#include <nodes/GibbsSampler.h>
#include <distribution/factor/const/Factor.h>
#include <algorithm>
#include <random>

namespace EFG::nodes {
    class GibbsSampler::UniformSampler {
    public:
        UniformSampler() = default;

        inline float sample() const { return this->distribution(this->generator); };

        std::size_t sampleFromDiscrete(const std::vector<float>& distribution) const;

        std::size_t sampleUniformDiscrete(std::size_t varSize) const;

    private:
        mutable std::default_random_engine generator;
        mutable std::uniform_real_distribution<float> distribution;
    };

    std::size_t GibbsSampler::UniformSampler::sampleFromDiscrete(const std::vector<float>& distribution) const {
        float s = this->sample();
        float cumul = 0.f;
        for (std::size_t k = 0; k < distribution.size(); ++k) {
            cumul += distribution[k];
            if (s <= cumul) {
                return k;
            }
        }
        return distribution.size() - 1;
    }

    std::size_t GibbsSampler::UniformSampler::sampleUniformDiscrete(std::size_t varSize) const {
        return static_cast<std::size_t>(roundf(this->sample() * (varSize - 1)));
    }

    GibbsSampler::SamplesStructure GibbsSampler::getSamplesStructure() const {
        SamplesStructure structure;
        for (auto itN = this->nodes.begin(); itN != this->nodes.end(); ++itN) {
            distribution::DistributionPtr unaryMerged;
            if(!itN->second.unaryFactors.empty()) {
                std::set<const distribution::Distribution*> toMerge;
                std::for_each(itN->second.unaryFactors.begin(), itN->second.unaryFactors.end(), [&toMerge](const distribution::DistributionPtr& d) {
                    toMerge.emplace(d.get());
                });
                unaryMerged = std::make_shared<distribution::factor::cnst::Factor>(toMerge);
            }
            NodeHidden node{ 0, unaryMerged, {} };
            for (auto itC = itN->second.activeConnections.begin(); itC != itN->second.activeConnections.end(); ++itC) {
                node.connections.push_back(NodeHiddenConnection{ itC->second.factor, itC->first->variable, nullptr});
            }
            structure.emplace(itN->first, node);
        }
        for (auto itS = structure.begin(); itS != structure.end(); ++itS) {
            for (auto itC = itS->second.connections.begin(); itC != itS->second.connections.end(); ++itC) {
                auto itSS = structure.find(itC->neighbourVariable);
                itC->neighbourSample = &itSS->second.sample;
            }
        }
        return structure;
    }

    void GibbsSampler::evolveSamples(SamplesStructure& structure, std::size_t iterations, UniformSampler& sampler) {
        for (std::size_t i = 0; i < iterations; ++i) {
            for (auto it = structure.begin(); it != structure.end(); ++it) {
                std::set<const distribution::Distribution*> toMerge = { it->second.unaryMerged.get() };
                if (nullptr != it->second.unaryMerged) {
                    toMerge.emplace(it->second.unaryMerged.get());
                }
                std::list<distribution::factor::cnst::Factor> marginalized;
                for (auto c = it->second.connections.begin(); c != it->second.connections.end(); ++c) {
                    marginalized.emplace_back(*c->factor, Combination({ *c->neighbourSample }), categoric::Group(c->neighbourVariable));
                    toMerge.emplace(&marginalized.back());
                }
                if (toMerge.empty()) {
                    it->second.sample = sampler.sampleUniformDiscrete(it->first->size());
                }
                else {
                    it->second.sample = sampler.sampleFromDiscrete(distribution::factor::cnst::Factor(toMerge).getProbabilities());
                }
            }
        }
    }

    std::vector<std::size_t> GibbsSampler::convert(const SamplesStructure& structure) {
        std::vector<std::size_t> converted;
        converted.reserve(structure.size());
        for (auto it = structure.begin(); it != structure.end(); ++it) {
            converted.push_back(it->second.sample);
        }
        return converted;
    }

    std::vector<std::vector<std::size_t>> GibbsSampler::getHiddenSetSamples(std::size_t numberOfSamples, std::size_t deltaIteration) const {
        UniformSampler sampler;
        auto structure = this->getSamplesStructure();
        std::vector<std::vector<std::size_t>> samples;
        samples.reserve(numberOfSamples);
        evolveSamples(structure, 10 * deltaIteration, sampler);
        for (std::size_t s = 0; s < numberOfSamples; ++s) {
            evolveSamples(structure, deltaIteration, sampler);
            samples.push_back(convert(structure));

        }
        return samples;
    }
}