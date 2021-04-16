/**
 * Author:    Andrea Casalino
 * Created:   01.01.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#include <nodes/InsertTunableCapable.h>
#include <Error.h>
#include <algorithm>

namespace EFG::nodes {
    void InsertTunableCapable::Insert(std::shared_ptr<distribution::factor::modif::FactorExponential> toInsert) {
        this->InsertCapable::Insert(toInsert);
        this->factorsExp.erase(this->factorsExp.find(toInsert.get()));
        this->factorsTunable.emplace(toInsert.get(), this->numberOfClusters);
        ++this->numberOfClusters;
    }

    void InsertTunableCapable::Insert(const distribution::factor::modif::FactorExponential& factor) {
        std::shared_ptr<distribution::factor::modif::FactorExponential> distr = std::make_shared<distribution::factor::modif::FactorExponential>(factor);
        distr->replaceGroup(this->convertUsingLocals(factor.getGroup()));
        this->Insert(distr);
    }

    std::map<distribution::factor::modif::FactorExponential*, std::size_t>::const_iterator InsertTunableCapable::findSharingFactor(const categoric::Group& potentialSharingWeight) const {
        for (auto it = this->factorsTunable.begin(); it != this->factorsTunable.end(); ++it) {
            if (it->first->getGroup() == potentialSharingWeight) {
                return it;
            }
        }
        throw Error("inexistent factor for sharing the weight");
    }

    void InsertTunableCapable::Insert(std::shared_ptr<distribution::factor::modif::FactorExponential> toInsert, const categoric::Group& potentialSharingWeight) {
        auto it = this->findSharingFactor(this->convertUsingLocals(potentialSharingWeight));
        this->InsertCapable::Insert(toInsert);
        this->factorsExp.erase(this->factorsExp.find(toInsert.get()));
        this->factorsTunable.emplace(toInsert.get(), it->second);
    }

    void InsertTunableCapable::Insert(const distribution::factor::modif::FactorExponential& factor, const categoric::Group& potentialSharingWeight) {
        std::shared_ptr<distribution::factor::modif::FactorExponential> distr = std::make_shared<distribution::factor::modif::FactorExponential>(factor);
        distr->replaceGroup(this->convertUsingLocals(factor.getGroup()));
        this->Insert(distr, potentialSharingWeight);
    }
}
