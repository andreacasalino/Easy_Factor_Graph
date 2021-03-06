/**
 * Author:    Andrea Casalino
 * Created:   04.06.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#ifdef ADVANCED_TRAINERS_ENABLED
#ifndef EFG_TRAINER_HESSIAN_APPROXIMATOR_H
#define EFG_TRAINER_HESSIAN_APPROXIMATOR_H

#include <trainers/components/ModelAware.h>
#include <trainers/components/Updatable.h>
#include <trainers/Commons.h>

namespace EFG::train {
    /**
     * @brief The base for an object storing the local hessian approximation of the function to optimize
     */
    class HessianApproximator
        : public virtual ModelAware
        , public virtual Updatable {
    protected:
        void update() override;
        void reset() override;

        virtual void updateInvHessian(const Vect& deltaWeight, const Vect& deltaGrad) = 0;

        Matr invHessianApprox = Matr(1);
    };
}

#endif
#endif
