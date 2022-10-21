#include "style.C+"
#include "draw.C+"
//#define CBM 1

void personpower(int config = 0)
{
  style();

  const Int_t NC = 3;
  const Double_t CE = 24;
  const Int_t N = 40;
  const Int_t ISkip = 8;  // adjustment for 3-7 yrs
  const Int_t ISkip2 = 28;  // adjustment for 3-7 yrs


  const Char_t *NameC[NC] = {"default","CEplus","CE"};
  
#ifdef CBM
  const Int_t NP = 8;
  const Char_t *Name[NP] = {"STAR", "sPHENIX", "JLab", "Instr-Comp", "CBM-ALICE3", "ALICE", "EIC-Other","EIC-Si"};
  const Char_t *LabelName[NP] = {"STAR", "sPHENIX", "JLab", "Instr-Comp", "CBM-ALICE3", "ALICE", "","EIC"};
#else
  const Int_t NP = 7;
  const Char_t *Name[NP] = {"STAR", "sPHENIX", "JLab", "Instr-Comp", "ALICE", "EIC-Other","EIC-Si"};
  const Char_t *LabelName[NP] = {"STAR", "sPHENIX", "JLab", "Instr-Comp", "ALICE", "","EIC"};
#endif
      
  Double_t xx[N+1], yy[NP][N+1], yyI[NP][N+1];
  Double_t yy2[NP][N+1], yyI2[NP][N+1];  // adjust for EIC in the first two years to make total flatter

  std::ifstream inData;
  for(int i=0;i<NP;i++) {
    inData.open(Form("data/%s_%s.txt",Name[i],NameC[config]));
    for(int j=0;j<N+1;j++) {
      inData >> xx[j] >> yy[i][j];
    }
    inData.close();
  }

  for(int i=0;i<NP;i++) {
    if(i<NP-2) {
      for(int j=0;j<N+1;j++) yy2[i][j] = yy[i][j];
    } else {
      inData.open(Form("data2/%s_%s.txt",Name[i],NameC[config]));
      for(int j=0;j<N+1;j++) {
	inData >> xx[j] >> yy2[i][j];
      }
      inData.close();
    }
  }

  for(int j=0;j<N+1;j++) {
    yyI[0][j] = yy[0][j];
    yyI2[0][j] = yy2[0][j];
    for(int i=1;i<NP;i++) {
      yyI[i][j] = yy[i][j] + yyI[i-1][j];
      yyI2[i][j] = yy2[i][j] + yyI2[i-1][j];
      //      cout << yyI[i][j] << endl;
    }
  }

  // Adjust the total to be smoother for 0-2yr, keep 3-7yr increase and step down at 8-10 years, the average is the same
  for(int j=0;j<N+1;j++) {
    if(j<ISkip+1) {
      if(config==0) {
	yyI2[NP-1][j] = 26.5 + (28.5-26.5)*j/ISkip;      
      } else if(config==1) {
	yyI2[NP-1][j] = 25.7;
      } else if(config==2) {
	yyI2[NP-1][j] = 25.5 - (25.5-24.7)*j/ISkip;
      }
    }
    if(j>=ISkip && j<ISkip2+1) {
      if(config==0) {
	yyI[NP-1][j] = 30.0;      
      } else if(config==1) {
	yyI[NP-1][j] = 27.2;
      } else if(config==2) {
	yyI[NP-1][j] = 23.3;
      }
    }
    if(j>=ISkip2) {
      if(config==0) {
	yyI2[NP-1][j] = 28.5;      
      } else if(config==1) {
	yyI2[NP-1][j] = 25.7;
      } else if(config==2) {
	yyI2[NP-1][j] = 23.3;
      }
    }
  }
  

  Double_t yAve[NP][3];
  Double_t yAve2[NP][3];
  const Int_t aveEdge[3] = {8, 20, 40};
  cout << setw(10) << "Project \t" << setw(10) << " t=0 \t" << setw(10) << " 0-2yr\t" // << setw(10) << " 0-2yr(2)\t"
       << setw(10) << "3-5yr\t" << setw(10) << "6-10yr" << endl;
  ofstream outData;
  double ySum0 = 0;
  double ySum[3] = {0, 0, 0};
  double ySum2[3] = {0, 0, 0};
  for(int i=0;i<NP;i++) {
    outData.open(Form("output_check/%s.txt",Name[i]));

    for(int jj=0;jj<3;jj++) { yAve[i][jj] = 0; yAve2[i][jj] = 0; }
    for(int j=0;j<N+1;j++) {
      double y_tmp = 0;
      double y2_tmp = 0;
      y_tmp = yy[i][j];
      y2_tmp = yy2[i][j];
      outData << setw(6) << j << setw(12) <<  y_tmp << "\t" << y2_tmp << endl;

      if(j>0 && j<aveEdge[0]) { yAve[i][0] += y_tmp; yAve2[i][0] += y2_tmp; }
      if(j>aveEdge[0] && j<aveEdge[1]) { yAve[i][1] += y_tmp; yAve2[i][1] += y2_tmp; }
      if(j>aveEdge[1] && j<aveEdge[2]) { yAve[i][2] += y_tmp; yAve2[i][2] += y2_tmp; }
      if(j==0) { yAve[i][0] += 0.5 * y_tmp; yAve2[i][0] += 0.5 * y2_tmp; }
      if(j==aveEdge[0]) {
	yAve[i][0] += 0.5 * y_tmp;
	yAve[i][1] += 0.5 * y_tmp;
	yAve2[i][0] += 0.5 * y2_tmp;
	yAve2[i][1] += 0.5 * y2_tmp;
      }
      if(j==aveEdge[1]) {
	yAve[i][1] += 0.5 * y_tmp;
	yAve[i][2] += 0.5 * y_tmp;
	yAve2[i][1] += 0.5 * y2_tmp;
	yAve2[i][2] += 0.5 * y2_tmp;
      }
      if(j==aveEdge[2]) { yAve[i][2] += 0.5 * y_tmp; yAve2[i][2] += 0.5 * y2_tmp; }
    }
    

    yAve[i][0] /= 8.;
    yAve[i][1] /= 12.;
    yAve[i][2] /= 20.;
    yAve2[i][0] /= 8.;
    yAve2[i][1] /= 12.;
    yAve2[i][2] /= 20.;
    
    outData.close();

    cout << setw(10) << Name[i] << ":\t " << setw(10) << yy[i][0] << "\t" << setw(10) << yAve[i][0] //<< "\t" << setw(10) << yAve2[i][0]
	 << "\t" << setw(10) << yAve[i][1] << "\t" << setw(10) << yAve[i][2] << endl;

    ySum0 += yy[i][0];
    ySum[0] += yAve[i][0];
    ySum[1] += yAve[i][1];
    ySum[2] += yAve[i][2];
    ySum2[0] += yAve2[i][0];
    ySum2[1] += yAve2[i][1];
    ySum2[2] += yAve2[i][2];
    
  }
  cout << setw(10) << "Total" << ":\t " << setw(10) << ySum0 << "\t" << setw(10) << ySum[0] //<< "\t" << setw(10) << ySum2[0]
       << "\t" << setw(10) << ySum[1] << "\t" << setw(10) << ySum[2] << endl;
  

  
  TCanvas *c1 = new TCanvas("c1","",1200, 750);
  c1->SetLeftMargin(0.12);
  c1->SetBottomMargin(0.10);
  // c1->SetGridx();
  // c1->SetGridy();
  c1->Draw();

#ifdef CBM
  const Double_t YMAX[NC] = {33., 33., 33.};
#else
  const Double_t YMAX[NC] = {32., 32., 32.};
#endif
  
  double x1 = 0;
  double x2 = N;
  double y1=0;
  double y2=YMAX[config];
  TH1D *h0 = new TH1D("h0","",1,x1,x2);
  h0->GetXaxis()->SetNdivisions(101);
  h0->GetXaxis()->CenterTitle();
  h0->GetXaxis()->SetTitleOffset(999.);
  h0->GetXaxis()->SetLabelOffset(999.);
  h0->SetYTitle("Total FTEs");
  h0->GetYaxis()->SetNdivisions(505);
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

  //  drawLine(x1, 3, x2, 3, 1, 4, 1);
  //  drawText(1, 1.8, "DF + 2 Jr.", 42, 0.03);
  
  TGraph *gr[NP];
#ifdef CBM  
  const Int_t kColor[NP] = {kOrange, kYellow, kGreen, kPink-4, kMagenta, kCyan, kBlue-3, kBlue-3};
  const Int_t kColorLine[NP] = {kOrange, kGreen+1, kYellow, kPink-4, kMagenta, kBlue-4, kBlue-3, kBlue-3};
#else
  const Int_t kColor[NP] = {kOrange, kYellow, kGreen, kPink-4, kCyan, kBlue-3, kBlue-3};
  const Int_t kColorLine[NP] = {kOrange, kGreen+1, kYellow, kPink-4, kBlue-4, kBlue-3, kBlue-3};
#endif
  

  TGraphErrors *gr_A[NP];
  TGraphErrors *gr_A2[NP];  // 0-2yr
  TGraphErrors *gr_A3[NP];  // 8-10yr
  for(int i=0;i<NP;i++) {
    double xx_tmp[N+1], yy_tmp[N+1], yy_e_tmp[N+1];
    double yy2_tmp[N+1], yy2_e_tmp[N+1];
    for(int j=0;j<N+1;j++) {
      if(i==0) {
	yy_tmp[j] = yyI[i][j]/2.0;
	yy_e_tmp[j] = yyI[i][j]/2.0;
      } else if(i<NP-2) {
	yy_tmp[j] = (yyI[i-1][j] + yyI[i][j])/2.0;
	yy_e_tmp[j] = fabs(yyI[i-1][j] - yyI[i][j])/2.0;	
      } else {
	yy_tmp[j] = (yyI[NP-3][j] + yyI[NP-1][j])/2.0;
	yy_e_tmp[j] = fabs(yyI[NP-3][j] - yyI[NP-1][j])/2.0;
	yy2_tmp[j] = (yyI2[NP-3][j] + yyI2[NP-1][j])/2.0;
	yy2_e_tmp[j] = fabs(yyI2[NP-3][j] - yyI2[NP-1][j])/2.0;
      }
    }
    if(i<NP-2) {
      gr_A[i] = new TGraphErrors(N+1, xx, yy_tmp, 0, yy_e_tmp);
      gr_A[i]->SetFillColorAlpha(kColor[i],0.4);
      gr_A[i]->Draw("e3");
    } else if(i<NP-1) {
      gr_A[i] = new TGraphErrors(ISkip2+1-ISkip, xx+ISkip, yy_tmp+ISkip, 0, yy_e_tmp+ISkip);
      gr_A[i]->SetFillColorAlpha(kColor[i],0.4);
      gr_A[i]->Draw("e3");
      gr_A2[i] = new TGraphErrors(ISkip+1, xx, yy2_tmp, 0, yy2_e_tmp);
      gr_A2[i]->SetFillColorAlpha(kColor[i],0.4);
      gr_A2[i]->Draw("e3");
      gr_A3[i] = new TGraphErrors(N-ISkip2+1, xx+ISkip2, yy2_tmp+ISkip2, 0, yy2_e_tmp+ISkip2);
      gr_A3[i]->SetFillColorAlpha(kColor[i],0.4);
      gr_A3[i]->Draw("e3");
      
    }
    //    gr_A[i]->Print();
  }

  
  for(int i=0;i<NP;i++) {
    if(i==NP-2) continue;
    if(i<NP-2) {
      for(int j=0;j<N;j++) {
	drawLine(xx[j], yyI[i][j], xx[j+1], yyI[i][j+1], 3, 1, kColorLine[i]);
      }
    } else {
      if(config<3) {
	for(int j=0;j<ISkip;j++) {
	  drawLine(xx[j], yyI2[i][j], xx[j+1], yyI2[i][j+1], 3, 1, kColorLine[i]);
	}
	for(int j=ISkip;j<ISkip2;j++) {
	  drawLine(xx[j], yyI[i][j], xx[j+1], yyI[i][j+1], 3, 1, kColorLine[i]);
	}
	for(int j=ISkip2;j<N;j++) {
	  drawLine(xx[j], yyI2[i][j], xx[j+1], yyI2[i][j+1], 3, 1, kColorLine[i]);
	}
	drawLine(xx[ISkip], yyI[i][ISkip+1], xx[ISkip], yyI2[i][ISkip], 3, 1, kColorLine[i]);
	drawLine(xx[ISkip2], yyI[i][ISkip2], xx[ISkip2], yyI2[i][ISkip2+1], 3, 1, kColorLine[i]);
      } else {
	for(int j=0;j<N;j++) {
	  drawLine(xx[j], yyI[i][j], xx[j+1], yyI[i][j+1], 3, 1, kColorLine[i]);
	}
      }
    }
  }


  // baseline sum
  const Double_t baseline[3] = {26., 30., 28.5};
  const Double_t baseline_x[3] = {8, 28, 40};
  //  drawLine(0, baseline[0], baseline_x[0], baseline[0], 3, 9, 2);
  //  drawLine(baseline_x[0], baseline[1], baseline_x[1], baseline[1], 3, 9, 2);
  //  drawLine(baseline_x[1], baseline[2], baseline_x[2], baseline[2], 3, 9, 2);
  
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
    
    drawText(XP[i]+offset, y1-2.5*YMAX[config]/YMAX[0], Form("FY%d",year), 42, 0.06);
  }

#ifdef CBM
  const Double_t xPos[NC][NP] = {
    {1.5, 14, 20, 1., 30, 26, 0, 31},
    {1.5, 14, 20, 1., 30, 26, 0, 31},
    {1.5, 14, 20, 1., 30, 26, 0, 31}
  };
  const Double_t yPos[NC][NP] = {
    {1.0, 1.0, 3.2, 8.8, 4.2, 9.6, 0, 19.6},
    {1.0, 1.0, 2.5, 7.8, 3.8, 8.6, 0, 16.6},
    {1.0, 1.0, 2.4, 7.2, 4.0, 8.6, 0, 17.6}
  };
#else
  const Double_t xPos[NC][NP] = {
    {1.5, 14, 20, 1., 26, 0, 31},
    {1.5, 14, 20, 1., 26, 0, 31},
    {1.5, 12, 20, 1., 26, 0, 31}
  };
  const Double_t yPos[NC][NP] = {
    {1.0, 1.0, 3.2, 8.8, 7.6, 0, 18.6},
    {1.0, 1.0, 2.5, 7.8, 6.6, 0, 17.6},
    {1.0, 1.0, 2.4, 8.2, 7., 0, 15.6}
  };
#endif
    

  for(int i=0;i<NP;i++) {
    double yt = yPos[config][i];
    drawText(xPos[config][i], yt, LabelName[i], 52, 0.045);
  }
  
  //  drawLine(x1, CE, x2, CE, 3, 9, 1);
  //  drawLine(x1, 0, x2, 0, 3, 9, 1);


  c1->Update();
#ifdef CBM
  c1->SaveAs(Form("FTE_%s_LRP.pdf",NameC[config]));
  c1->SaveAs(Form("FTE_%s_LRP.png",NameC[config]));
#else
  c1->SaveAs(Form("FTE_%s.pdf",NameC[config]));
  c1->SaveAs(Form("FTE_%s.png",NameC[config]));
#endif
  
}
