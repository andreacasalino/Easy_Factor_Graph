/**
 * Author:    Andrea Casalino
 * Created:   04.06.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#ifndef EFG_TRAINER_GRADIENT_DESCEND_FIXED_H
#define EFG_TRAINER_GRADIENT_DESCEND_FIXED_H

#include <trainers/components/IterativeDescend.h>
#include <trainers/strategies/BasicTrainSet.h>
#include <Error.h>

namespace EFG::train {
    template<typename TrainSetT = BasicTrainSet>
    class GradientDescendFixed
        : public IterativeDescend
        , public TrainSetT {
        static_assert(std::is_base_of<BasicTrainSet, TrainSetT>::value, "TrainSetT should be a form of BasicTrainSet");
    public:
        void setStep(const float step) { 
            if (step <= 0.f) {
                throw Error("Invalid step");
            }
            this->step = step; 
        };

    protected:
        void descend() override {
            auto direction = this->model->getGradient();
            auto w = this->model->getWeights();
            direction *= this->step;
            w -= direction;
            this->model->setWeights(w);
        };

    private:
        float step = 1.f;
    };
}

#endif
