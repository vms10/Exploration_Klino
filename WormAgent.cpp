/*
 *  WormAgent.cpp
 *
 *  Created by EJI on 17/08/2008.
 *  Copyright 2008 Centre for Computational Neuroscience and Robotics. All rights reserved.
 *
 */

#include "WormAgent.h"
#include "TSearch.h"
#include "CTRNN.h"
#include "random.h"




// ****************************
// Constructors and Destructors
// ****************************

// The constructor
WormAgent::WormAgent(int newsize)
{
	InitialiseCircuit(newsize);
	InitialiseSensors();
	InitialiseMotors();
}

WormAgent::WormAgent(char* fnm)
{
	SetWormParametersFromFile(fnm);  //Call to initialise sensors within
}

// The destructor
WormAgent::~WormAgent()
{
	InitialiseCircuit(0);
	InitialiseSensors();
}

// *********
// Setting paremeters
// *********
void WormAgent::SetWormParametersFromFile(char* fnm)
{
	ifstream BestIndividualFile;
	BestIndividualFile.open(fnm);
	BestIndividualFile >> NervousSystem;
	size = NervousSystem.CircuitSize();	
	InitialiseSensors();
	InitialiseMotors();								
	for (int i = 1; i <= size; i++)					
		BestIndividualFile >> onWeight(i);	
	for (int i = 1; i <= size; i++)				
		BestIndividualFile >> offWeight(i);		
	for (int i = 1; i <= size; i++)				
		BestIndividualFile >> cpgWeight(i);		
	BestIndividualFile >> sensorN;
	BestIndividualFile >> sensorM;
	BestIndividualFile >> sensorD;
	BestIndividualFile >> outputGain;	
	BestIndividualFile.close();
}

void WormAgent::SetParameters(TVector<double> &v)
{	
	// Make the connections from the SENSORY cells to the motor neurons bilaterally symmetrical
	onWeight(dorsalMotorNeuron) = onWeight(ventralMotorNeuron) = v(1);
	offWeight(dorsalMotorNeuron) = offWeight(ventralMotorNeuron) = v(2);
	
#ifdef KILLONCELL // Cut the connections from the OFF cell
	onWeight(dorsalMotorNeuron) = onWeight(ventralMotorNeuron) = 0.0;
#endif
	
#ifdef KILLOFFCELL // Cut the connections from the OFF cell
	offWeight(dorsalMotorNeuron) = offWeight(ventralMotorNeuron) = 0.0;
#endif
	
	// Make the connections from the OSCILLIATOR to both motor neurons equal
	cpgWeight(dorsalMotorNeuron) = cpgWeight(ventralMotorNeuron) = v(3);
	
	// Set connections between the two motor neurons to 0.0
	NervousSystem.SetConnectionWeight(dorsalMotorNeuron, ventralMotorNeuron, 0.0);
	NervousSystem.SetConnectionWeight(ventralMotorNeuron, dorsalMotorNeuron, 0.0);
	
	// Set the left motor neuron parameters to be the same as the right's (self-connection, bias, time)
	NervousSystem.SetConnectionWeight(ventralMotorNeuron, ventralMotorNeuron, v(4));
	NervousSystem.SetConnectionWeight(dorsalMotorNeuron, dorsalMotorNeuron, v(4));
	
	NervousSystem.SetNeuronBias(dorsalMotorNeuron, v(5));
	NervousSystem.SetNeuronBias(ventralMotorNeuron, v(5));
	
	NervousSystem.SetNeuronTimeConstant(dorsalMotorNeuron, 0.1);
	NervousSystem.SetNeuronTimeConstant(ventralMotorNeuron, 0.1);
	
	sensorN = v(6);	
	sensorM = v(7);	
	sensorD = 0.0;	//v(8);
	
	outputGain = v(9);
}


// *******
// Initialising
// *******

void WormAgent::InitialiseCircuit(int CircuitSize)
{	
	size = CircuitSize;
	NervousSystem.SetCircuitSize(size);
}

void WormAgent::InitialiseSensors(){
	onWeight.SetBounds(1,size);
	onWeight.FillContents(0.0);
	offWeight.SetBounds(1,size);
	offWeight.FillContents(0.0);
	V_on = 0.0;
	V_off = 0.0;
}

void WormAgent::InitialiseMotors(){
	dorsalMotorNeuron = size;
	ventralMotorNeuron = size - 1;
	cpgWeight.SetBounds(1,size);
	cpgWeight.FillContents(0.0);
}

void WormAgent::InitialiseAgent(double stepsize)
{
	iSensorN = (int) (sensorN/stepsize); // es mi N_int
	dSensorN = (double) iSensorN; // es mi N_int casteado de double
	iSensorM = (int) (sensorM/stepsize); // es mi M_int
	dSensorM = (double) iSensorM; // es mi M_int casteado de double
	iSensorD = (int) (sensorD/stepsize); // esto es nuevo y no se que es
	dSensorD = (double) iSensorD;
	
	//pathlength = 0.0;
	concave = 0;
	convex = 0;
	avgvel = 0.0;
	pushConcave = 0.0;
	pushConvex = 0.0;	
	histConcave.SetBounds(1,VelDelta); // VelDelta		=	(int) (HST/StepSize);
	histConcave.FillContents(0.0);
	histConvex.SetBounds(1,VelDelta);
	histConvex.FillContents(0.0);
}

void WormAgent::InitialiseSensorHistory()
{
	currentConc = 0.0;
	pastConc = 0.0;
	timer = iSensorD + iSensorN + iSensorM;
	for (int i = 0; i <= timer; i++)
		chemConHistory[i] = chemCon;
}

// *******
// Resetting
// *******

void WormAgent::ResetAgentsBody(RandomState &rs, double ang_inici)
{
	double tempangle = ang_inici; //rs.UniformRandom(0,2*Pi); //XXXXXXX
#ifdef GRAD_CONE				
	distanceToCentre = -MaxDist;	
#endif
#ifdef GRAD_INVCONE				
	distanceToCentre = 0.0;
#endif
#ifdef GRAD_GAUS				
	distanceToCentre = -MaxDist;	
#endif		
#ifdef GRAD_ISO
	distanceToCentre = 0.0;
#endif
	px = cos(tempangle)*distanceToCentre;
	py = sin(tempangle)*distanceToCentre;
	vx = 0.0;
	vy = 0.0;
	V_on = 0.0;
	V_off = 0.0;
	theta = 0.0;
	pastTheta = 0.0;
	orient = rs.UniformRandom(0,2*Pi); 
	CPGoffset = rs.UniformRandom(0,2*Pi);
}

void WormAgent::ResetChemCon( RandomState &rs )
{
	//gradSteepness = 1.0; //exp(rs.UniformRandom(log(0.1),log(1.0)));
	//gradSteepness = rs.UniformRandom(0.1, 1.0);
	gradSteepness = 0.451615; 
}

void WormAgent::ResetAgentIntState(RandomState &rs)
{
	//NervousSystem.RandomizeCircuitState(-0.1,0.1,rs);
	NervousSystem.RandomizeCircuitState(0.0,0.0,rs);	
}

// *******
// Updating
// *******

void WormAgent::UpdateChemCon( RandomState &rs )
{
	distanceToCentre = sqrt(pow(px,2) + pow(py,2));
#ifdef GRAD_CONE				
	chemCon = -distanceToCentre*gradSteepness; 
#endif
#ifdef GRAD_INVCONE				
	chemCon = distanceToCentre*gradSteepness; 
#endif
#ifdef GRAD_GAUS				
	chemCon = 1*exp(-(pow(distanceToCentre,2))/ChemDiffConst);
#endif	
#ifdef GRAD_ISO				
	chemCon = - (fabs((distanceToCentre - MaxDist)*gradSteepness)); 
#endif	
	
}

void WormAgent::UpdateChemConHistory( )
{
	chemConHistory[timer] = chemCon;
	timer++;
}

void WormAgent::UpdateSensors()
{
	currentConc += chemConHistory[timer - iSensorD - 1] - chemConHistory[timer - iSensorD - iSensorN - 1];
	//cout << currentConc << " " << endl;
	pastConc += chemConHistory[timer - iSensorD - iSensorN - 1] - chemConHistory[timer - iSensorD - iSensorN - iSensorM - 1];
	tempDiff = (currentConc/dSensorN) - (pastConc/dSensorM);
	V_on = tempDiff > 0.0 ? tempDiff: 0.0;
	V_off = tempDiff < 0.0 ? fabs(tempDiff): 0.0;
}

void WormAgent::PrintPath( ofstream &file)
{
	file << px << " ";
	file << py << " ";
	file << endl;
}

void WormAgent::PrintDetail( ofstream &file, double timestep)
{
	//file << tempDiff << " ";
	//file << chemConHistory[timer - iSensorD - 1] << " ";
	//file << chemConHistory[timer - iSensorD - iSensorN - 1] << " ";
	//file << chemConHistory[timer - iSensorD - iSensorN - iSensorM - 1] << " ";
	//file << currentConc << " ";
	//file << pastConc << " ";
	//file << chemCon << " ";
	//file << V_on << " ";
	//file << V_off << " ";
	file << px << " ";
	file << py << " ";
	for (int i = 1; i <= size; i++){
		file << NervousSystem.states(i) << " ";
	}	
	//file << chemCon << " ";
	file << orient << " ";
	file << endl;
}

// Step the bacteria simulation
void WormAgent::Step(double StepSize, RandomState &rs, double timestep)
{	
	
	// Add antiphase oscillatory input to the motor neurons, as well as input from the sensory ON cell
	NervousSystem.SetNeuronExternalInput(dorsalMotorNeuron, onWeight(dorsalMotorNeuron)*V_on + offWeight(dorsalMotorNeuron)*V_off + cpgWeight(dorsalMotorNeuron)*sin(CPGoffset + HSP*timestep));
	NervousSystem.SetNeuronExternalInput(ventralMotorNeuron, onWeight(ventralMotorNeuron)*V_on + offWeight(ventralMotorNeuron)*V_off + cpgWeight(ventralMotorNeuron)*sin(CPGoffset + Pi + HSP*timestep));
	
	// Update the signaling system
	NervousSystem.EulerStep(StepSize);
	
	// Calculate head alignment	
	
	NMdiff = NervousSystem.NeuronOutput(ventralMotorNeuron) - NervousSystem.NeuronOutput(dorsalMotorNeuron);
	theta = (outputGain * NMdiff);
	orient += StepSize * theta;
		
	DthetaDt = theta - pastTheta;
	pastTheta = theta;
	
	// Check that there is always one part of the curvature that is convex and another that is concave. This way thrust is generated. 
	pushConcave = DthetaDt > 0 ? StepSize : 0.0;
	pushConvex = DthetaDt < 0 ? StepSize : 0.0;
	histConcave.PushFront(pushConcave);
	histConvex.PushFront(pushConvex);
	concave = concave - histConcave(VelDelta) + pushConcave;
	convex = convex - histConvex(VelDelta) + pushConvex;
	avgvel = MaxVel*pow((concave/(HST/2))*(convex/(HST/2)),2); // concave and convex tienen que ser ambos distintos de cero para que la velocidad sea distinta de cero. 
	aux_vel = avgvel == 0 ? 0.0 : MaxVel;
	
	// vel cte
	//vx = cos(orient) * aux_vel; 	
	//vy = sin(orient) * aux_vel; 

	// vel cte sin chequear si es sinusoidal
	vx = cos(orient) * MaxVel; 	
	vy = sin(orient) * MaxVel; 

	// Update the velocity , avge vel
	//vx = cos(orient) * avgvel; 	
	//vy = sin(orient) * avgvel; 
	
	// Move the agent
	px += StepSize * vx;
	py += StepSize * vy;
}

