/*
 * LifeSequentialImplementation.h
 */

#ifndef LIFESEQUENTIALIMPLEMENTATION_H_
#define LIFESEQUENTIALIMPLEMENTATION_H_

#include "Life.h"

class LifeSequentialImplementation: public Life {
protected:
	void realStep();
public:
	LifeSequentialImplementation();
	int numberOfLivingCells();
	double averagePollution();
	void oneStep();
};

#endif /* LIFESEQUENTIALIMPLEMENTATION_H_ */