#ifndef MAPPINGLEARNERFANN_H
#define MAPPINGLEARNERFANN_H

#include "globals.h"

#include <stdio.h>
#include <vector>
#include "fann.h"
#include "fann_train.h"
#include "mappingLearner.h"


using namespace std;


vector<vector<double> >* m_inputs; 
vector<vector<double> >* m_outputs;

void callbacktraindata(unsigned int num, unsigned int num_input,unsigned int num_output, fann_type * input, fann_type* output); //it is easyer this way than to have it as a member function and then try to cast it


class mappingLearnerFANN: public mappingLearner
{

public:

mappingLearnerFANN(unsigned int numinput, unsigned int numoutput);
~mappingLearnerFANN();
double train(vector<vector<double> >& inputs, vector<vector<double> >& outputs);
void run(vector<double>& input, vector<double>& output);
void save(char *filename);
bool load(char *filename);

double annerror;

private:


unsigned int num_input;
unsigned int num_output;
float desired_error;
struct fann *ann;


};


mappingLearnerFANN::mappingLearnerFANN(unsigned int numinput, unsigned int numoutput):num_input(numinput),num_output(numoutput)

	{

	stringstream logfilename;
	
	m_inputs=NULL;
	m_outputs=NULL;
	unsigned int num_layers = 3;
//	unsigned int num_neurons_hidden = num_input*num_output;
	unsigned int num_neurons_hidden = num_input;
//	unsigned int num_neurons_hidden = 0;
	

	logfilename<<"learner_log"<<".txt";

	ofstream logfile;

	logfile.open(logfilename.str().c_str());

	logfile<<"FANN learner "<<endl;
	logfile<<"dimensions input "<<numinput<<" output "<<numoutput<<" hidden "<<num_neurons_hidden<<endl;

	logfile.close();

	desired_error = (const float) 0.01;

	printf("Creating network.\n");
	ann = fann_create_standard(num_layers, num_input, num_neurons_hidden, num_output);
//	ann = fann_create_standard(num_layers, num_input, num_output);

//	fann_set_activation_steepness_hidden(ann, 1);
	fann_set_activation_steepness_output(ann, 1);

//	fann_set_activation_function_hidden(ann, FANN_SIGMOID);
	fann_set_activation_function_output(ann, FANN_SIGMOID);

	fann_set_train_stop_function(ann, FANN_STOPFUNC_MSE);

	fann_randomize_weights(ann, 0,0.2);


	}


mappingLearnerFANN::~mappingLearnerFANN()
{
fann_destroy(ann);
}

double mappingLearnerFANN::train(vector<vector<double> >& inputs, vector<vector<double> >& outputs)
	{
	
	if(ann)
	{
	
	m_inputs=&inputs;
	m_outputs=&outputs;

	unsigned int s=inputs.size();

	fann_train_data *data;
	
	if(loglevel>2)
		{
		stringstream logfilename;

		logfilename<<"learner_log"<<".txt";

		ofstream logfile;

		logfile.open(logfilename.str().c_str(),ios_base::app);

		for(unsigned int i=0;i<inputs.size();++i)
			{
			logfile<<"train input "<<i<<" ";
			for(unsigned j=0;j<inputs[i].size();++j)
				logfile<<inputs[i][j]<<" ";	
			logfile<<endl;
			}

		for(unsigned int i=0;i<outputs.size();++i)
			{
			logfile<<"output "<<i<<" ";
			for(unsigned j=0;j<outputs[i].size();++j)
				logfile<<outputs[i][j]<<" ";	
			logfile<<endl;
			}

		logfile.close();
		}
	
	fann_randomize_weights(ann, 0,0.2);

	data = fann_create_train_from_callback(s,num_input,num_output,callbacktraindata);

			
	//fann_randomize_weights(ann, 0,0.2);
	fann_reset_MSE(ann);

	cout<<"training for "<<numepochsperiteration<<" iterations"<<endl;
		

	fann_train_on_data(ann, data, numepochsperiteration, numepochsperiteration, 0);

	annerror=fann_test_data(ann, data);

	printf("MSE error on train data: %f\n", annerror);

	fann_destroy_train(data);

	}

	return annerror;

	}

void callbacktraindata(unsigned int num, unsigned int num_input,unsigned int num_output, fann_type * input, fann_type* output)
{

stringstream logfilename;

logfilename<<"learner_log"<<".txt";

ofstream logfile;

logfile.open(logfilename.str().c_str(),ios_base::app);

if(loglevel>2)
	{	
	logfile<<"callback input " <<num<<" ";
	}

for(unsigned int i=0;i<num_input;++i)
	{
	input[i]=(*m_inputs)[num][i];
	if(loglevel>2)
		{	
		logfile<<input[i]<<" ";
		}
	
	//printf("passed train data %d  input: %f \n", i, input[i]);
	}

if(loglevel>2)
	{	
	logfile<<endl;
	logfile<<"output ";
	}


for(int i=0;i<num_output;++i)
	{
	output[i]=(*m_outputs)[num][i];
	//printf("passed train data %d  output: %f \n", i, output[i]);
	if(loglevel>2)
		{	
		logfile<<output[i]<<" ";
		}

	}

if(loglevel>2)
	{	
	logfile<<endl;
	}

logfile.close();

}



void mappingLearnerFANN::run(vector<double>& input, vector<double>& output)
{

if (ann)
{

	fann_type* finput = new fann_type[num_input];

	for(unsigned int j=0;j<num_input;++j)
			finput[j]=(fann_type)input[j];

	//printf("Run ANN\n");

	fann_type* foutput=fann_run(ann,finput);
	
	output.resize(num_output);
	
	for(unsigned int j=0;j<num_output;++j)
		output[j]=foutput[j];
			
	delete finput;

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

}

void mappingLearnerFANN::save(char *filename)
{

if(ann)
	fann_save(ann,filename);
}


bool mappingLearnerFANN::load(char *filename)
{

ifstream annfile;
annfile.open (filename);


if (annfile.is_open()) 
{
annfile.close();
delete ann;
ann=fann_create_from_file(filename);
return true;
}

return false;


}

#endif
