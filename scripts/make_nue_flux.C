double GetAppearanceProb(double E)
{
	double dm2 = 2.39e-3;
	double s22th13 = 0.0945;
	double s2th23 = 0.43;
	double L = 708;
	return s22th13 * s2th23 * pow(TMath::Sin(1.27*dm2*L/E),2);
}

void Oscillate(TH1D* h)
{
	for(int b = 1; b <= h->GetNbinsX(); ++b){
		double val = h->GetBinContent(b);
		double prob = GetAppearanceProb(h->GetXaxis()->GetBinCenter(b));
		std::cout << " E = " << h->GetXaxis()->GetBinCenter(b) << ", P = " << prob << std::endl;
		h->SetBinContent(b,val*prob);
	}
}

void makeNueFlux()
{
	TFile *fInput = new TFile("config/old-input-flux.root","update");

	// Clone the numu spectrum and generate a normalised version
	TH1D* hSpectrum_numu_norm = (TH1D*)((TH1D*)fInput->Get("spectrum-numu"))->Clone();
	hSpectrum_numu_norm->SetDirectory(0);
	Double_t scale = 1/(hSpectrum_numu_norm->Integral());
	hSpectrum_numu_norm->Scale(scale);
	hSpectrum_numu_norm->SetTitle("");
	hSpectrum_numu_norm->SetName("spectrum-numu-norm");
	hSpectrum_numu_norm->GetXaxis()->SetTitle("Neutrino Energy [GeV]");
	hSpectrum_numu_norm->Write("spectrum-numu-norm");
	
	// Clone the numu spectrum and oscillation it into the nuel spectrum
	TH1D* hSpectrum_nuel = (TH1D*)((TH1D*)fInput->Get("spectrum-numu"))->Clone();
	hSpectrum_nuel->SetDirectory(0);
	Oscillate(hSpectrum_nuel);
	hSpectrum_nuel->SetTitle("");
	hSpectrum_nuel->SetName("spectrum-nuel");
	hSpectrum_nuel->GetXaxis()->SetTitle("Neutrino Energy [GeV]");
	hSpectrum_nuel->Write("spectrum-nuel");

	// Copy the oscillated spectrum and generate a normalised version
	TH1D* hSpectrum_nuel_norm = (TH1D*)hSpectrum_nuel->Clone();
	hSpectrum_nuel_norm->SetDirectory(0);
	scale = 1/(hSpectrum_nuel_norm->Integral());
	hSpectrum_nuel_norm->Scale(scale);
	hSpectrum_nuel_norm->SetTitle("");
	hSpectrum_nuel_norm->SetName("spectrum-nuel-norm");
	hSpectrum_nuel_norm->GetXaxis()->SetTitle("Neutrino Energy [GeV]");
	hSpectrum_nuel_norm->Write("spectrum-nuel-norm");

	// Close the file
	fInput->Close();
}
