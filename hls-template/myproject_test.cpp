//
//    rfnoc-hls-neuralnet: Vivado HLS code for neural-net building blocks
//
//    Copyright (C) 2017 EJ Kreinar
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "firmware/parameters.h"
#include "firmware/myproject.h"
#include "nnet_helpers.h"

int main(int argc, char **argv) {
  //load input data from text file
  std::ifstream fin("tb_data/tb_input_features.dat");
  //load predictions from text file
  std::ifstream fpr("tb_data/tb_output_predictions.dat");

  std::string iline;
  std::string pline;
  int e = 0;
  if (fin.is_open() && fpr.is_open()) {
	  std::ofstream fout;
	  fout.open("tb_output_data.dat");
	  while ( std::getline(fin,iline) && std::getline (fpr,pline) ) {
	    if (e%5000==0) std::cout << "Processing event " << e << std::endl;
	    e++;
      char* cstr=const_cast<char*>(iline.c_str());
      char* current;
      std::vector<float> in;
      current=strtok(cstr," ");
      while(current!=NULL){
        in.push_back(atof(current));
        current=strtok(NULL," ");
      }
      cstr=const_cast<char*>(pline.c_str());
      std::vector<float> pr;
      current=strtok(cstr," ");
      while(current!=NULL){
        pr.push_back(atof(current));
        current=strtok(NULL," ");
      }

      //hls-fpga-machine-learning insert data
      result_t res_str[N_OUTPUTS] = {0};
      unsigned short size_in, size_out;
      myproject(data_str, res_str, size_in, size_out);

      for(int i=0; i<N_OUTPUTS; i++) {
    	  fout << res_str[i] << " ";
      }
      fout << "\n";
      if (e%5000==0) {
        std::cout << "Predictions" << std::endl;
        for(int i=0; i<N_OUTPUTS; i++) {
    	    std::cout << pr[i] << " ";
        }
        std::cout << std::endl;
        std::cout << "Quantized predictions" << std::endl;
        for(int i=0; i<N_OUTPUTS; i++) {
    	    std::cout << res_str[i] << " ";
        }
        std::cout << std::endl;
      }
    }
    fin.close();
    fpr.close();
    fout.close();
  } else {
    std::cout << "Unable to open input/predictions file" << std::endl;
  }
  
  return 0;
}
