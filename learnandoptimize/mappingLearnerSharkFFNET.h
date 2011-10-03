#ifndef mappingLearnerSHARKFFNET_H
#define mappingLearnerSHARKFFNET_H


/*

This ANN implementation is slower and worse than FANN

*/


#include "globals.h"

#include <stdio.h>
#include <vector>
#include <ReClaM/FFNet.h>
#include <ReClaM/MeanSquaredError.h>
#include <ReClaM/ClassificationError.h>
#include <ReClaM/CG.h>
#include <ReClaM/MeanSquaredError.h>
#include <ReClaM/createConnectionMatrix.h>

#include "mappingLearner.h"


using namespace std;




class mappingLearnerSharkFFNet: public mappingLearner
{

public:

mappingLearnerSharkFFNet(unsigned int numinput, unsigned int numoutput);
~mappingLearnerSharkFFNet();
double train(vector<vector<double> >& inputs, vector<vector<double> >& outputs);
virtual double train(vector<vector<double> >& inputs, vector<vector<double> >& outputs, vector<double>& fitnesses, vector<int>& instances);
double run(vector<double>& input, vector<double>& output);
void save(char *filename);
bool load(char *filename);
bool areYouDirectMapping();


private:


unsigned int num_input;
unsigned int num_output;

FFNet* net;
CG optimizer;


};

bool mappingLearnerSharkFFNet::areYouDirectMapping()
{
return true;
};

double mappingLearnerSharkFFNet::train(vector<vector<double> >& inputs, vector<vector<double> >& outputs, vector<double>& fitnesses, vector<int>& instances)
{
return 0;
}



mappingLearnerSharkFFNet::mappingLearnerSharkFFNet(unsigned int numinput, unsigned int numoutput):num_input(numinput),num_output(numoutput)

	{

	stringstream logfilename;
	

	logfilename<<"learner_log"<<".txt";

	ofstream logfile;

	logfile.open(logfilename.str().c_str());

	logfile<<"SharkFFNet learner "<<endl;
	logfile<<"dimensions input "<<numinput<<" output "<<numoutput<<endl;

	logfile.close();

	

	  Array<int> con;
	  createConnectionMatrix(con, num_input, num_input, num_output);

	net=new FFNet(num_input, num_output, con);
		
	optimizer.init(*net);

	net->initWeights(-0.1, 0.1);




	}


mappingLearnerSharkFFNet::~mappingLearnerSharkFFNet()
{

}

double mappingLearnerSharkFFNet::train(vector<vector<double> >& inputs, vector<vector<double> >& outputs)
	{

	unsigned int s=inputs.size();
	if (s>outputs.size())
		s=outputs.size();

	if(s==0)
		return -1;
	
	if(inputs[0].size() != num_input)
		return -1;
	
	if(outputs[0].size() != num_output)
		return -1;



	Array<double> x(s,num_input);
	Array<double> y(s,num_output);

	
	if(loglevel>2)
		{
		stringstream logfilename;

		logfilename<<"learner_log"<<".txt";

		ofstream logfile;

		logfile.open(logfilename.str().c_str(),ios_base::app);

		for(unsigned int i=0;i<inputs.size();++i)
			{
			logfile<<"train input "<<i<<" ";
			for(unsigned j=0;j<num_input;++j)
				logfile<<inputs[i][j]<<" ";	
			logfile<<endl;
			}

		for(unsigned int i=0;i<outputs.size();++i)
			{
			logfile<<"output "<<i<<" ";
			for(unsigned j=0;j<num_output;++j)
				logfile<<outputs[i][j]<<" ";	
			logfile<<endl;
			}

		logfile.close();
		}

			for(unsigned int i=0;i<inputs.size();++i)
			{

			for(unsigned j=0;j<num_input;++j)
				x(i,j)=inputs[i][j];

			}

		for(unsigned int i=0;i<outputs.size();++i)
			{

			for(unsigned j=0;j<num_output;++j)
				y(i,j)=outputs[i][j];	

			}

	    cout << "optimize "<<numepochsperiteration << endl;

 	
	MeanSquaredError error;
	double err;


	 for (unsigned epoch = 0; epoch < numepochsperiteration; epoch++)
	        {

		optimizer.optimize(*net, error,x, y);
		err=error.error(*net,x,y);
		
		if(loglevel>2)
			cout<< epoch <<" "<< err<<endl;
		}


	return err;

	}




double mappingLearnerSharkFFNet::run(vector<double>& input, vector<double>& output)
{
	if(input.size() != num_input)
		return 0;


	Array<double> x(num_input);
	Array<double> y(num_output);



	for(unsigned int j=0;j<num_input;++j)
			x(j)=input[j];


	net->model(x,y);
	
	output.resize(num_output);
	
	for(unsigned int j=0;j<num_output;++j)
		output[j]=y(j);
			


if(loglevel>2)
		{
		stringstream logfilename;

		logfilename<<"learner_log"<<".txt";

		ofstream logfile;

		logfile.open(logfilename.str().c_str(),ios_base::app);


		logfile<<"run input ";
		for(unsigned j=0;j<input.size();++j)
				logfile<<input[j]<<" ";	
		logfile<<endl;

		logfile<<"output ";
		for(unsigned j=0;j<output.size();++j)
			logfile<<output[j]<<" ";	
		logfile<<endl;


		logfile.close();
		}


return 0; //for direct learner we can not return fitness value
}

void mappingLearnerSharkFFNet::save(char *filename)
{

//cout<< "opening "<< filename<<endl;

ofstream annfile;
annfile.open (filename);

if(!net)
	cout<< "no model to save"<<endl;

if (annfile.is_open() && net)
{
//cout<< "saving"<<endl;
net->write(annfile);
annfile.close();
}
else
	cout<< "could not open file "<< filename <<endl;


}


bool mappingLearnerSharkFFNet::load(char *filename)
{

ifstream annfile;
annfile.open (filename);


if (annfile.is_open()) 
{

net->read(annfile);
annfile.close();
return true;
}

return false;


}



#endif
