#include "SimpleCouche.h"

SimpleCouche::SimpleCouche(size_t e, size_t s, size_t h, double (*_fonction_activation)(double)) : Reseau(e, s) {
	default_random_engine generator;
	uniform_real_distribution<double> dist(-1.0, 1.0);

	// Création des neurones cachés
	for (int i = 0; i < h; i++) {
		// On génère les connexions vers tous les neurones d'entrée
		vector<Connexion> ce;
		for (auto* it : entrees)
			ce.push_back(*new Connexion(it, dist(generator)));
		// On créé le neurone caché
		Neurone* n = new Neurone(_fonction_activation, ce);
		neurones.push_back(n);
	}

	// Gestion des neurones de sortie
	for (auto& it : sorties) {
		// On génère les connexions vers tous les neurones cachés
		vector<Connexion> cs;
		for (auto& it2 : neurones)
			cs.push_back(*new Connexion(it2, dist(generator)));

		it->aj_entrees(cs);
	}

	async_neurones = new boost::asio::thread_pool(neurones.size());
	async_sorties = new boost::asio::thread_pool(sorties.size());

}

void SimpleCouche::train(vector<double> e, vector<double> r) {
	set_entrees(e);
	eval();

	if (r.size() != sorties.size())
		throw ExceptionTaille();

	for (size_t i = 0; i < r.size(); i++) {
		for (size_t j = 0; j < sorties[i]->get_entrees().size(); j++) {
			sorties[i]->set_deltaW(j, 2 * (r[i] - sorties[i]->eval()) * sorties[i]->eval() * (1 - sorties[i]->eval()) * sorties[i]->get_entrees()[j].first->eval());
		}
		sorties[i]->app_deltaBiais(2 * (r[i] - sorties[i]->eval()) * sorties[i]->eval() * (1 - sorties[i]->eval()));
	}

	for (size_t i = 0; i < neurones.size(); i++) {
		double coeff = 0;
		// On recherche toutes les connexions avec les neurones de sortie
		for (size_t k = 0; k < sorties.size(); k++) {
			for (size_t l = 0; l < sorties[k]->get_entrees().size(); l++) {
				if (sorties[k]->get_entrees()[l].first == neurones[i]) {
					coeff += sorties[k]->get_entrees()[l].second *
						sorties[k]->eval() * (1 - sorties[k]->eval()) *
						2 * (r[k] - sorties[k]->eval());
				}
			}
		}

		for (size_t j = 0; j < neurones[i]->get_entrees().size(); j++) {
			double deltaW = coeff * neurones[i]->eval() * (1 - neurones[i]->eval()) * neurones[i]->get_entrees()[j].first->eval();
			neurones[i]->set_deltaW(j, deltaW);
		}
		neurones[i]->app_deltaBiais(coeff * neurones[i]->eval() * (1 - neurones[i]->eval()));
	}

	double sum = 0;
	for (int i = 0; i < sorties.size(); i++) {
		sum += pow(sorties[i]->eval() - r[i], 2);
	}

	for (auto& it : sorties)
		it->app_deltaW();
	for (auto& it : neurones)
		it->app_deltaW();
}

void SimpleCouche::log() {
	for (auto& it : neurones) {
		it->log();
	}
	for (auto& it : sorties) {
		it->log();
	}
}

void SimpleCouche::eval() {
	raz();

	for (auto* it : sorties)
		it->eval();
}