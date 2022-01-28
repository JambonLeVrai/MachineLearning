#include <iostream>
#include <time.h>
#include "SimpleCouche.h"


using namespace std;

int main(int argc, char** argv) {
	srand(time(nullptr));

	SimpleCouche p(3, 2, 5, *Activation::Sigmoid);

	for (unsigned int i = 0; i < 1500; i++) {
		unsigned int a = (i % 2);
		unsigned int b = floor((double)(i % 4) / 2);
		unsigned int c = floor((double)(i % 8) / 4);
		p.train({ (double)a,(double)b,(double)c}, { (double)(a ^ b ^ c), (double)((a & b) | (a & c) | (b & c)) });
	}

	p.log();

	vector<double> a = { 0.0, 0.0, 0.0 };
	while (true) {
		cin >> a[0];
		cin >> a[1];
		cin >> a[2];
		p.set_entrees(a);

		for (auto& it : p.get_sorties())
			cout << it << endl;
	}

	return 0;
}