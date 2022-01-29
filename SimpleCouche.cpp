#include "SimpleCouche.h"

SimpleCouche::SimpleCouche(size_t e, size_t s, size_t h, double (*_fonction_activation)(double)) : Reseau(e, s) {
	default_random_engine generator;
	uniform_real_distribution<double> dist(-1.0, 1.0);

	// Création des neurones cachés
	for (int i = 0; i < h; i++) {
		// On créé les neurones cachés
		Neurone* n = new Neurone(_fonction_activation);
		neurones.push_back(n);

		// On créé les connexions vers toutes les entrées
		for (auto entree : entrees) {
			Axone* a = new Axone(entree, n, dist(generator));
			axones.push_back(a);
			n->aj_entree(a);
			entree->aj_sortie(a);
		}
	}

	// Gestion des neurones de sortie
	for (auto& sortie : sorties) {
		// On créé tous les axones entre la couche cachée et la couche de sortie
		for (auto& cache : neurones) {
			Axone* a = new Axone(cache, sortie, dist(generator));
			axones.push_back(a);
			cache->aj_sortie(a);
			sortie->aj_entree(a);
		}
	}

}

void SimpleCouche::train(vector<double> e, vector<double> r) {
	set_entrees(e);
	eval();

	if (r.size() != sorties.size())
		throw ExceptionTaille();

	for (size_t i = 0; i < r.size(); i++) {
		for (size_t j = 0; j < sorties[i]->get_entrees().size(); j++) {
			sorties[i]->get_entrees()[j]->set_delta_poids(2 * (r[i] - sorties[i]->eval()) * sorties[i]->eval() * (1 - sorties[i]->eval()) * sorties[i]->get_entrees()[j]->get_entree()->eval());
		}
		sorties[i]->app_deltaBiais(2 * (r[i] - sorties[i]->eval()) * sorties[i]->eval() * (1 - sorties[i]->eval()));
	}

	for (size_t i = 0; i < neurones.size(); i++) {
		double coeff = 0;
		for (size_t j = 0; j < neurones[i]->get_sorties().size();j++) {
			Axone* axone = neurones[i]->get_sorties()[j];
			double t = axone->get_sortie()->eval();
			coeff += axone->get_poids() * t * (1 - t) * 2 * (r[j] - t);
		}

		for (auto entree : neurones[i]->get_entrees()) {
			double e = neurones[i]->eval();
			double deltaW = coeff * e * (1 - e) * entree->get_entree()->eval();
			entree->set_delta_poids(deltaW);
		}

		neurones[i]->app_deltaBiais(coeff * neurones[i]->eval() * (1 - neurones[i]->eval()));
	}

	double sum = 0;
	for (int i = 0; i < sorties.size(); i++) {
		sum += pow(sorties[i]->eval() - r[i], 2);
	}

	for (auto it : axones)
		it->app_delta_poids();
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