#include "style.C+"
#include "draw.C+"

void personpower_Time()
{
  style();

  const Int_t N = 40;
  const Int_t NN = 3;

  const Int_t NP = 7;
  const Char_t *Name[NP] = {"EIC", "JLab", "ALICE3", "ALICE", "CBM", "sPHENIX", "STAR"};

  const Double_t Edge[NN] = {8.0, 20.0, 40.0};
  Double_t frac_i[NP][NN][2] = {
    {{4.0, 12.5}, {14.0, 17.5}, {30., 18.5}},				    
    {{4.0, 2.4}, {14.0, 2.1}, {30., 1.7}},
    {{4.0, 1.0}, {14.0, 1.5}, {30., 2.5}},
    {{4.0, 5.0}, {14.0, 4.5}, {30., 3.5}},
    //				    {{4.0, 0.6}, {14.0, 0.8}, {30., 1.9}}, // EIC-HF
    {{4.0, 0.4}, {14.0, 1.1}, {30., 2.1}},
    {{4.0, 2.5}, {14.0, 4.0}, {30., 2.7}},
    {{4.0, 3.5}, {14.0, 1.1}, {30., 0.3}}
  };

  Double_t frac[NP][NN][2] = {0.};
  for(int j=0;j<NN;j++) {
    frac[0][j][0] = frac_i[0][j][0];
    frac[0][j][1] = frac_i[0][j][1];
    for(int i=1;i<NP;i++) {
      frac[i][j][0] = frac[i-1][j][0] + frac_i[i][j][0];
      frac[i][j][1] = frac[i-1][j][1] + frac_i[i][j][1];
    }
  }

  TCanvas *c1 = new TCanvas("c1","",1200, 600);
  c1->SetLeftMargin(0.09);
  c1->SetBottomMargin(0.14);
  c1->SetGridx();
  //  c1->SetGridy();
  c1->Draw();

  double x1 = 0;
  double x2 = N;
  double y1 = 0;
  double y2 = 35;
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
  const Int_t kColor[NP] = {kBlue-2, kPink, kMagenta, kCyan, kGreen+1, kOrange, kRed};
  // for(int i=0;i<NP;i++) {
  //   double x_tmp[NN];
  //   double y_tmp[NN];
  //   for(int j=0;j<NN;j++) {
  //     x_tmp[j] = frac[i][j][0];
  //     y_tmp[j] = frac[i][j][1];
  //     //      drawLine(frac[i][j][0], frac[i][j][1], frac[i][j+1][0], frac[i][j+1][1], 3, 1, kColor[i]);
  //   }
  //   gr[i] = new TGraph(NN, x_tmp, y_tmp);
  // }

  TGraphErrors *gr_A[NP];
  for(int i=0;i<NP;i++) {
    double xx_tmp[N+1], yy_tmp[N+1], yy_e_tmp[N+1];
    for(int j=0;j<N+1;j++) {
      xx_tmp[j] = j;
      int i_j = 0;
      for(int k=0;k<NN;k++) {
	if((xx_tmp[j]-0.1)<Edge[k]) {
	  i_j = k;
	  break;
	}
      }

      if(i==0) {
       	yy_tmp[j] = frac[i][i_j][1]/2.0;
       	yy_e_tmp[j] = frac[i][i_j][1]/2.0;
      } else {
       	yy_tmp[j] = (frac[i][i_j][1] + frac[i-1][i_j][1])/2.0;
       	yy_e_tmp[j] = fabs(frac[i][i_j][1] - frac[i-1][i_j][1])/2.0;	
      }
      
      // if(i==0) {
      // 	yy_tmp[j] = gr[i]->Eval(xx_tmp[j])/2.0;
      // 	yy_e_tmp[j] = gr[i]->Eval(xx_tmp[j])/2.0;
      // } else {
      // 	yy_tmp[j] = (gr[i-1]->Eval(xx_tmp[j]) + gr[i]->Eval(xx_tmp[j]))/2.0;
      // 	yy_e_tmp[j] = fabs(gr[i-1]->Eval(xx_tmp[j]) - gr[i]->Eval(xx_tmp[j]))/2.0;	
      // }
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
    
    drawText(XP[i]-offset, -3, Form("FY%d",year), 42, 0.06);
  }

  // drawText(18, 2.2, "ALICE", 52, 0.06);
  // drawText(32, 5.0, "ALICE3", 52, 0.06);
  // drawText(34, 6.2, "EIC", 52, 0.06);

  drawLine(x1, 22.8, x2, 22.8, 3, 2, 1);

  c1->Update();
  c1->SaveAs("personpower_Time.pdf");
  c1->SaveAs("personpower_Time.png");

  std::ofstream outData;
  for(int i=0;i<NP;i++) {
    outData.open(Form("data/%s.txt",Name[i]));
    for(int j=0;j<N+1;j++) {
      outData << setw(6) << gr_A[i]->GetX()[j] << setw(12) << gr_A[i]->GetEY()[j]*2 << std::endl;
    }
    outData.close();
  }
}
