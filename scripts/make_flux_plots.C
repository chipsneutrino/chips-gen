void makeFluxPlots()
{
	TFile *fInput = new TFile("config/input-flux.root","update");

	TH1D* hSpectrum = (TH1D*)((TH1D*)fInput->Get("spectrum"))->Clone();
    hSpectrum->SetLineColor(kRed);
    hSpectrum->SetLineWidth(3);
    hSpectrum->GetXaxis()->SetRangeUser(0, 15);
    hSpectrum->GetXaxis()->SetTitle("Neutrino Energy [GeV]");

    TH1D* hSpectrum_nue = (TH1D*)((TH1D*)fInput->Get("spectrum_nue"))->Clone();
    hSpectrum_nue->SetLineColor(kBlue);
    hSpectrum_nue->SetLineWidth(3);

    TCanvas* c = new TCanvas("c", "c", 800, 800);
    hSpectrum->Draw("");
    hSpectrum_nue->Draw("same");
    c->SetLogy();
    c->SetGridx();
    c->Update();

    c->SaveAs("input-flux.png");
    c->SaveAs("input-flux.C");
}