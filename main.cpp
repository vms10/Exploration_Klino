// ***************************************
// Worm Chemotaxis
// ***************************************

#include "CTRNN.h"
#include "WormAgent.h"
#include "TSearch.h"
#include <sstream>
#include <iomanip>

// Global constants
const int		CircuitSize				= 2;
const double	RunDuration				= 500.0;
//const double	PirouetteProb			= 0.000333333; //(2/60)*0.01; // They reverse about twice a minute (according to Shawn and Serge). In 500 secs, that's 16.66 on avg.
const double	PirouetteProb			= 0.0; // Smaller. So that it doesn't evolve to depend on it. On avg, 2 per trial (500secs) [(2/500) * 0.01]

// Global variables
int	VectSize = 9;

void ExampleRun()
{
	ofstream ExampleFile;
	ofstream histchem;
	double t;
	long IDUM=time(0);
	int repetitions,replacements;
	RandomState rs;
	rs.SetRandomSeed(IDUM);
	//ExampleFile.open("/Users/carlos/Desktop/Exploration_Klino/for_angulo_vel_cte_sin_restric_CONTROL.txt");
	ExampleFile.open("/Users/carlos/Desktop/Exploration_Klino/chemHistory.txt");
	histchem.open("/Users/carlos/Desktop/Exploration_Klino/chemHistory_vect.txt");
	WormAgent Worm("/Users/carlos/Desktop/Exploration_Klino/best.ns.dat");
	Worm.InitialiseAgent(StepSize);
	double ang_inicial_for= 0.0;
	for (repetitions = 1; repetitions <= 10; repetitions++)
	{
		Worm.ResetAgentIntState(rs);
		for (replacements = 1; replacements <= 1; replacements++)
		{
			Worm.ResetAgentsBody(rs, ang_inicial_for);
			Worm.ResetChemCon(rs);
			Worm.UpdateChemCon(rs);
			Worm.InitialiseSensorHistory();
			//cout << Worm.iSensorM << " " << endl;
			Worm.SetOffsetCPG(Pi/2);
			Worm.SetOrientation(-Pi/2);
			t=0;
			Worm.PrintDetail(ExampleFile, t);
			//cout << Worm.timer << " " << endl;
			for (t = StepSize; t <= RunDuration; t += StepSize)
			{
				Worm.Step(StepSize, rs,t);
				Worm.UpdateChemCon(rs);
				Worm.UpdateChemConHistory();
				Worm.UpdateSensors();
				Worm.PrintDetail(ExampleFile, t);
			}
			
		}
		for (int i = 0; i < 50196; i++){
			histchem << std::fixed << std::setprecision(12) << Worm.chemConHistory[i] << " ";
			histchem << endl;
	}
		ang_inicial_for = ang_inicial_for + 0.6283185307179586;
	}

	ExampleFile.close();
	histchem.close();
	
	
		
}


int main (int argc, const char* argv[])
{
	ExampleRun();
	//TVector<double> phenotype;
	//phenotype.SetBounds(1,3);
    //phenotype[1] = 1;
    //phenotype[2] = 2;
    //phenotype[3] = 4;
	//phenotype.PushFront(5);
    //cout << phenotype(3) << " " << endl;
	return 0;
}

