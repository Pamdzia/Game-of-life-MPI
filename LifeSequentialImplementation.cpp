/*
 * LifeSequentialImplementation.cpp
 *
 */

#include "LifeSequentialImplementation.h"
#include <stdlib.h>

LifeSequentialImplementation::LifeSequentialImplementation()
{
}

void LifeSequentialImplementation::realStep()
{
	int currentState, currentPollution;
	for (int row = 1; row < size_1; row++)
		for (int col = 1; col < size_1; col++)
		{
			currentState = cells[row][col];
			currentPollution = pollution[row][col];
			cellsNext[row][col] = rules->cellNextState(currentState, liveNeighbours(row, col),
													   currentPollution);
			pollutionNext[row][col] =
				rules->nextPollution(currentState, currentPollution, pollution[row + 1][col] + pollution[row - 1][col] + pollution[row][col - 1] + pollution[row][col + 1],
									 pollution[row - 1][col - 1] + pollution[row - 1][col + 1] + pollution[row + 1][col - 1] + pollution[row + 1][col + 1]);
		}
}

void LifeSequentialImplementation::oneStep()
{
	realStep();
	swapTables();
}

int LifeSequentialImplementation::numberOfLivingCells() {
	return sumTable( cells );
}

double LifeSequentialImplementation::averagePollution() {
	return (double)sumTable( pollution ) / size_1_squared / rules->getMaxPollution();
}