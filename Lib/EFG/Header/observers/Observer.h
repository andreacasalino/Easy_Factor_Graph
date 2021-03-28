/**
 * Author:    Andrea Casalino
 * Created:   01.01.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#ifndef EFG_OBSERVER_H
#define EFG_OBSERVER_H

#include <observers/Subject.h>

namespace EFG {
    class Observer {
    public:
        Observer(const Subject& subject);
        ~Observer();

    private:
        ObserverCounterPtr counter;
    };
}

#endif