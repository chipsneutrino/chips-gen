#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>

#include "TCanvas.h"
#include "TH1D.h"
#include "TH2D.h"

void cosmicPlots()
{
	std::string fileName = "input.vec";

	TH1D* hMuonEnergy = new TH1D("hMuonEnergy","",100,0,100);
	hMuonEnergy->GetXaxis()->SetTitle("True Muon Energy / GeV");
	hMuonEnergy->GetXaxis()->CenterTitle(1);

	TH1D* hMuonAngleX = new TH1D("hMuonAngleX","",50,-1,1);
	hMuonAngleX->GetXaxis()->SetTitle("True Muon Angle Cosine");
	hMuonAngleX->GetXaxis()->CenterTitle(1);

	TH1D* hMuonAngleY = new TH1D("hMuonAngleY","",50,-1,1);
	hMuonAngleY->GetXaxis()->SetTitle("True Muon Angle Cosine");
	hMuonAngleY->GetXaxis()->CenterTitle(1);

	TH1D* hMuonAngleZ = new TH1D("hMuonAngleZ","",50,-1,0);
	hMuonAngleZ->GetXaxis()->SetTitle("True Muon Angle Cosine");
	hMuonAngleZ->GetXaxis()->CenterTitle(1);

	TH2D* hMuonVertex = new TH2D("hMuonVertex","",100,-10000,10000,100,-10000,10000);
	hMuonVertex->GetXaxis()->SetTitle("Muon Position X / cm");
	hMuonVertex->GetXaxis()->CenterTitle(1);
	hMuonVertex->GetYaxis()->SetTitle("Muon Position Y / cm");
	hMuonVertex->GetYaxis()->CenterTitle(1);

	// Open the .vec file to read the lines
	std::ifstream infile(fileName.c_str(), std::ifstream::in);
	std::string tString = "";
	while(getline(infile,tString))
	{
		if(tString.find("track") != std::string::npos) // Is this a track line?
		{
			double energy, dx, dy, dz;
			int pdg, real;
			std::string dollar, track;

			std::stringstream tStream; // Use a stream to read into the variables
			tStream << tString;
			tStream >> dollar >> track >> pdg >> energy >> dx >> dy >> dz >> real;

			if(pdg == 13 && real == 0) // Just look at the muons
			{
				hMuonEnergy->Fill(energy/1000.);
				hMuonAngleX->Fill(dx);
				hMuonAngleY->Fill(dy);
				hMuonAngleZ->Fill(dz);
			} 
		}
		else if(tString.find("vertex") != std::string::npos) // Is this a vertex line?
		{
			double x,y,z,t;
			std::string dollar, vertex;

			std::stringstream tStream; // Use a stream to read into the variables
			tStream << tString;
			tStream >> dollar >> vertex >> x >> y >> z >> t;

			hMuonVertex->Fill(x,y);
		}
	}

	TFile * mainOutput = new TFile("output.root", "RECREATE");
	hMuonEnergy->Write();
	hMuonAngleX->Write();
	hMuonAngleY->Write();
	hMuonAngleZ->Write();
	hMuonVertex->Write();
	mainOutput->Close();
}
