/* 
Copyright (c) 2007-2012, The Regents of the University of California. 
Produced at the Lawrence Livermore National Laboratory 
UCRL-CODE-227323. 
All rights reserved. 
 
For details, see http://nuclear.llnl.gov/simulations
Please also read this http://nuclear.llnl.gov/simulations/additional_bsd.html
 
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:
 
1.  Redistributions of source code must retain the above copyright
notice, this list of conditions and the disclaimer below.

2. Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the disclaimer (as noted below) in
the documentation and/or other materials provided with the
distribution.

3. Neither the name of the UC/LLNL nor the names of its contributors
may be used to endorse or promote products derived from this software
without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OF
THE UNIVERSITY OF CALIFORNIA, THE U.S. DEPARTMENT OF ENERGY OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*
Modified by Josh Tingey (j.tingey.16@ucl.ac.uk) for the CHIPS experiment
*/

#include "CRYGenerator.h"
#include "CRYSetup.h"

#include <iostream>
#include <algorithm>
#include <cmath>
#include <string>
#include <vector>
#include <fstream>
#include <stdlib.h>
#include <cstdlib>

// Default values to the "chips5" geometry
double detHalfHeight = 6.00;
double detRadius = 12.66;
double overburden = 50.0;
bool forTraining = false;
double xDirThreshold = 0.90;

bool KeepEvent(CRYParticle* p, double &newX, double &newY, double &newE);

int main(int argc, const char *argv[]) 
{
	std::string configFile = "default.conf"; 	// default path to configuration file
	int nEv = 1000; 						 	// default number of cosmic-ray events to produce
	std::string geoType = "chips5";				// default geometry to use

	if (argc < 2) 
	{
		std::cout << "usage " << argv[0] << " <config file name> <N events> <Geometry Type> <For Training>\n";
		std::cout << "Config file = " << configFile << " by default\n";
		std::cout << "N events = " << nEv << " by default\n";
		std::cout << "Geometry = " << geoType << " by default. Options <chipsM>, <chips10>, <chips5>, <realistic>\n";
		std::cout << "For Training = " << forTraining << " by default. If true, imposes x>0.97\n";
		return 0;
	}

	configFile = argv[1];
	if (argc > 2) nEv=atoi(argv[2]);
	if (argc > 3) geoType = argv[3];
	if (argc > 4) forTraining = argv[4];

	if(geoType == "chipsM")
	{
		detHalfHeight = 1.66;
		detRadius = 1.72;
		overburden = 50.0;
	}
	else if(geoType == "chips10")
	{
		detHalfHeight = 10.00;
		detRadius = 12.66;
		overburden = 50.0;
	}
	else if(geoType == "chips5")
	{
		detHalfHeight = 6.00;
		detRadius = 12.66;
		overburden = 50.0;
	}
	else if(geoType == "realistic")
	{
		detHalfHeight = 4.00;
		detRadius = 12.66;
		overburden = 6.0;
	}
	else
	{
		std::cout << "Unknown detector type!";
		return -1;
	}

	// Read the setup file into setupString
	std::ifstream inputFile;
	inputFile.open(configFile.c_str(), std::ios::in);
	char buffer[1000];

	std::string setupString("");
	while (!inputFile.getline(buffer,1000).eof()) 
	{
		setupString.append(buffer);
		setupString.append(" ");
	}

	// Parse the contents of the setup file
	CRYSetup *setup=new CRYSetup(setupString, std::getenv("CRYDATA"));

	// Setup the CRY event generator
	CRYGenerator gen(setup);

	// Generate the events
	std::vector<CRYParticle*> *ev=new std::vector<CRYParticle*>;
	int i = 0;
	while (i<nEv)
	{
		// Generate the event
		ev->clear();
		gen.genEvent(ev);
		CRYParticle* p = (*ev)[0];

		// Check if we want the event. If we do, work out
		// the values of x, y and E just above the detector
		double newX = 0.0;
		double newY = 0.0;
		double newE = 0.0;
		if(KeepEvent(p, newX, newY, newE)) // That's a good event. Yay!
		{
			// Write a nuance style vec file
			std::cout << "$ begin" << std::endl;
			std::cout << "$ nuance " << 100 << std::endl;
	
			// Only use the first muon for now, and set the event vertex
			// Convert to cm, and set z to be about 1m above the detector
			std::cout << "$ vertex " << newX*100 << " " 
									 << newY*100 << " " 
									 << (1.0+detHalfHeight)*100 << " " // 1m above the detector
									 << "0" << std::endl;
			// Need two dummy lines for the neutrino and target
			std::cout << "$ track 13 " << newE << " 0 0 -1 -1" << std::endl;
			std::cout << "$ track 8016 14890 -999 -999 -999 -1" << std::endl;
			std::cout << "$ track 13" << " " << newE // The energy of the muon 1m above the detector
									  << " " << p->u()
									  << " " << p->v()
									  << " " << p->w()
									  << " 0" << std::endl;
			std::cout << "$ end" << std::endl;
			++i; 
		}

		// Clean up to prevent memory leaks
		for(unsigned int v = 0; v < ev->size(); ++v)
		{
			delete (*ev)[v];
			(*ev)[v] = 0x0;
		}
	}
	std::cout << "$ stop" << std::endl;
	std::cout << "Total time simulated: " << gen.timeSimulated() << " seconds\n";
	return 0;
}

bool KeepEvent(CRYParticle* p, double &newX, double &newY, double &newE)
{
	// If we only want events for network training we insist that the x-direction value is above some threshold
	if(forTraining && (p->u() < xDirThreshold)) return false;


	// The muons are generated at "sea level" we treat this as being the level of the water in the pit!
	// Apply muon energy threshold for traversing water overburden ~ E_thres = (2.2 Mev/cm * 100) * 50m = 10 GeV.
	if(p->ke() < (220*overburden)) return false;

	// Look at slices in z over the height of the detector and check to see if the x,y position
	// is within the disc defined by the detector radius
	bool withinADisc = false;
	int nSlice = 100;
	for(int i = 0; i < nSlice; ++i)
	{
		double depth = -1*(overburden) -i*(2.0*detHalfHeight/((double)nSlice));
		double nSteps = depth/p->w();
		double curX = p->x() + p->u()*nSteps;  
		double curY = p->y() + p->v()*nSteps;  
		double curR = sqrt(curX*curX + curY*curY);

		// If the muon is within the detector slice, break out and keep the event.
		if(curR < detRadius){
			withinADisc = true;
			break;
		}
	}
	if(!withinADisc) return false;

	// At this point we have decided that we want the event.
	// Now we need to work out the x, y and E 1m above the detector. This means we don't need to use
	// GEANT to simulate the overburden of water, which it turns out is rather slow.
	double nSteps = -1*(overburden-1)/p->w();
	newX = p->x() + p->u()*nSteps;
	newY = p->y() + p->v()*nSteps;
	
	// For energy we work out the path length between where we started and ended. 
	// arXiv hep-ph/0106010v1 suggests 2.2MeV/cm should be ok up to 100GeV, so let's use that for now.
	double pathLength = (p->x()-newX)*(p->x()-newX) + (p->y()-newY)*(p->y()-newY) + (overburden-1)*(overburden-1);
	pathLength = sqrt(pathLength);
	pathLength = pathLength * 100; // Convert to cm.
	newE = p->ke() - pathLength*2.2;

	// Check to see if the energy is sensible, as a high angle muon could reach zero even given the cut
	// at the start of this function. We are at minimum 1m above the detector now, so make sure we have
	// at least 220 MeV
	if(newE < 220) return false;

	return true;
}

