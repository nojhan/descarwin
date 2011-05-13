#include "globals.h"
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include "time.h"
#include "mappingLearnerFANN.h"
#include "mappingLearnerSVM.h"
#include "mappingLearnerSharkFFNET.h"



using namespace std;






int main ( int argc, char* argv[] )
{



mappingLearner* mappinglearner;

	checkexitandreadconfig();


/*
	if(learningModelType==SharkFFNET)
		mappinglearner= dynamic_cast<mappingLearner*>(new mappingLearnerSharkFFNet(4,4)); // 4 dimensional input and output
	else
		mappinglearner= dynamic_cast<mappingLearner*>(new mappingLearnerFANN(4,4)); 
*/

mappinglearner= dynamic_cast<mappingLearner*>(new mappingLearnerSVM(4,4)); // 4 dimensional input and output


int s=20;




vector<vector<double> > inputs;		
vector<vector<double> > outputs;

		for(unsigned int i=0;i<s;i+=2)
		for(unsigned int j=0;j<s;j+=2)
		for(unsigned int k=0;k<s;k+=2)
		for(unsigned int l=0;l<s;l+=2)
		{

		vector<double> input(4,0);
		vector<double> output(4,0);

		double x=(double)i/s;
		double y=(double)j/s;
		double v=(double)k/s;
		double w=(double)l/s;

		
		input[0]=x;
		input[1]=y;
		input[2]=k;
		input[3]=l;
		
		output[0]=sin(x)*sin(y)*sin(v)*sin(w);
		output[1]=cos(x)*cos(y)*cos(v)*cos(w);
		output[2]=cos(x)*sin(y)*cos(v)*sin(w);;
		output[3]=cos(x)*sin(y)*cos(v)*sin(w);;

		//cout<< x << " " << y << " " << v << " " <<w<< " " << output[0] << " " << output[1]<< " " << output[2]<< " " << output[3]<<endl;

		inputs.push_back(input);
		outputs.push_back(output);
		}	



		double annerror=mappinglearner->train(inputs,outputs);

		cout<<"trainerror "<<annerror<<endl;



inputs.clear();
outputs.clear();

		for(unsigned int i=1;i<s;i+=2)
		for(unsigned int j=1;j<s;j+=2)
		for(unsigned int k=1;k<s;k+=2)
		for(unsigned int l=1;l<s;l+=2)
		{

		vector<double> input(4,0);
		vector<double> output(4,0);

		double x=(double)i/s;
		double y=(double)j/s;
		double v=(double)k/s;
		double w=(double)l/s;

		
		input[0]=x;
		input[1]=y;
		input[2]=k;
		input[3]=l;
		
		output[0]=sin(x)*sin(y)*sin(v)*sin(w);
		output[1]=cos(x)*cos(y)*cos(v)*cos(w);
		output[2]=cos(x)*sin(y)*cos(v)*sin(w);;
		output[3]=cos(x)*sin(y)*cos(v)*sin(w);;

		//cout<< x << " " << y << " " << v << " " <<w<< " " << output[0] << " " << output[1]<< " " << output[2]<< " " << output[3]<<endl;

		inputs.push_back(input);
		outputs.push_back(output);
		}	

double testerror=0;

		cout<< "testing"<<endl;


		for (unsigned int n=0;n<inputs.size();++n)
			{
			vector<double> thisoutput;
			mappinglearner->run(inputs[n], thisoutput);

			//cout<< outputs[n][0] << " " << outputs[n][1] << " " << outputs[n][2] << " " << outputs[n][3]<<endl;
			//cout<< thisoutput[0] << " " << thisoutput[1] << " " << thisoutput[2] << " " << thisoutput[3]<<endl;
			//cout<<endl;

			double thiserror=0;
			for(unsigned int i=0;i<4;++i)
				thiserror+=(outputs[n][i]-thisoutput[i])*(outputs[n][i]-thisoutput[i]);
			thiserror=sqrt(thiserror);
			testerror+=thiserror;

			}

cout<<"testerror this should be around 0.06 :"<<testerror/inputs.size()<<endl;



cout<< "saving"<<endl;

mappinglearner->save("unittest.mappinglearner");
cout<< "loading"<<endl;

mappingLearner* mappinglearner2;

	checkexitandreadconfig();
/*
	if(learningModelType==SharkFFNET)
		mappinglearner2= dynamic_cast<mappingLearner*>(new mappingLearnerSharkFFNet(4,4)); // 4 dimensional input and output
	else
		mappinglearner2= dynamic_cast<mappingLearner*>(new mappingLearnerFANN(4,4)); 
*/

mappinglearner= dynamic_cast<mappingLearner*>(new mappingLearnerSVM(4,4)); // 4 dimensional input and output

mappinglearner2->load("unittest.mappinglearner");


		testerror=0;

		cout<< "testing"<<endl;


		for (unsigned int n=0;n<inputs.size();++n)
			{
			vector<double> thisoutput;
			mappinglearner2->run(inputs[n], thisoutput);

			//cout<< outputs[n][0] << " " << outputs[n][1] << " " << outputs[n][2] << " " << outputs[n][3]<<endl;
			//cout<< thisoutput[0] << " " << thisoutput[1] << " " << thisoutput[2] << " " << thisoutput[3]<<endl;
			//cout<<endl;

			double thiserror=0;
			for(unsigned int i=0;i<4;++i)
				thiserror+=(outputs[n][i]-thisoutput[i])*(outputs[n][i]-thisoutput[i]);
			thiserror=sqrt(thiserror);
			testerror+=thiserror;

			}

cout<<"testerror this should be around 0.06 :"<<testerror/inputs.size()<<endl;


		

	return 0;
}

