/*
 * SimpleRules.h
 *
 * Author: oramus
 */

#ifndef SIMPLERULES_H_
#define SIMPLERULES_H_

#include"Rules.h"

#define MAX_POLLUTION 255
#define INC 10

class SimpleRules : public Rules {
public:
	SimpleRules();
	int cellNextState(int cellCurrentState, int liveN, int currentPollution);
	int nextPollution(int cellCurrentState, int currentPollution, int pollutionSumNN,
							  int pollutionSumNNN);
	int getMaxPollution() {
		return MAX_POLLUTION;
	}
};

#endif /* RANDOMRULES_H_ */