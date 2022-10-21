#include "style.C+"
#include "draw.C+"

void personpower_EIC(int config = 0)
{
  style();

  const Int_t NC = 3;
  const Int_t N = 40;
  const Int_t NN = 5;

  const Int_t NP = 4;
  const Char_t *Name[NP] = {"Instr-Comp","CBM-ALICE3","EIC-Other","EIC-Si"};
  const Char_t *NameC[NC] = {"default","CEplus","CE"};

  const Double_t frac_all[NC][NP][NN][2] = {
    // default
    {{{0., 2.0}, {8., 0.8}, {20., 0.8}, {40, 0.8}, {40, 0.8}},
     {{0., 2.2}, {8., 1.2}, {20., 2.4}, {40, 3.2}, {40, 3.2}},
     {{0., 5.0}, {8., 5.4}, {20., 5.8}, {40, 7.8}, {40, 7.8}},
     {{0., 10.}, {8., 15.8}, {20., 19.8}, {40, 20.8}, {40, 20.8}}},					
    // CEplus
    {{{0., 2.0}, {8., 0.8}, {20., 0.8}, {40, 0.8}, {40, 0.8}},
     {{0., 2.2}, {8., 1.2}, {20., 2.4}, {40, 3.2}, {40, 3.2}},
     {{0., 5.0}, {8., 5.4}, {20., 3.2}, {40, 7.8}, {40, 7.8}},
     {{0., 11.3}, {8., 14.5}, {20., 18.5}, {40, 19.5}, {40, 19.5}}},					
    // CE
    {{{0., 1.5}, {8., 1.3}, {20., 1.3}, {40, 1.3}, {40, 1.3}},
     {{0., 1.6}, {8., 1.8}, {20., 2.8}, {40, 3.8}, {40, 3.8}},
     {{0., 3.8}, {8., 5.6}, {20., 7.6}, {40, 9.0}, {40, 9.0}},
     {{0., 10.0}, {8., 11.8}, {20., 15.4}, {40, 17.2}, {40, 17.2}}}      
  };

  Double_t frac[NP][NN][2];
  for(int i=0;i<NP*NN*2;i++) {
    frac[i/(NN*2)][(i%(NN*2))/2][i%2] = frac_all[config][i/(NN*2)][(i%(NN*2))/2][i%2];
  }
  
  
  TCanvas *c1 = new TCanvas("c1","",1200, 600);
  //  c1->SetTopMargin(0.001);
  c1->SetLeftMargin(0.09);
  c1->SetBottomMargin(0.14);
  c1->SetGridx();
  //  c1->SetGridy();
  c1->Draw();

  double x1 = 0;
  double x2 = N;
  double y1 = 0;
  double y2 = 21.0;
  if(config==1) y2=6.7;
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
  const Int_t kColor[NP+1] = {kMagenta, kCyan, kBlue-4, kBlue-2};
  for(int i=0;i<NP;i++) {
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
  for(int i=NP-1;i>=0;i--) {
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
    
    drawText(XP[i]+offset, -2, Form("FY%d",year), 42, 0.06);
  }

  drawText(33, 0.3, "Instr", 52, 0.06);
  if(config==0) drawText(16, 3.0, "EIC-Other", 52, 0.06);
  drawText(33, 10.0, "EIC-Si", 52, 0.06);

  c1->Update();
  c1->SaveAs(Form("personpower_EIC_%s.pdf",NameC[config]));
  c1->SaveAs(Form("personpower_EIC_%s.png",NameC[config]));

  Double_t yAve[NP][3];
  const Int_t aveEdge[3] = {8, 20, 40};
  std::ofstream outData;
  for(int i=0;i<NP;i++) {
    outData.open(Form("data/%s_%s.txt",Name[i],NameC[config]));

    for(int jj=0;jj<3;jj++) yAve[i][jj] = 0;
    for(int j=0;j<N+1;j++) {
      outData << setw(6) << gr_A[i]->GetX()[j] << setw(12) << gr_A[i]->GetEY()[j]*2 << std::endl;
      double y_tmp = gr_A[i]->GetEY()[j]*2;

      if(j>0 && j<aveEdge[0]) yAve[i][0] += y_tmp;
      if(j>aveEdge[0] && j<aveEdge[1]) yAve[i][1] += y_tmp;
      if(j>aveEdge[1] && j<aveEdge[2]) yAve[i][2] += y_tmp;
      if(j==0) yAve[i][0] += 0.5 * y_tmp;
      if(j==aveEdge[0]) {
	yAve[i][0] += 0.5 * y_tmp;
	yAve[i][1] += 0.5 * y_tmp;
      }
      if(j==aveEdge[1]) {
	yAve[i][1] += 0.5 * y_tmp;
	yAve[i][2] += 0.5 * y_tmp;
      }
      if(j==aveEdge[2]) yAve[i][2] += 0.5 * y_tmp;      
    }
    

    yAve[i][0] /= 8.;
    yAve[i][1] /= 12.;
    yAve[i][2] /= 20.;
    
    outData.close();

    cout << Name[i] << ":\t " << yAve[i][0] << "\t" << yAve[i][1] << "\t" << yAve[i][2] << endl;
    
  }
}
