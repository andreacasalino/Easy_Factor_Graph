/**
 * Author:    Andrea Casalino
 * Created:   01.01.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#ifndef EFG_STRUCTURE_BELIEF_PROPAGATOR_H
#define EFG_STRUCTURE_BELIEF_PROPAGATOR_H

#include <structure/components/EvidenceAware.h>
#include <structure/components/BeliefAware.h>
#ifdef THREAD_POOL_ENABLED
#include <structure/components/ThreadPoolAware.h>
#endif

namespace EFG::strct {
    class BeliefPropagator
        : virtual public EvidenceAware
        , virtual public BeliefAware
#ifdef THREAD_POOL_ENABLED
        , virtual public ThreadPoolAware
#endif
        {
    protected:
        void propagateBelief(const PropagationKind& kind) override;

    private:
        bool messagePassing(const std::set<Node*>& cluster, const PropagationKind& kind);
        bool loopyPropagation(const std::set<Node*>& cluster, const PropagationKind& kind);

#ifdef THREAD_POOL_ENABLED
        bool messagePassingThreadPool(const std::set<Node*>& cluster, const PropagationKind& kind);
        bool loopyPropagationThreadPool(const std::set<Node*>& cluster, const PropagationKind& kind);
#endif
    };
}

#endif
