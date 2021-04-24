/**
 * Author:    Andrea Casalino
 * Created:   01.01.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#include <categoric/Range.h>
#include <Error.h>

namespace EFG::categoric {
    Range::Range(const std::set<VariablePtr>& group) {
        this->sizes.reserve(group.size());
        for(auto it = group.begin(); it!=group.end(); ++it) {
            this->sizes.push_back((*it)->size());
        }
        this->combination = std::vector<std::size_t>(this->sizes.size() , 0);
    }

    void Range::operator++() {
        std::size_t k = this->combination.size() - 1;
		while (true) {
			++this->combination[k];
			if (this->combination[k] == this->sizes[k]) {
				if (k == 0) {
					this->isAtEnd = true;
					break;
				}
				else {
					this->combination[k] = 0;
					--k;
				}
			}
			else break;
		}
    };
}
