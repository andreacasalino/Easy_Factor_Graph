/**
 * Author:    Andrea Casalino
 * Created:   01.01.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#include <structure/GibbsSampler.h>
#include <distribution/factor/modifiable/Factor.h>
#include "Commons.h"
#include <algorithm>
#include <random>

namespace EFG::strct {
    class GibbsSampler::UniformSampler {
    public:
        UniformSampler() = default;

        inline float sample() const { return this->distribution(this->generator); };

        std::size_t sampleFromDiscrete(const std::vector<float>& distribution) const;

        void resetSeed(const std::size_t& newSeed) { this->generator.seed( static_cast<unsigned int>(newSeed)); }

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

    GibbsSampler::HiddenStructure GibbsSampler::getHiddenStructure() const {
        HiddenStructure structure;
        std::for_each(this->hidden.clusters.begin(), this->hidden.clusters.end(), [&structure](const std::set<Node*>& s) {
            std::for_each(s.begin(), s.end(), [&structure](const Node* n) {
                std::set<const distribution::Distribution*> toMerge;
                gatherUnaries(toMerge, *n);
                distribution::DistributionPtr unaryMerged;
                if (1 == toMerge.size()) {
                    unaryMerged = std::make_shared<distribution::factor::cnst::Factor>(**toMerge.begin());
                }
                else if(!toMerge.empty()) {
                    unaryMerged = std::make_shared<distribution::factor::cnst::Factor>(toMerge);
                }
                NodeHidden node{ 0, unaryMerged, {} };
                for (auto itC = n->activeConnections.begin(); itC != n->activeConnections.end(); ++itC) {
                    node.connections.push_back(NodeHiddenConnection{ itC->second.factor, itC->first->variable, nullptr });
                }
                structure.emplace(n->variable, node);
            });
        });
        for (auto itS = structure.begin(); itS != structure.end(); ++itS) {
            for (auto itC = itS->second.connections.begin(); itC != itS->second.connections.end(); ++itC) {
                itC->neighbourSample = &structure.find(itC->neighbourVariable)->second.sample;
            }
        }
        return structure;
    }

    void GibbsSampler::evolveSamples(HiddenStructure& structure, std::size_t iterations, UniformSampler& sampler) {
        std::set<const distribution::Distribution*> toMerge;
        for (std::size_t i = 0; i < iterations; ++i) {
            for (auto it = structure.begin(); it != structure.end(); ++it) {
                toMerge.clear();
                if (nullptr != it->second.unaryMerged) {
                    toMerge.emplace(it->second.unaryMerged.get());
                }
                std::list<distribution::factor::cnst::Factor> marginalized;
                for (auto c = it->second.connections.begin(); c != it->second.connections.end(); ++c) {
                    marginalized.emplace_back(*c->factor, categoric::Combination(c->neighbourSample, 1), std::set<categoric::VariablePtr>{c->neighbourVariable});
                    toMerge.emplace(&marginalized.back());
                }
                if (1 == toMerge.size()) {
                    it->second.sample = sampler.sampleFromDiscrete((*toMerge.begin())->getProbabilities());
                }
                else {
                    it->second.sample = sampler.sampleFromDiscrete(distribution::factor::cnst::Factor(toMerge).getProbabilities());
                }
            }
        }
    }

    void GibbsSampler::convert(std::size_t* combData, const HiddenStructure& structure) {
        std::size_t k = 0;
        for (auto it = structure.begin(); it != structure.end(); ++it) {
            combData[k] = it->second.sample;
            ++k;
        }
    };

    std::vector<categoric::Combination> GibbsSampler::getHiddenSetSamples(std::size_t numberOfSamples, std::size_t deltaIteration) const {
        UniformSampler sampler;
        auto structure = this->getHiddenStructure();
        std::vector<categoric::Combination> samples;
        samples.reserve(numberOfSamples);
        // burn out
        evolveSamples(structure, 10 * deltaIteration, sampler);
        // compute samples
#ifdef THREAD_POOL_ENABLED
        if (nullptr != this->threadPool) {
            std::vector<std::vector<std::vector<std::size_t>>> samplesBatteries;
            std::list<HiddenStructure> structures;
            std::list<UniformSampler> samplers;
            samplesBatteries.resize(this->threadPool->size());
            std::size_t samplesCounter = 0, samplesCounterDelta = numberOfSamples / this->threadPool->size();
            for (std::size_t t = 0; t < this->threadPool->size(); ++t) {
                HiddenStructure* strPtr = &structure;
                UniformSampler* samplerPtr = &sampler;
                std::vector<std::vector<std::size_t>>* samplesContainer = &samplesBatteries[t];

                std::size_t threadIterations = std::min(numberOfSamples - samplesCounter, samplesCounterDelta);
                samplesContainer->reserve(threadIterations);
                if (0 < t) {
                    structures.push_back(this->getHiddenStructure());
                    strPtr = &structures.back();
                    // assume the same sample values
                    auto itSource = structure.cbegin();
                    for (auto itCopy = strPtr->begin(); itCopy != strPtr->end(); ++itCopy) {
                        itCopy->second.sample = itSource->second.sample;
                        ++itSource;
                    }
                    samplers.emplace_back();
                    samplerPtr = &samplers.back();
                    samplerPtr->resetSeed(t * 100);
                }
                this->threadPool->push([strPtr, samplesContainer, threadIterations, samplerPtr, &deltaIteration]() {
                    for (std::size_t s = 0; s < threadIterations; ++s) {
                        evolveSamples(*strPtr, deltaIteration, *samplerPtr);
                        samplesContainer->emplace_back(strPtr->size());
                        convert(samplesContainer->back().data(), *strPtr);
                    }
                });
                samplesCounter += threadIterations;
            }
            this->threadPool->wait();
            std::for_each(samplesBatteries.begin(), samplesBatteries.end(), [&samples](const std::vector<std::vector<std::size_t>>& b) {
                std::for_each(b.begin(), b.end(), [&samples](const std::vector<std::size_t>& c) {
                    samples.emplace_back(c.data(), c.size());
                });
            });
        }
        else {
#endif
            for (std::size_t s = 0; s < numberOfSamples; ++s) {
                evolveSamples(structure, deltaIteration, sampler);
                samples.emplace_back(structure.size());
                convert(samples.back().data() , structure);
            }
#ifdef THREAD_POOL_ENABLED
        }
#endif
        return samples;
    }
}
