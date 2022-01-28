#pragma once
#include "Neurone.h"
#include <vector>
#include <random>
#include <boost/asio/thread_pool.hpp>
#include <boost/asio/post.hpp>

struct ExceptionTaille : public exception {
	const char* what() const throw () {
		return "La taille des tableaux ne correspond pas";
	}
};

class Reseau
{
public:
	Reseau(size_t e, size_t s, double (*_fonction_activation)(double) = *Activation::Sigmoid);
	void set_entrees(vector<double> e);
	vector<double> get_sorties();
	void raz();
	virtual void train(vector<double> e, vector<double> r) {}

protected:
	virtual void eval();

	vector<NeuroneEntree*> entrees;
	vector<Neurone*> neurones;
	vector<Neurone*> sorties;
};