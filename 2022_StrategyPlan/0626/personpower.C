#include "style.C+"
#include "draw.C+"

void personpower(const int config = 0, const int cfg_min = 0)
{
  style();

  const Double_t CE = 24;
  const Int_t N = 40;
  const Int_t NC = 3; // number of configurations

  const Int_t NP = 7;
  const Char_t *Name[NP] = {"EIC","JLab","ALICE3","ALICE","CBM","sPHENIX","STAR"};
  
  Double_t yy_EIC_0[N+1], yy_EIC_diff[N+1];
  Double_t yy_EIC_min[N+1], yy_EIC_diffmin[N+1];
  
  Double_t xx[N+1], yy[NP][N+1], yyI[NP][N+1];

  std::ifstream inData;
  for(int i=0;i<NP;i++) {
    if(i>=1) {
      if(cfg_min==0) inData.open(Form("data/%s_minimum.txt",Name[i]));
      else inData.open(Form("data/%s_default.txt",Name[i]));
    } else {
      inData.open(Form("data/%s_default.txt",Name[i]));
    }
    for(int j=0;j<N+1;j++) {
      if(i!=0) {
	inData >> xx[j] >> yy[i][j];
      } else if (i==0) { // EIC
	inData >> xx[j] >> yy_EIC_0[j];  // EIC total needed
      }
    }
    inData.close();
  }

  inData.open("data/EIC_minimum.txt");
  for(int j=0;j<N+1;j++) {
    inData >> xx[j] >> yy_EIC_min[j];
  }
  inData.close();

  // Re-baseline EIC
  Double_t y_noEIC[N+1];
  for(int j=0;j<N+1;j++) {
    y_noEIC[j] = 0;
    for(int i=1;i<NP;i++) {
      y_noEIC[j] += yy[i][j];
    }

    if(config==0 || config==1) {   // set EIC
      for(int j=0;j<N+1;j++) yy[0][j] = CE - y_noEIC[j];
    }      
    if(config==2) {
      for(int j=0;j<N+1;j++) yy[0][j] = yy_EIC_0[j];
    }
    
    for(int j=0;j<N+1;j++) {
      yy_EIC_diff[j] = yy[0][j] - yy_EIC_0[j];
      yy_EIC_diffmin[j] = yy[0][j] - yy_EIC_min[j];
    }
          
  }

  for(int j=0;j<N+1;j++) {
    yyI[0][j] = yy[0][j];
    for(int i=1;i<NP;i++) {
      yyI[i][j] = yy[i][j] + yyI[i-1][j];
      //      cout << yyI[i][j] << endl;
    }
  }

  Double_t yAve[NP][3];
  const Int_t aveEdge[3] = {8, 20, 40};
  cout << setw(10) << "Project \t" << setw(10) << " t=0 \t" << setw(10) << " 0-2yr\t" << setw(10) << "3-5yr\t" << setw(10) << "6-10yr" << endl;
  ofstream outData;
  for(int i=0;i<NP;i++) {
    outData.open(Form("output/%s.txt",Name[i]));

    for(int jj=0;jj<3;jj++) yAve[i][jj] = 0;
    for(int j=0;j<N+1;j++) {
      double y_tmp = 0;
      if(i==0) {
	y_tmp = yy_EIC_0[j];
      } else {
	y_tmp = yy[i][j];
      }
      outData << setw(6) << j << setw(12) <<  y_tmp << endl;

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

    cout << setw(10) << Name[i] << ":\t " << setw(10) << yy[i][0] << "\t" << setw(10) << yAve[i][0] << "\t" << setw(10) << yAve[i][1] << "\t" << setw(10) << yAve[i][2] << endl;
  }

  
  TCanvas *c1 = new TCanvas("c1","",1200, 600);
  c1->SetLeftMargin(0.09);
  c1->SetBottomMargin(0.12);
  c1->SetGridx();
  c1->SetGridy();
  c1->Draw();

  double x1 = 0;
  double x2 = N;
  double y1 = -12;
  double y2 = 25;
  if(config==2) {
    y1=0;
    y2=37;
  }
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
  h0->Draw("c");

  const Double_t tickL = 1.5;
  for(int i=1;i<N/4;i++) {
    drawLine(i*4, y1, i*4, y1+tickL, 2, 1, 1);
    drawLine(i*4, y2, i*4, y2-tickL, 2, 1, 1);
    drawLine(i*4, y1+tickL, i*4, y2-tickL, 1, 2, 8);
  }

  drawLine(x1, 3, x2, 3, 1, 4, 1);
  drawText(1, 1.8, "DF + 2 Jr.", 42, 0.03);
  
  TGraph *gr[NP];
  const Int_t kColor[NP] = {kBlue-2, kTeal, kMagenta, kViolet, kSpring+6, kYellow, kOrange};
  for(int i=0;i<NP;i++) {
    for(int j=0;j<N;j++) {
      drawLine(xx[j], yyI[i][j], xx[j+1], yyI[i][j+1], 3, 1, kColor[i]);
    }
  }

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
    //    gr_A[i]->Print();
  }

  TGraphErrors *gr_EIC_diff;
  double y_diff[N+1], y_e_diff[N+1];
  if(config>0) {
    for(int j=0;j<N+1;j++) {
      y_diff[j] = yy_EIC_diff[j]/2.;
      y_e_diff[j] = fabs(yy_EIC_diff[j])/2.;
      if(j<N) drawLine(xx[j], yy_EIC_diff[j], xx[j+1], yy_EIC_diff[j+1], 3, 1, kColor[0]);
    }
    gr_EIC_diff = new TGraphErrors(N+1, xx, y_diff, 0, y_e_diff);
    gr_EIC_diff->SetFillColorAlpha(kColor[0], 0.4);
    gr_EIC_diff->Draw("e3");

    for(int j=0;j<N+1;j++) {
      if(j<N) drawLine(xx[j], yy_EIC_diffmin[j], xx[j+1], yy_EIC_diffmin[j+1], 3, 2, 5);
    }

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
    
    drawText(XP[i]+offset, y1-3, Form("FY%d",year), 42, 0.06);
  }

  const Double_t xPos[NC][NP] = { {34, 12, 34, 16, 34, 12, 1.5},
				  {34, 12, 34, 16, 34, 12, 1.5},
				  {34, 12, 34, 16, 34, 12, 1.5}};
  const Double_t yPos[NC][NP] = { {4.0, 7.8, 12.4, 15.4, 21.7, 20.6, 20.8},
				  {4.0, 7.8, 12.4, 15.4, 21.7, 20.6, 20.8},
				  {6.0, 17.6, 20.8, 24.2, 30.2, 30.6, 26.6}};

  for(int i=0;i<NP;i++) {
    double yt = yPos[config][i];
    if(cfg_min==0 && i==1) yt += 3.5;
    if(cfg_min!=0 || (i!=2 && i!=4) ) drawText(xPos[config][i], yt, Name[i], 52, 0.055);
  }
  
  drawLine(x1, CE, x2, CE, 3, 9, 1);
  drawLine(x1, 0, x2, 0, 3, 9, 1);

  drawText(36, yy_EIC_diffmin[36]+0.2, "minimum", 12, 0.045);
  drawText(36, yy_EIC_diff[36]-1.2, "default", 12, 0.045);

  c1->Update();
  c1->SaveAs(Form("personpower_%d_%d.pdf",config,cfg_min));
  c1->SaveAs(Form("personpower_%d_%d.png",config,cfg_min));
  
}
