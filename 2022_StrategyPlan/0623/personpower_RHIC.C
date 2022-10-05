#include "style.C+"
#include "draw.C+"

void personpower_RHIC()
{
  style();

  const Double_t sc = 7./4.;
  const Int_t N = 40;
  const Int_t NN = 5;

  const Int_t NP = 4;
  const Char_t *Name[NP] = {"EIC1","CBM","sPHENIX","STAR"};

  Double_t frac[NP][NN][2] = {{{0., 0.3}, {2., 0.3}, {18., 0.75}, {36, 2.0}, {40., 2.0}},
			      
			      //      {0., 0.2}, {2., 0.2}, {18., 0.75}, {26., 1.5}, {40., 1.5}},
			      {{0., 0.5}, {18., 1.5}, {30., 3.5}, {36, 4.0}, {40., 4.0}},
			      {{0., 1.500}, {2., 1.500}, {10., 3.5}, {16., 4.0}, {40, 4.0}},
			      {{0., 4.0}, {10., 4.0}, {20., 4.0}, {30., 4.0}, {40, 4.0}}
  };

  TCanvas *c1 = new TCanvas("c1","",1200, 600);
//  c1->SetTopMargin(0.001);
  c1->SetLeftMargin(0.09);
  c1->SetBottomMargin(0.14);
  c1->SetGridx();
  c1->SetGridy();
  c1->Draw();

  double x1 = 0;
  double x2 = N;
  double y1 = 0;
  double y2 = 4.*sc;
  TH1D *h0 = new TH1D("h0","",1,x1,x2);
  h0->GetXaxis()->SetNdivisions(101);
  h0->GetXaxis()->CenterTitle();
  h0->GetXaxis()->SetTitleOffset(999.);
  h0->GetXaxis()->SetLabelOffset(999.);
  h0->SetYTitle("Total FTEs");
  h0->GetYaxis()->SetNdivisions(505);
  h0->GetYaxis()->SetTitleOffset(0.6);
  h0->GetYaxis()->SetTitleSize(0.06);
  h0->SetMaximum(y2);
  h0->SetMinimum(y1);
  h0->Draw();

  const Double_t tickL = 0.3;
  for(int i=1;i<N/4;i++) {
    drawLine(i*4, y1, i*4, y1+tickL, 2, 1, 1);
    drawLine(i*4, y2, i*4, y2-tickL, 2, 1, 1);
    drawLine(i*4, y1+tickL, i*4, y2-tickL, 1, 2, 8);
  }
  
  TGraph *gr[NP];
  const Int_t kColor[NP] = {kBlue-2, kGreen+1, kOrange, kRed};
  for(int i=0;i<NP;i++) {
    for(int j=0;j<NN;j++) frac[i][j][1] *= sc;

    double x_tmp[NN];
    double y_tmp[NN];
    for(int j=0;j<NN;j++) {
      x_tmp[j] = frac[i][j][0];
      y_tmp[j] = frac[i][j][1];
      if(j<NN-1) drawLine(frac[i][j][0], frac[i][j][1], frac[i][j+1][0], frac[i][j+1][1], 3, 1, kColor[i]);
    }
    gr[i] = new TGraph(NN, x_tmp, y_tmp);
  }

  TGraphErrors *gr_A[NP];
  for(int i=0;i<NP;i++) {
    double xx_tmp[N+1], yy_tmp[N+1], yy_e_tmp[N+1];
    for(int j=0;j<N+1;j++) {
      xx_tmp[j] = j;
      if(i==0) {
	yy_tmp[j] = gr[i]->Eval(xx_tmp[j])/2.0;
	yy_e_tmp[j] = gr[i]->Eval(xx_tmp[j])/2.0;
      } else {
	yy_tmp[j] = (gr[i-1]->Eval(xx_tmp[j]) + gr[i]->Eval(xx_tmp[j]))/2.0;
	yy_e_tmp[j] = fabs(gr[i-1]->Eval(xx_tmp[j]) - gr[i]->Eval(xx_tmp[j]))/2.0;	
      }
    }
    gr_A[i] = new TGraphErrors(N+1, xx_tmp, yy_tmp, 0, yy_e_tmp);
    gr_A[i]->SetFillColorAlpha(kColor[i],0.4);
    gr_A[i]->Draw("e3");
  }

  c1->RedrawAxis();

  drawHistBox(x1,x2,y1,y2);

  const Double_t offset = 0.5;
  const Int_t NL = 5;
  Int_t XP[NL];
  Char_t *XTitle[NL];
  for(int i=0;i<NL;i++) {
    XP[i] = i*8;
    int year = (XP[i]+3)/4 + 23;
    //    int qua = (XP[i]+3)%4 + 1;
    
    drawText(XP[i]+offset, -0.4*sc, Form("FY%d",year), 42, 0.06);
  }

  drawText(2, 3.0*sc, "STAR", 52, 0.06);
  drawText(12, 2.4*sc, "sPHENIX", 52, 0.06);
  drawText(32, 2.8*sc, "CBM", 52, 0.06);
  drawText(33, 0.7*sc, "EIC", 52, 0.06);

  c1->Update();
  c1->SaveAs("personpower_RHIC.pdf");
  c1->SaveAs("personpower_RHIC.png");

  std::ofstream outData;
  for(int i=0;i<NP;i++) {
    outData.open(Form("data/%s.txt",Name[i]));
    for(int j=0;j<N+1;j++) {
      outData << setw(6) << gr_A[i]->GetX()[j] << setw(12) << gr_A[i]->GetEY()[j]*2 << std::endl;
    }
    outData.close();
  }
  
}
