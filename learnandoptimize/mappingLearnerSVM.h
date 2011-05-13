#ifndef mappingLearnerSVM_H
#define mappingLearnerSVM_H

#include "globals.h"

#include <stdio.h>
#include <vector>
#include <ReClaM/Svm.h>
#include <ReClaM/MeanSquaredError.h>
#include <ReClaM/ClassificationError.h>

#include "mappingLearner.h"


using namespace std;




class mappingLearnerSVM: public mappingLearner
{

public:

mappingLearnerSVM(unsigned int numinput, unsigned int numoutput);
~mappingLearnerSVM();
double train(vector<vector<double> >& inputs, vector<vector<double> >& outputs);
void run(vector<double>& input, vector<double>& output);
void save(char *filename);
bool load(char *filename);


private:


unsigned int num_input;
unsigned int num_output;

RBFKernel* kernel;
//PolynomialKernel* kernel;

SVM* svm;
Epsilon_SVM* esvm;
//C_SVM* csvm;

SVM_Optimizer svmopt;



};


mappingLearnerSVM::mappingLearnerSVM(unsigned int numinput, unsigned int numoutput):num_input(numinput),num_output(numoutput)

	{

	stringstream logfilename;
	

	logfilename<<"learner_log"<<".txt";

	ofstream logfile;

	logfile.open(logfilename.str().c_str());

	logfile<<"SVM learner "<<endl;
	logfile<<"dimensions input "<<numinput<<" output "<<numoutput<<endl;

	logfile.close();

	double C = 100.0;
    	double epsilon = 0.01;
	double sigma = 2.0;

	// create the SVM for prediction
    	double gamma = 0.5 / (sigma * sigma);
	kernel=new RBFKernel(gamma);
//	kernel=new PolynomialKernel(10, 0);
	svm=new SVM(kernel, false);


	// create a training scheme and an optimizer for learning
	    esvm=new Epsilon_SVM(svm, C, epsilon);
	//csvm=new C_SVM(svm, C, C);

	    svmopt.init(*esvm);
//	svmopt.init(*csvm);



	}


mappingLearnerSVM::~mappingLearnerSVM()
{

}

double mappingLearnerSVM::train(vector<vector<double> >& inputs, vector<vector<double> >& outputs)
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

	    cout << "optimize "<< endl;

		svmopt.optimize(*svm, x, y);

	    cout << "getting error "<< endl;

	 // compute the mean squared error on the training data:
	    MeanSquaredError mse;

//	    ClassificationError mse;
 		   double err = mse.error(*svm, x, y);
	    cout << "mean squared error on the training data: " << err << endl << endl;


	return err;

	}




void mappingLearnerSVM::run(vector<double>& input, vector<double>& output)
{
	if(input.size() != num_input)
		return;


	Array<double> x(num_input);
	Array<double> y(num_output);



	for(unsigned int j=0;j<num_input;++j)
			x(j)=input[j];


	svm->model(x,y);
	
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



}

void mappingLearnerSVM::save(char *filename)
{
}


bool mappingLearnerSVM::load(char *filename)
{



}

#endif
