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
<<<<<<< HEAD

=======
void GenPhenMapping(TVector<double> &gen, TVector<double> &phen)
{
	phen(1) = MapSearchParameter( gen(1), -SensoryWeightRange, SensoryWeightRange); // w_on
	phen(2) = MapSearchParameter( gen(2), -SensoryWeightRange, SensoryWeightRange);	// w_off
	phen(3) = MapSearchParameter( gen(3), 0.0, WeightRange);						// w_osc o cpg 
	phen(4) = MapSearchParameter( gen(4), -WeightRange, WeightRange);				// self loop
	phen(5) = MapSearchParameter( gen(5), -BiasRange, BiasRange);					// bias or threshold
	phen(6) = MapSearchParameter( gen(6), MinSensorN, MaxSensorN);					// Sensory cell integration time, current (N)
	phen(7) = MapSearchParameter( gen(7),  MinSensorM, MaxSensorM);					// Sensory cell integration time, past (M)
	phen(8) = MapSearchParameter( gen(8),  MinSensorD, MaxSensorD);					// Sensory cell time constant delay (D)
	phen(9) = MapSearchParameter( gen(9), MinNeckTurnGain, MaxNeckTurnGain);		// Neck turning gain o Output gain
}
>>>>>>> d5660999439d6c934fa2fe660eba208c50fb3bef

// ------------------------------------
// Fitness function
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
	ExampleFile.open("/Users/carlos/Downloads/Klino/examples.dat");
	PirFile.open("/Users/carlos/Downloads/Klino/pirxy.dat");
	WormAgent Worm("/Users/carlos/Downloads/Klino/best.ns.dat");
	Worm.InitialiseAgent(StepSize);

	for (repetitions = 1; repetitions <= 1; repetitions++)
	{
		Worm.ResetAgentIntState(rs);
		for (replacements = 1; replacements <= 1; replacements++)
		{
			Worm.ResetAgentsBody(rs);
			Worm.ResetChemCon(rs);
			Worm.UpdateChemCon(rs);
			Worm.InitialiseSensorHistory();
			Worm.SetOffsetCPG(Pi/2);
			Worm.SetOrientation(-Pi/2);
			Worm.PrintDetail(ExampleFile);
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
				Worm.PrintDetail(ExampleFile);
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
	//BehavioralOverview();
	//AccConcentration();
	//BehavioralAnalysis();
	//OrientationAnalysis();
	//BearingCurvature();

	//AutonomousDynamics();
	//PhaseSensitivityExample();
	//PhaseSensitivity();
	//ExploreParameterSpace();
	//ConcStepSensoryDyn();
	//Riverchip();
	return 0;
}

