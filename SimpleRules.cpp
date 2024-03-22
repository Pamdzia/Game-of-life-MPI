/*
 * SimpleRules.cpp
 *
 *      Author: oramus
 */

#include "SimpleRules.h"

SimpleRules::SimpleRules() {
}

int SimpleRules::cellNextState(int cellCurrentState, int liveN, int currentPollution) {
	if ( ( currentPollution > 50 ) && ( liveN == 3 ) && ( cellCurrentState == 0 ) ) return 0; 
	if ( liveN == 3 ) return 1;
	if ( cellCurrentState && ( liveN == 2 ) ) return 1;
	return 0;
}

int SimpleRules::nextPollution(int cellCurrentState, int currentPollution, int pollutionSumNN,
							  int pollutionSumNNN) {
	
	int p = ( 2 * ( currentPollution + pollutionSumNNN ) + 3 * pollutionSumNN ) / 22 + INC * cellCurrentState;

	if ( p > MAX_POLLUTION) return MAX_POLLUTION;

	return p;
}