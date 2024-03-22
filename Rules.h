/*
 * Rules.h
 */

#ifndef RULES_H_
#define RULES_H_

class Rules
{
public:
	Rules();
	virtual int cellNextState(int cellCurrentState, int liveN, int currentPollution) = 0;
	virtual int nextPollution(int cellCurrentState, int currentPollution, int pollutionSumNN,
							  int pollutionSumNNN) = 0;
	virtual int getMaxPollution() = 0;
};

#endif /* RULES_H_ */