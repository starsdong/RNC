#include "style.C+"
#include "draw.C+"

void makePlot(int config=0)
{
  style();
  const Int_t N = 40;
  const Int_t ISkip = 8;  // adjustment for 3-7 yrs
  const Int_t ISkip2 = 28;  // adjustment for 3-7 yrs

  double sc = 1.;
  if(config==1) sc = 100./20.8;
  
  
  const Int_t NP = 6;
  const Char_t *Name[NP] = {"STAR", "sPHENIX", "JLab", "Instr-Comp", "ALICE", "EIC-Other"};
  const Char_t *LabelName[NP] = {"STAR", "sPHENIX", "JLab", "Instr-Comp", "ALICE", "EIC"};

  Double_t xx[N+1], yy[NP][N+1], yyI[NP][N+1];
  
  std::ifstream inData;
  for(int i=0;i<NP;i++) {
    inData.open(Form("output_check/%s.txt",Name[i]));
    if(i<NP-1) {
      for(int j=0;j<N+1;j++) {
	inData >> xx[j] >> yy[i][j];
      }
      inData.close();
    } else {
      for(int j=0;j<N+1;j++) {
	double yt1, yt2;
	inData >> xx[j] >> yt1 >> yt2;
	if(j<ISkip || j>ISkip2) yy[i][j] = yt2;
	else yy[i][j] = yt1;
      }
      inData.close();
    }
  }

  for(int j=0;j<N+1;j++) {
    yyI[0][j] = yy[0][j];
    for(int i=1;i<NP;i++) {
      yyI[i][j] = yy[i][j] + yyI[i-1][j];
    }
  }

  if(config==1) {
    for(int j=0;j<N+1;j++) {
      for(int i=0;i<NP;i++) {
	yyI[i][j] *= sc;
      }
    } 
  }

  
  
  TCanvas *c1 = new TCanvas("c1","",1200, 750);
  c1->SetLeftMargin(0.12);
  c1->SetBottomMargin(0.10);
  // c1->SetGridx();
  // c1->SetGridy();
  c1->Draw();


  double x1 = 0;
  double x2 = N;
  double y1 = 0;
  double y2 = 24 * sc;
  
  TH1D *h0 = new TH1D("h0","",1,x1,x2);
  h0->GetXaxis()->SetNdivisions(101);
  h0->GetXaxis()->CenterTitle();
  h0->GetXaxis()->SetTitleOffset(999.);
  h0->GetXaxis()->SetLabelOffset(999.);
  h0->SetYTitle("Total FTEs");
  if(config==1) h0->SetYTitle("FTE Fraction (%)");
  if(config==0) h0->GetYaxis()->SetNdivisions(505);
  if(config==1) h0->GetYaxis()->SetNdivisions(210);
  h0->GetYaxis()->SetTitleOffset(0.85);
  h0->GetYaxis()->SetTitleSize(0.06);
  h0->GetYaxis()->SetLabelSize(0.045);
  h0->SetMaximum(y2);
  h0->SetMinimum(y1);
  h0->Draw("c");

  const Double_t tickL = 1.5;
  for(int i=1;i<N/4;i++) {
    //    drawLine(i*4, y1, i*4, y1+tickL, 2, 1, 1);
    //    drawLine(i*4, y2, i*4, y2-tickL, 2, 1, 1);
    drawLine(i*4, y1, i*4, y2, 1, 2, 8);
  }
  
  TGraph *gr[NP];
  const Int_t kColor[NP] = {kOrange, kYellow, kGreen, kPink-4, kCyan, kBlue-3};
  const Int_t kColorLine[NP] = {kOrange, kGreen+1, kYellow, kPink-4, kBlue-4, kBlue-3};
  
  Double_t yAve[NP][3], y0[NP];
  const Int_t aveEdge[3] = {8, 20, 40};
  cout << setw(10) << "Project \t" << setw(10) << " t=0 \t" << setw(10) << " 0-2yr\t" // << setw(10) << " 0-2yr(2)\t"
       << setw(10) << "3-5yr\t" << setw(10) << "6-10yr" << endl;
  double ySum0 = 0;
  double ySum[3] = {0, 0, 0};

  TGraphErrors *gr_A[NP];
  for(int i=0;i<NP;i++) {
    double xx_tmp[N+1], yy_tmp[N+1], yy_e_tmp[N+1];
    for(int j=0;j<N+1;j++) {
      if(i==0) {
	yy_tmp[j] = yyI[i][j]/2.0;
	yy_e_tmp[j] = yyI[i][j]/2.0;
      } else {
	yy_tmp[j] = (yyI[i-1][j] + yyI[i][j])/2.0;
	yy_e_tmp[j] = fabs(yyI[i-1][j] - yyI[i][j])/2.0;	
      }
    }

    gr_A[i] = new TGraphErrors(N+1, xx, yy_tmp, 0, yy_e_tmp);
    gr_A[i]->SetFillColorAlpha(kColor[i],0.4);
    gr_A[i]->Draw("e3");

    for(int j=0;j<N+1;j++) {
      
      if(j>0 && j<aveEdge[0]) { yAve[i][0] += yy_e_tmp[j]*2; }
      if(j>aveEdge[0] && j<aveEdge[1]) { yAve[i][1] += yy_e_tmp[j]*2; }
      if(j>aveEdge[1] && j<aveEdge[2]) { yAve[i][2] += yy_e_tmp[j]*2; }
      if(j==0) { yAve[i][0] += yy_e_tmp[j]; }
      if(j==aveEdge[0]) {
	yAve[i][0] += yy_e_tmp[j];
	yAve[i][1] += yy_e_tmp[j];
      }
      if(j==aveEdge[1]) {
	yAve[i][1] += yy_e_tmp[j];
	yAve[i][2] += yy_e_tmp[j];
      }
      if(j==aveEdge[2]) { yAve[i][2] += yy_e_tmp[j]; }
    }
    y0[i] = yy_e_tmp[0]*2;

    yAve[i][0] /= 8.;
    yAve[i][1] /= 12.;
    yAve[i][2] /= 20.;
        
    cout << setw(10) << LabelName[i] << ":\t " << setw(10) << y0[i] << "\t" << setw(10) << yAve[i][0] //<< "\t" << setw(10) << yAve2[i][0]
	 << "\t" << setw(10) << yAve[i][1] << "\t" << setw(10) << yAve[i][2] << endl;
    
    ySum0 += y0[i];
    ySum[0] += yAve[i][0];
    ySum[1] += yAve[i][1];
    ySum[2] += yAve[i][2];
    
  }


  cout << setw(10) << "Total" << ":\t " << setw(10) << ySum0 << "\t" << setw(10) << ySum[0] //<< "\t" << setw(10) << ySum2[0]
       << "\t" << setw(10) << ySum[1] << "\t" << setw(10) << ySum[2] << endl;
  

  for(int i=0;i<NP;i++) {
    for(int j=0;j<N;j++) {
      drawLine(xx[j], yyI[i][j], xx[j+1], yyI[i][j+1], 3, 1, kColorLine[i]);
    }
  }
  
  c1->RedrawAxis();

  drawHistBox(x1,x2,y1,y2);

  const Double_t offset = 0.2;
  const Int_t NL = 5;
  Int_t XP[NL];
  Char_t *XTitle[NL];
  for(int i=0;i<NL;i++) {
    XP[i] = i*8;
    int year = (XP[i]+3)/4 + 23;
    //    int qua = (XP[i]+3)%4 + 1;
    
    drawText(XP[i]+offset, y1-1.8*sc, Form("FY%d",year), 42, 0.06);
  }

  const Double_t xPos[NP] = {1.5, 12, 20, 1., 26, 31};
  const Double_t yPos[NP] = {1.0, 1.0, 2.4, 8.2, 6., 14.6};
  
  for(int i=0;i<NP;i++) {
    double yt = yPos[i] * sc;
    drawText(xPos[i], yt, LabelName[i], 52, 0.045);
  }
  
  //  drawLine(x1, CE, x2, CE, 3, 9, 1);
  //  drawLine(x1, 0, x2, 0, 3, 9, 1);

  c1->Update();

  c1->SaveAs(Form("FTE_CE_Check_%d.pdf",config));
  c1->SaveAs(Form("FTE_CE_Check_%d.png",config));
  
}
