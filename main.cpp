#include <iostream>
#include <time.h>
#include "SimpleCouche.h"


using namespace std;

int main(int argc, char** argv) {
	srand(time(nullptr));

	/*
	// Additionneur
	SimpleCouche p(3, 2, 20, *Activation::Sigmoid);

	for (unsigned int i = 0; i < 1500; i++) {
		unsigned int a = (i % 2);
		unsigned int b = floor((double)(i % 4) / 2);
		unsigned int c = floor((double)(i % 8) / 4);
		p.train({ (double)a,(double)b,(double)c}, { (double)(a ^ b ^ c), (double)((a & b) | (a & c) | (b & c)) });
	}*/

	SimpleCouche p(2, 1, 2, *Activation::Sigmoid);

	for (unsigned int i = 0; i < 150000; i++) {
		unsigned int a = (i % 2);
		unsigned int b = floor((double)(i % 4) / 2);
		p.train({ (double)a,(double)b }, { (double)(a ^ b)});
	}

	p.log();

	vector<double> a = { 0.0, 0.0 };
	while (true) {
		cin >> a[0];
		cin >> a[1];
		p.set_entrees(a);

		for (auto& it : p.get_sorties())
			cout << it << endl;
	}

	return 0;
}