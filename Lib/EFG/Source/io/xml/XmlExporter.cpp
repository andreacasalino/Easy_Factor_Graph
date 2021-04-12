/**
 * Author:    Andrea Casalino
 * Created:   01.01.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#include <io/xml/XmlExporter.h>
#include <distribution/DistributionIterator.h>
#include <nodes/bases/StructureTunableAware.h>
#include <Parser.h>
#include <algorithm>

namespace EFG::nodes {
	void printGroup(const categoric::Group& group, xmlPrs::Tag& tag) {
		std::for_each(group.getVariables().begin(), group.getVariables().end(), [&tag](const categoric::VariablePtr& v) {
			tag.getAttributes().emplace("var", v->name());
		});
	};

	xmlPrs::Tag& printPotential(const distribution::Distribution& distr, xmlPrs::Tag& tag) {
		auto& pot_tag = tag.addNested("Potential");
		printGroup(distr.getGroup(), pot_tag);
		auto it = distr.getIterator();
		iterator::forEach(it, [&pot_tag](const distribution::DistributionIterator& it) {
			auto& temp = pot_tag.addNested("Distr_val");
			for (std::size_t k = 0; k < it.getCombination().size(); ++k) temp.getAttributes().emplace("v", std::to_string(it.getCombination().data()[k]));
			temp.getAttributes().emplace("D", std::to_string(it.getImageRaw()));
		});
		return pot_tag;
	};

    void XmlExporter::exportToXml(const std::string& path, const std::string& name) const {
		xmlPrs::Parser exporter;
		xmlPrs::Tag& exp_root = exporter.getRoot();
		if (name.empty()) {
			exp_root.setName("graphical-model");
		}
		else {
			exp_root.setName(name);
		}
		// hidden set
		auto H_vars = this->getHiddenVariables();
		std::for_each(H_vars.begin(), H_vars.end(), [&exp_root](const categoric::VariablePtr& v) {
			auto& temp = exp_root.addNested("Variable");
			temp.getAttributes().emplace("name", v->name());
			temp.getAttributes().emplace("Size", std::to_string(v->size()));			
		});
		// evidence set
		auto O_vars = this->getEvidences();
		for (auto itO = this->evidences.begin(); itO != this->evidences.end(); ++itO) {
			auto& temp = exp_root.addNested("Variable");
			temp.getAttributes().emplace("name", itO->first->name());
			temp.getAttributes().emplace("Size", std::to_string(itO->first->size()));
			temp.getAttributes().emplace("flag", "O");
		}
		// factors
		std::for_each(this->factors.begin(), this->factors.end(), [&exp_root](const distribution::factor::cnst::Factor* f) {
			printPotential(*f, exp_root);
		});
		// exp factors
		std::for_each(this->factorsExp.begin(), this->factorsExp.end(), [&exp_root](const distribution::factor::cnst::FactorExponential* f) {
			auto& temp = printPotential(*f, exp_root);
			temp.getAttributes().emplace("weight", std::to_string(f->getWeight()));
			temp.getAttributes().emplace("tunability", "N");
		});
		// exp tunable factors
		const StructureTunableAware* tunabStructurePtr = dynamic_cast<const StructureTunableAware*>(this);
		if (nullptr == tunabStructurePtr) {
			return;
		}
		std::for_each(tunabStructurePtr->getFactorsTunable().begin(), tunabStructurePtr->getFactorsTunable().end(), [&exp_root](const std::vector<std::shared_ptr<distribution::factor::modif::FactorExponential>>& cluster) {
			auto itCl = cluster.begin();
			auto& temp = printPotential(**itCl, exp_root);
			temp.getAttributes().emplace("weight", std::to_string((*itCl)->getWeight()));
			const auto& varsFront = (*itCl)->getGroup();
			++itCl;
			std::for_each(itCl, cluster.end(), [&exp_root, &varsFront](const std::shared_ptr<distribution::factor::modif::FactorExponential>& f) {
				auto& temp = printPotential(*f, exp_root);
				temp.getAttributes().emplace("weight", std::to_string(f->getWeight()));
				auto& shareTag = temp.addNested("Share");
				printGroup(varsFront, shareTag);
			});
		});

		exporter.reprint(path);
    }
}
