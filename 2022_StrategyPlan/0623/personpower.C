#include "style.C+"
#include "draw.C+"

void personpower()
{
  style();

  const Int_t N = 40;

  const Int_t NP = 6;
  const Char_t *Name[NP] = {"EIC","ALICE3","CBM","ALICE","sPHENIX","STAR"};
  const Char_t *EICName[3] = {"EIC1","EIC2","EIC_instr"};
  const Char_t *sPHName[2] = {"sPHENIX","sPHENIX_instr"};
  

  Double_t xx[N+1], yy[NP][N+1], yyI[NP][N+1];

  for(int i=0;i<NP;i++) {
    std::ifstream inData;
    if(i==1 || i==2 || i==3 || i==5) {
      inData.open(Form("data/%s.txt",Name[i]));
      for(int j=0;j<N+1;j++) {
	inData >> xx[j] >> yy[i][j];
      }
      inData.close();
    } else if (i==0) { // EIC
      for(int j=0;j<N+1;j++) yy[i][j] = 0;
      for(int ii=0;ii<3;ii++) {
	inData.open(Form("data/%s.txt",EICName[ii]));
	for(int j=0;j<N+1;j++) {
	  double y_tmp;
	  inData >> xx[j] >> y_tmp;
	  yy[i][j] += y_tmp;
	}
	inData.close();
      }		  
    } else if (i==4) { // sPHENIX
      for(int j=0;j<N+1;j++) yy[i][j] = 0;
      for(int ii=0;ii<2;ii++) {
	inData.open(Form("data/%s.txt",sPHName[ii]));
	for(int j=0;j<N+1;j++) {
	  double y_tmp;
	  inData >> xx[j] >> y_tmp;
	  yy[i][j] += y_tmp;
	}
	inData.close();
      }		  
    }
  }

  for(int j=0;j<N+1;j++) {
    yyI[0][j] = yy[0][j];
    for(int i=1;i<NP;i++) {
      yyI[i][j] = yy[i][j] + yyI[i-1][j];
      cout << yyI[i][j] << endl;
    }
  }

  
  TCanvas *c1 = new TCanvas("c1","",1200, 600);
  c1->SetLeftMargin(0.09);
  c1->SetBottomMargin(0.14);
  c1->SetGridx();
  c1->SetGridy();
  c1->Draw();

  double x1 = 0;
  double x2 = N;
  double y1 = 0;
  double y2 = 17;
  TH1D *h0 = new TH1D("h0","",1,x1,x2);
  h0->GetXaxis()->SetNdivisions(505);
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

  TGraph *gr[NP];
  const Int_t kColor[NP] = {kBlue-2, kMagenta, kGreen+1, kCyan, kOrange, kRed};
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
    gr_A[i]->Print();
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
    
    drawText(XP[i]-offset, -1.6, Form("FY%d",year), 42, 0.06);
  }

  drawText(2, 14.4, "STAR", 52, 0.06);
  drawText(12, 13.6, "sPHENIX", 52, 0.06);
  drawText(16, 9.4, "ALICE", 52, 0.06);
  drawText(32, 10.2, "CBM", 52, 0.06);
  drawText(34, 7.8, "ALICE3", 52, 0.06);
  drawText(34, 3.0, "EIC", 52, 0.06);

  c1->Update();
  c1->SaveAs("personpower.pdf");
  c1->SaveAs("personpower.png");
  
}
