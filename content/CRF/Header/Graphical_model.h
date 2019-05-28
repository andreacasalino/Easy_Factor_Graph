//Author: Andrea Casalino
//mail: andrecasa91@gmail.com

#pragma once
#ifndef __CRF_GRAPHICAL_MODEL_H__
#define __CRF_GRAPHICAL_MODEL_H__

#include "Node.h"

namespace Segugio {

	/*!
	 * \brief Interface for managing generic graphs.
	 * \details Both Exponential and normal shapes can be included
	 * into the model. Learning is not possible: all belief propagation 
	 * operations are performed assuming the mdoel as is.
	 * Every Potential_Shape or Potential_Exp_Shape inserted into the model,
	 * is wrapped and is automatically destroyed when that instance of Graph
	 * is destroyed.
	 */
	class Graph : public Node::Node_factory {
	public:
		/** \brief empty constructor
		*/
		Graph() : Node_factory() {};
		/** \brief The model is built considering the information contained in an xml configuration file.
		* \details TODO spiegare come � fatto xml
		* @param[in] configuration file
		* @param[in] prefix to use. The file prefix_config_xml_file/config_xml_file is searched.
		*/
		Graph(const std::string& config_xml_file, const std::string& prefix_config_xml_file = "");

		/** \brief The model is built considering the information contained in an xml configuration file
		* @param[in] the potential to insert. It can be a unary or a binary potential. In case it is binary, at least one 
		* of the variable involved must be already inserted to the model before (with a previous Insert having as input 
		* a potential which involves that variable).
		*/
		void Insert(Potential_Shape* pot) { this->Node_factory::Insert(pot); };
		/** \brief The model is built considering the information contained in an xml configuration file
		* @param[in] the potential to insert. It can be a unary or a binary potential. In case it is binary, at least one
		* of the variable involved must be already inserted to the model before (with a previous Insert having as input
		* a potential which involves that variable).
		*/
		void Insert(Potential_Exp_Shape* pot) { this->Node_factory::Insert(pot); };

		void Set_Observation_Set_var(const std::list<Categoric_var*>& new_observed_vars) { this->Node_factory::Set_Observation_Set_var(new_observed_vars); };
		void Set_Observation_Set_val(const std::list<size_t>& new_observed_vals) { this->Node_factory::Set_Observation_Set_val(new_observed_vals); };
	};


	class I_Learning_handler : public I_Potential_Decorator, public Potential_Exp_Shape::Handler_weight {
	public:
		void			Get_weight(float* w) { *w = *this->pWeight; };
		void			Set_weight(const float& w_new) { *this->pWeight = w_new; };

		void			Get_grad_alfa_part(float* alfa, std::list<size_t*>* comb_in_train_set, std::list<Categoric_var*>* comb_var);
		virtual void    Get_grad_beta_part(float* beta) = 0; //according to last performed belief propagation

		void			Cumul_Log_Activation(float* result, size_t* val_to_consider, const std::list<Categoric_var*>& var_in_set);
	protected:
		I_Learning_handler(Potential_Exp_Shape* pot_to_handle);
		I_Learning_handler(I_Learning_handler* other);
	// data
		float*								          pWeight;
	// cache
		std::list<I_Distribution_value*>			  Extended_shape_domain; //for computing beta part of gradient
	private:
		Potential_Exp_Shape*   ref_to_wrapped_exp_potential;
	};



	class Graph_Learnable : public Node::Node_factory {
	public:
		Graph_Learnable() : Node::Node_factory(), pLast_train_set(NULL) {};
		~Graph_Learnable();

		struct Weights_Manager {
			friend class I_Trainer;
		public:
			static void Get_w(std::list<float>* w, Graph_Learnable* model);
		private:
			static void Get_w_grad(std::list<float>* grad_w, Graph_Learnable* model, std::list<size_t*>* comb_train_set, std::list<Categoric_var*>* comb_var_order);
			static void Set_w(const std::list<float>& w, Graph_Learnable* model);
		};
		size_t Get_model_size() { return this->Model_handlers.size(); };
		virtual void Get_Likelihood_estimation(float* result, std::list<size_t*>* comb_train_set, std::list<Categoric_var*>* comb_var_order) = 0;
	
	protected:
	
		void Get_Log_activation(float* result, size_t* Y, std::list<Categoric_var*>* Y_var_order);

		void Insert(Potential_Exp_Shape* pot);
		void Insert(Potential_Shape* pot);

	// data
		std::list<I_Learning_handler*>   Model_handlers;
	private:

		//as baseline behaviour the alfa part of gradient is recomputed in case train set has changed, and is added to the result
		virtual void Get_w_grad(std::list<float>* grad_w, std::list<size_t*>* comb_train_set, std::list<Categoric_var*>* comb_var_order);

	// cache for gradient computation
		std::list<float>			Alfa_part_gradient;
		std::list<size_t*>*			pLast_train_set;
	};



	class Random_Field : public Graph_Learnable {
	public:
		Random_Field() : Graph_Learnable() {};
		Random_Field(const std::string& config_xml_file, const std::string& prefix_config_xml_file = "");

		void Insert(Potential_Exp_Shape* pot) { this->Graph_Learnable::Insert(pot); };

		void Set_Observation_Set_var(const std::list<Categoric_var*>& new_observed_vars) { this->Node_factory::Set_Observation_Set_var(new_observed_vars); };
		void Set_Observation_Set_val(const std::list<size_t>& new_observed_vals) { this->Node_factory::Set_Observation_Set_val(new_observed_vals); };
	private:
		void Get_w_grad(std::list<float>* grad_w, std::list<size_t*>* comb_train_set, std::list<Categoric_var*>* comb_var_order);
		void Get_Likelihood_estimation(float* result, std::list<size_t*>* comb_train_set, std::list<Categoric_var*>* comb_var_order);
	};



	class Conditional_Random_Field : public Graph_Learnable {
	public:
		Conditional_Random_Field(const std::string& config_xml_file, const std::string& prefix_config_xml_file = "");
		Conditional_Random_Field(const std::list<Potential_Exp_Shape*>& potentials, const std::list<Categoric_var*>& observed_var);

		void Set_Observation_Set_val(const std::list<size_t>& new_observed_vals) { this->Node_factory::Set_Observation_Set_val(new_observed_vals); };
	private:
		void Get_w_grad(std::list<float>* grad_w, std::list<size_t*>* comb_train_set, std::list<Categoric_var*>* comb_var_order);
		void Get_Likelihood_estimation(float* result, std::list<size_t*>* comb_train_set, std::list<Categoric_var*>* comb_var_order);
	};

}

#endif