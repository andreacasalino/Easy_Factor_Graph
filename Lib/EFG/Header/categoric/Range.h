/**
 * Author:    Andrea Casalino
 * Created:   01.01.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#ifndef EFG_CATEGORIC_RANGE_H
#define EFG_CATEGORIC_RANGE_H

#include <categoric/Group.h>
#include <iterator/Forward.h>
#include <vector>

namespace EFG::categoric {
    /**
     * @brief This object allows you to iterate all the elements in the joint domain of a group of variables, without having to precompute all the elements in such domain.
     * For example when having a domain made by variables = { A (size = 2), B (size = 3), C (size  = 2)  }, the elements in the
     * joint domain that will be iterated are:
     * <0,0,0>
     * <0,0,1>
     * <0,1,0>
     * <0,1,1>
     * <0,2,0>
     * <0,2,1>
     * <1,0,0>
     * <1,0,1>
     * <1,1,0>
     * <1,1,1>
     * <1,2,0>
     * <1,2,1>
     * The Range object starts to point to the first element in the joint domain after construction. Then, when incrementing the object,
     * the following element is pointed. When calling get() the current pointed element can be accessed.
     */
    class Range : public iterator::Forward {
    public:
        /** @param the group of variables whose joint domain must be iterated 
         */
        explicit Range(const std::set<VariablePtr>& group);

        Range(const Range& ) = default;
        Range& operator=(const Range& ) = default;

        /** @param the group of variables whose joint domain must be iterated.
         */
        inline const std::vector<std::size_t>& get() const { return this->combination; };

        /** @brief Make the object to point to the next element in the joint domain.
         *  @throw if the current pointed element is the last one.
         */
        void operator++() final;

        inline bool operator==(std::nullptr_t) const final { return this->isAtEnd; };

        /** @brief Make the object to point to the first element of the joint domain, i.e.
         * reset the status as the initial one after construction.
         */
        inline void reset() { this->combination = std::vector<std::size_t>(this->sizes.size(), 0); this->isAtEnd = false; };

    private:
        std::vector<size_t>		sizes;
        std::vector<size_t>		combination;
        bool                    isAtEnd = false;
    };
}

#endif
