#ifndef mappingLearnerSHARKGauss_H
#define mappingLearnerSHARKGauss_H


/*

This ANN implementation is slower and worse than FANN

*/


#include "globals.h"

#include <stdio.h>
#include <vector>
#include <ReClaM/GaussianProcess.h>
#include <ReClaM/MeanSquaredError.h>
#include <ReClaM/ClassificationError.h>
#include <ReClaM/CG.h>
#include <ReClaM/MeanSquaredError.h>
#include <ReClaM/createConnectionMatrix.h>

#include "mappingLearner.h"


using namespace std;




class mappingLearnerSharkGauss: public mappingLearner
{

public:

mappingLearnerSharkGauss(unsigned int numinput, unsigned int numoutput);
~mappingLearnerSharkGauss();
double train(vector<vector<double> >& inputs, vector<vector<double> >& outputs);
virtual double train(vector<vector<double> >& inputs, vector<vector<double> >& outputs, vector<double>& fitnesses, vector<int>& instances);
double run(vector<double>& input, vector<double>& output);
void save(char *filename);
bool load(char *filename);
bool areYouDirectMapping();


private:


unsigned int num_input;
unsigned int num_output;

vector<GaussianProcess*>  gaussianprocess;

vector<RBFKernel*> kernel;
//PolynomialKernel* kernel;

vector<SVM*> svm;
vector<Epsilon_SVM*> esvm;
//C_SVM* csvm;



};


bool mappingLearnerSharkGauss::areYouDirectMapping()
{
return true;
};

double mappingLearnerSharkGauss::train(vector<vector<double> >& inputs, vector<vector<double> >& outputs, vector<double>& fitnesses, vector<int>& instances)
{
return 0;
}


mappingLearnerSharkGauss::mappingLearnerSharkGauss(unsigned int numinput, unsigned int numoutput):num_input(numinput),num_output(numoutput)

	{

	stringstream logfilename;
	

	logfilename<<"learner_log"<<".txt";

	ofstream logfile;

	logfile.open(logfilename.str().c_str());

	logfile<<"SharkGauss learner "<<endl;
	logfile<<"dimensions input "<<numinput<<" output "<<numoutput<<endl;

	logfile.close();
	
	double C = 100.0;
    	double epsilon = 0.01;
	double sigma = 2.0;


	// create the SVM for prediction
    	double gamma = 0.5 / (sigma * sigma);

	for(unsigned int i=0;i<numoutput;++i)
		{
		RBFKernel* thiskernel=new RBFKernel(gamma);
		kernel.push_back(thiskernel);
	
		SVM* thissvm=new SVM(thiskernel, false);
		
		svm.push_back(thissvm);
		
		// create a training scheme and an optimizer for learning
		Epsilon_SVM* thisesvm=new Epsilon_SVM(thissvm, C, epsilon);

		esvm.push_back(thisesvm);
	
		GaussianProcess* thisgaussianprocess=new GaussianProcess((SVM*)thisesvm);

		gaussianprocess.push_back(thisgaussianprocess);

		}
		

	}


mappingLearnerSharkGauss::~mappingLearnerSharkGauss()
{

}

double mappingLearnerSharkGauss::train(vector<vector<double> >& inputs, vector<vector<double> >& outputs)
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
				y(j,i)=outputs[i][j];	

			}

	cout<< " y dimensions " <<y.rows() << " "<<y.cols()<<endl;
	cout<< " x dimensions " <<x.rows() << " "<<x.cols()<<endl;

	for(unsigned int i=0;i<num_output;++i)
		{
		cout<<"training svm # "<<i<<endl;
		Array<double> t(s);
		t=y.col(i);
		cout<< "t size "<<t.dim(0)<<endl;
		
		gaussianprocess[i]->train(x,t);
		}
	
	 

	return 0;

	}




double mappingLearnerSharkGauss::run(vector<double>& input, vector<double>& output)
{
	if(input.size() != num_input)
		return 0;


	Array<double> x(num_input);



	for(unsigned int j=0;j<num_input;++j)
			x(j)=input[j];


	
	output.resize(num_output);
	
	for(unsigned int j=0;j<num_output;++j)
		{
		GaussianProcess* gp=gaussianprocess[j];
		double y=(*gp)(x);
		output[j]=y;
		}
			


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

void mappingLearnerSharkGauss::save(char *filename)
{

//cout<< "opening "<< filename<<endl;

ofstream annfile;
annfile.open (filename);


if (annfile.is_open())
{
//cout<< "saving"<<endl;

annfile.close();
}
else
	cout<< "could not open file "<< filename <<endl;


}


bool mappingLearnerSharkGauss::load(char *filename)
{

ifstream annfile;
annfile.open (filename);


if (annfile.is_open()) 
{


annfile.close();
return true;
}

return false;


}



#endif
