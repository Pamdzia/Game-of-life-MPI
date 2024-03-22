/*
 * Main.cpp
 */

#include "Life.h"
#include "LifeParallelImplementation.h"
#include "LifeSequentialImplementation.h"
#include "Rules.h"
#include "SimpleRules.h"
#include "Alloc.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>

using namespace std;

void glider(Life *l, int row, int col)
{
	l->bringToLife(row, col);
	l->bringToLife(row + 1, col);
	l->bringToLife(row + 2, col);
	l->bringToLife(row, col + 1);
	l->bringToLife(row + 1, col + 2);
}

void lineV(Life *l, int row, int col, int length)
{
	for (int c = 0; c < length; c++)
		l->bringToLife(row + c, col);
}

void lineH(Life *l, int row, int col, int length)
{
	for (int c = 0; c < length; c++)
		l->bringToLife(row, col + c);
}

void hwss(Life *l, int row, int col)
{
	lineH(l, row, col, 6);
	l->bringToLife(row - 1, col);
	l->bringToLife(row - 2, col);
	l->bringToLife(row - 3, col + 1);
	l->bringToLife(row - 4, col + 3);
	l->bringToLife(row - 4, col + 4);
	l->bringToLife(row - 1, col + 6);
	l->bringToLife(row - 3, col + 6);
}

void simulationInit(Life *life)
{
	lineH(life, 10, 30, 40);
	lineH(life, 30, 30, 340);
	lineV(life, 5, 50, 31);
	glider(life, 60, 40);
	hwss(life, 70, 70);
	hwss(life, 70, 80);
}

int main(int argc, char **argv)
{
	srand(time(NULL));
	const int simulationSize = 100;
	const int steps = 100;
	double start;
	int procs, rank;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &procs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	Rules *rules = new SimpleRules();

	Life *lifes = new LifeSequentialImplementation();
	lifes->setRules(rules);
	lifes->setSize(simulationSize);

	Life *lifep = new LifeParallelImplementation();
	lifep->setRules(rules);
	lifep->setSize(simulationSize);

	for(int i=1;i<simulationSize-1;i++)
	{
		for(int j=1;j<simulationSize-1;j++)
		{
			if(rand()%2==0)
			{
				lifep->bringToLife(i, j);
				lifes->bringToLife(i, j);
			}
		}
	}

	if (!rank)
	{
		start = MPI_Wtime();
	}


	lifes->beforeFirstStep();
	for (int t = 0; t < steps; t++)
	{
		lifes->oneStep();
	}
	lifes->afterLastStep();

	if (!rank)
	{
		int livingCells = lifes->numberOfLivingCells();
		double averagePollution = 100.0 * lifes->averagePollution();
		double end = MPI_Wtime();
		int cellsTotal = (simulationSize - 2) * (simulationSize - 2);
		int ram = 2 * simulationSize * simulationSize * sizeof(int);
		int oneBorder = 4 * simulationSize * sizeof( int );

		cout << "-------------------------------------" << endl;
		cout << "  LifeSequentialImplementation  " << endl;
		cout << "-------------------------------------" << endl;

		cout << "MPI size         : " << procs << endl;
		cout << "Total cells      : " << cellsTotal << endl;
		cout << "RAM for tables   : " << ram / 1024 << "KB" << endl;
		cout << "Border size      : " << oneBorder / 1024 << "KB" << endl;
		cout << "Living cells     : " << livingCells << endl;
		cout << "Avg pollution    : " << averagePollution << "%" << endl;
		cout << "Simulation size  : " << simulationSize << endl;
		cout << "Simulation steps : " << steps << endl;
		cout << "Simulation time  : " << (end - start) << " sek. " << endl;
		cout << "Time per step    : " << (end - start) / steps << " sek. " << endl;
		cout << "pollution@(10,10): " << lifes->getPollution(10,10) << endl;
		cout << "cell@(10,10)     : " << lifes->getCellState(10,10) << endl;

	}


	if (!rank)
	{
		start = MPI_Wtime();
	}

	lifep->beforeFirstStep();
	for (int t = 0; t < steps; t++)
	{
		lifep->oneStep();
	}
	lifep->afterLastStep();

	if (!rank)
	{
		int livingCells = lifep->numberOfLivingCells();
		double averagePollution = 100.0 * lifep->averagePollution();
		double end = MPI_Wtime();
		int cellsTotal = (simulationSize - 2) * (simulationSize - 2);
		int ram = 2 * simulationSize * simulationSize * sizeof(int);
		int oneBorder = 4 * simulationSize * sizeof( int );

		cout << "-------------------------------------" << endl;
		cout << "      LifeParallelImplementation     " << endl;
		cout << "-------------------------------------" << endl;

		cout << "MPI size         : " << procs << endl;
		cout << "Total cells      : " << cellsTotal << endl;
		cout << "RAM for tables   : " << ram / 1024 << "KB" << endl;
		cout << "Border size      : " << oneBorder / 1024 << "KB" << endl;
		cout << "Living cells     : " << livingCells << endl;
		cout << "Avg pollution    : " << averagePollution << "%" << endl;
		cout << "Simulation size  : " << simulationSize << endl;
		cout << "Simulation steps : " << steps << endl;
		cout << "Simulation time  : " << (end - start) << " sek. " << endl;
		cout << "Time per step    : " << (end - start) / steps << " sek. " << endl;
		cout << "pollution@(10,10): " << lifep->getPollution(10,10) << endl;
		cout << "cell@(10,10)     : " << lifep->getCellState(10,10) << endl;

	}

	MPI_Finalize();
	return 0;
}