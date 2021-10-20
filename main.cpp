// ***************************************
// Worm Chemotaxis
// ***************************************

#include "CTRNN.h"
#include "WormAgent.h"
#include "TSearch.h"
#include <sstream>

//#define EVOLVE
//#define PRINTTOFILE

//#define PSON
//#define PSOFF
//#define PSBOTH

//#define ONKILL
//#define OFFKILL

// Global constants
const int		CircuitSize				= 2;
const double	RunDuration				= 500.0;
const int		MaxRepetitions			= 5;
const int		MaxReplacements			= 10;
const double	WeightRange				= 15.0;
const double	SensoryWeightRange		= 1500.0;
const double	BiasRange				= 15.0;
const double	CloseEnoughRadius		= 0.5;
const double	MinSensorN				= 10*StepSize;
const double	MaxSensorN				= HST;
const double	MinSensorM				= 10*StepSize;
const double	MaxSensorM				= HST;
const double	MinSensorD				= 10*StepSize;
const double	MaxSensorD				= HST;
const double	MinNeckTurnGain			= 1.0;
const double	MaxNeckTurnGain			= 3.0;
//const double	PirouetteProb			= 0.000333333; //(2/60)*0.01; // They reverse about twice a minute (according to Shawn and Serge). In 500 secs, that's 16.66 on avg.
const double	PirouetteProb			= 0.00004; // Smaller. So that it doesn't evolve to depend on it. On avg, 2 per trial (500secs) [(2/500) * 0.01]
const double	SizeOfStep				= 0.002;	// Size of steps for the riverchip experiment

const double	RiverChipSteepness		= 50.0;

// Global variables
int	VectSize = 9;

// ------------------------------------
// Genotype-Phenotype Mapping Functions
// ------------------------------------


// ------------------------------------
// Behavioral Analysis
// ------------------------------------
void ExampleRun()
{
	ofstream ExampleFile,PirFile;
	double t;
	long IDUM=time(0);
	int repetitions,replacements;
	RandomState rs;
	rs.SetRandomSeed(IDUM);
	ExampleFile.open("/Users/carlos/Desktop/Exploration_Klino/examples.dat");
	PirFile.open("/Users/carlos/Desktop/Exploration_Klino/pirxy.dat");
	WormAgent Worm("/Users/carlos/Desktop/Exploration_Klino/best.ns.dat");
	Worm.InitialiseAgent(StepSize);

	for (repetitions = 1; repetitions <= 1; repetitions++)
	{
		Worm.ResetAgentIntState(rs);
		//cout << rs.UniformRandom(0.0, 0.0) << endl; // distribucion uniforme entre cero y cero: siempre devuelve cero 
		cout << Worm.NervousSystem.states[1] << endl;
		for (replacements = 1; replacements <= 1; replacements++)
		{
			Worm.ResetAgentsBody(rs);
			Worm.ResetChemCon(rs);
			Worm.UpdateChemCon(rs);
			Worm.InitialiseSensorHistory();
			Worm.SetOffsetCPG(Pi/2);
			Worm.SetOrientation(-Pi/2);
			t=0,
			Worm.PrintDetail(ExampleFile, t);
			//Worm.PrintPath(ExampleFile);
			for (t = StepSize; t <= RunDuration; t += StepSize)
			{
				Worm.Step(StepSize, rs,t);
				Worm.UpdateChemCon(rs);
				Worm.UpdateChemConHistory();
				Worm.UpdateSensors();
				if (rs.UniformRandom(0.0,1.0) < PirouetteProb){
					PirFile << Worm.PositionX() << " " << Worm.PositionY() << endl;
					Worm.SetOrientation(rs.UniformRandom(0,2*Pi));
				}

#ifdef ONKILL
				Worm.SetOnCell(0.0);
#endif
#ifdef OFFKILL
				Worm.SetOffCell(0.0);
#endif
				Worm.PrintDetail(ExampleFile, t);
				//Worm.PrintPath(ExampleFile);
			}
		}
	}

	ExampleFile.close();
	PirFile.close();
}


int main (int argc, const char* argv[])
{
	ExampleRun();
	TVector<double> phenotype;
	phenotype.SetBounds(1,3);
    phenotype[1] = 1;
    phenotype[2] = 2;
    phenotype[3] = 4;
	//phenotype.PushFront(5);
    //cout << phenotype(3) << " " << endl;
	return 0;
}

