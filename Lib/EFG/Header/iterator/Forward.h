/**
 * Author:    Andrea Casalino
 * Created:   01.01.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#ifndef EFG_ITERATOR_FORWARD_H
#define EFG_ITERATOR_FORWARD_H

#include <cstddef> // std::nullptr_t

namespace EFG::iterator {
    /** @brief A Forward iterable object.
     */
    class Forward {
    public:
        virtual void operator++() = 0;

        /** @return true when the iterator is at the end, i.e. can't be incremented further.
         */
        virtual bool operator==(std::nullptr_t) const = 0;

        inline bool operator!=(std::nullptr_t) const { return !(*this == nullptr); };
    };

    /** @brief takes an iterator to increment till the end, calling at every iteration the passed action.
     *  @param the iterator to increment
     *  @param an action taking as input the iterator for every iteration
     */
    template<typename Iter, typename Action>
    void forEach(Iter& iter, Action action) {
        while (iter != nullptr) {
            action(iter);
            ++iter;
        }
    };

    /** @brief similar to forEach(...), but in this case the action should be a predicate, taking 
     * as input the iterator and returning true when the loop should be terminated before reaching
     * the end of the iterator.
     *  @param the iterator to increment
     *  @param an action taking as input the iterator for every iteration and returning true when the loop should break
     */
    template<typename Iter, typename ActionCondition>
    void forEachConditioned(Iter& iter, ActionCondition action) {
        while ((iter != nullptr) &&
                !action(iter)) {
            ++iter;
        }
    };
}

#endif
