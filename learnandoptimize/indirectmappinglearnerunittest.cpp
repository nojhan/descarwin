#include "globals.h"
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include "time.h"
#include "mappingLearnerFANNIndirect.h"
#include "mappingLearnerRankSVMIndirect.h"


using namespace std;






int main ( int argc, char* argv[] )
{



mappingLearner *mappinglearner, *mappinglearner2;

	checkexitandreadconfig();



if (learningModelType==FANNIndirect)
	mappinglearner= dynamic_cast<mappingLearner*>(new mappingLearnerFANNIndirect(2,2)); // 2 dimensional input and output
else if (learningModelType==RankSVMIndirect)
	mappinglearner= dynamic_cast<mappingLearner*>(new mappingLearnerRankSVMIndirect(2,2));



int s=10;


vector<vector<double> > inputs;		
vector<vector<double> > outputs;
vector<double> fitnesses;
vector<int> instances;

		for(unsigned int i=0;i<s;i+=2)
		for(unsigned int j=0;j<s;j+=2)
		for(unsigned int k=0;k<s;k+=2)
		for(unsigned int l=0;l<s;l+=2)
		{

		vector<double> input(2,0);
		vector<double> output(2,0);

		double x=(double)i/s;
		double y=(double)j/s;
		double v=(double)k/s;
		double w=(double)l/s;

		
		input[0]=x;
		input[1]=y;
		
		output[0]=v;
		output[1]=w;
	
		double f=0.1+0.4*(x-v)*(x-v)+0.4*(y-w)*(y-w);
		//double f=0.1+0.4*(x-v)+0.4*(y-w); linear has no sense in optimization
		
		//double f=0.1+x+v+y+w;


		if (f>1) f=1;

		//cout<< x << " " << y << " " << v << " " << w << " "<<f <<endl;
		fitnesses.push_back(f);
		instances.push_back(1); //everything is one big instance
		inputs.push_back(input);
		outputs.push_back(output);

		}	

		cout<<"start train "<<endl;

		double error=mappinglearner->train(inputs,outputs,fitnesses,instances);

		cout<<"trainerror "<<error<<endl;


		mappinglearner->save("unittest.mappinglearner");



		inputs.clear();
		outputs.clear();

		for(unsigned int i=1;i<s;i+=2)
		for(unsigned int j=1;j<s;j+=2)
		{

		vector<double> input(2,0);
		vector<double> output(2,0);

		double x=(double)i/s;
		double y=(double)j/s;

		
		input[0]=x;
		input[1]=y;
		
		output[0]=x; //desired
		output[1]=y;

		//cout<< x << " " << y << " " << v << " " <<w<< " " << output[0] << " " << output[1]<< " " << output[2]<< " " << output[3]<<endl;

		inputs.push_back(input);
		outputs.push_back(output);
		}	

		double regressionerror=0;
		double performancelost=0;


		cout<< "testing"<<endl;

		inputs.clear();
		outputs.clear();


		for(unsigned int i=0;i<s;i+=2)
		for(unsigned int j=0;j<s;j+=2)
			{

			vector<double> input(2,0);

			double x=(double)i/s;
			double y=(double)j/s;
			input[0]=x;
			input[1]=y;
			inputs.push_back(input);
			outputs.push_back(input); //the desired output is the same
			
			}



//		for (unsigned int n=0;n<inputs.size();++n)
		for (unsigned int n=10;n<20;++n)
			{
			

			vector<double> bestoutput;

			double bestfitness=invalidresult;

			for(unsigned int x=0;x<5;++x) //1 restarts not needed
				{
				vector<double> thisoutput;
				double fitness=	mappinglearner->run(inputs[n], thisoutput);
				cout<< x<< " predicted output "<< thisoutput[0] << " " << thisoutput[1] <<endl;
				if(fitness<bestfitness)
					{
					bestoutput=thisoutput;
					bestfitness=fitness;
					}
					
				}

			cout<< "target output "<< outputs[n][0] << " " << outputs[n][1] <<endl;
			cout<< "predicted output "<< bestoutput[0] << " " << bestoutput[1] <<endl;
			//cout<<endl;

			double thiserror=0;
			for(unsigned int i=0;i<2;++i)
				thiserror+=(outputs[n][i]-bestoutput[i])*(outputs[n][i]-bestoutput[i]);
			thiserror=sqrt(thiserror);
			regressionerror+=thiserror;



			}

cout<<"regression error this should be around 0.007 :"<<regressionerror/inputs.size()<<endl;
//cout<<"performancelost loss error this should be around 0.06 :"<<performancelost/inputs.size()<<endl;


//cout<<"regression error this should be around 0.06 :"<<regressionerror<<endl;




cout<< "saving"<<endl;

mappinglearner->save("unittest.mappinglearner");
cout<< "loading"<<endl;



if (learningModelType==FANNIndirect)
	mappinglearner2= dynamic_cast<mappingLearner*>(new mappingLearnerFANNIndirect(2,2)); // 2 dimensional input and output
else if (learningModelType==RankSVMIndirect)
		mappinglearner2= dynamic_cast<mappingLearner*>(new mappingLearnerRankSVMIndirect(2,2));





	checkexitandreadconfig();


mappinglearner2->load("unittest.mappinglearner");

		cout<< "testing"<<endl;

		inputs.clear();
		outputs.clear();


		for(unsigned int i=0;i<s;i+=2)
		for(unsigned int j=0;j<s;j+=2)
			{

			vector<double> input(2,0);

			double x=(double)i/s;
			double y=(double)j/s;
			input[0]=x;
			input[1]=y;
			inputs.push_back(input);
			outputs.push_back(input); //the desired output is the same
			
			}



//		for (unsigned int n=0;n<inputs.size();++n)
		for (unsigned int n=10;n<20;++n)
			{
			

			vector<double> bestoutput;

			double bestfitness=invalidresult;

			for(unsigned int x=0;x<5;++x) //1 restarts not needed
				{
				vector<double> thisoutput;
				double fitness=	mappinglearner2->run(inputs[n], thisoutput);
				cout<< x<< " predicted output "<< thisoutput[0] << " " << thisoutput[1] <<endl;
				if(fitness<bestfitness)
					{
					bestoutput=thisoutput;
					bestfitness=fitness;
					}
					
				}

			cout<< "target output "<< outputs[n][0] << " " << outputs[n][1] <<endl;
			cout<< "predicted output "<< bestoutput[0] << " " << bestoutput[1] <<endl;
			//cout<<endl;

			double thiserror=0;
			for(unsigned int i=0;i<2;++i)
				thiserror+=(outputs[n][i]-bestoutput[i])*(outputs[n][i]-bestoutput[i]);
			thiserror=sqrt(thiserror);
			regressionerror+=thiserror;



			}

cout<<"regression error this should be around 0.007 :"<<regressionerror/inputs.size()<<endl;
//cout<<"performancelost loss error this should be around 0.06 :"<<performancelost/inputs.size()<<endl;


//cout<<"regression error this should be around 0.06 :"<<regressionerror<<endl;


		

	return 0;
}


