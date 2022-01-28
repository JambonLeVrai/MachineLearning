#pragma once
#include "Reseau.h"
#include <vector>


class SimpleCouche : public Reseau
{
public:
	SimpleCouche(size_t e, size_t s, size_t h, double (*_fonction_activation)(double) = *Activation::Sigmoid);
	void train(vector<double> e, vector<double> r);
	void log();
private:
	void eval();

	boost::asio::thread_pool* async_neurones, * async_sorties;
};