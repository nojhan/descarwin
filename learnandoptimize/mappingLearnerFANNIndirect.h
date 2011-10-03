#ifndef mappingLearnerFANNIndirect_H
#define mappingLearnerFANNIndirect_H

#include "globals.h"
#include "optimizer.h"

#include <stdio.h>
#include <vector>
#include "fann.h"
#include "fann_train.h"
#include "mappingLearner.h"
#include "mappingLearnerFANN.h"


using namespace std;




class mappingLearnerFANNIndirect: public mappingLearner
{

public:

mappingLearnerFANNIndirect(unsigned int numinput, unsigned int numoutput);
~mappingLearnerFANNIndirect();
double train(vector<vector<double> >& inputs, vector<vector<double> >& outputs);
virtual double train(vector<vector<double> >& inputs, vector<vector<double> >& outputs, vector<double>& fitnesses, vector<int>& instances);
double run(vector<double>& input, vector<double>& output);
void save(char *filename);
bool load(char *filename);
bool areYouDirectMapping();

double annerror;

private:


unsigned int num_input;
unsigned int num_output;
unsigned int dim_parameters;


float desired_error;
struct fann *ann;


};

bool mappingLearnerFANNIndirect::areYouDirectMapping()
{
return false;
};

double mappingLearnerFANNIndirect::train(vector<vector<double> >& inputs, vector<vector<double> >& outputs, vector<double>& fitnesses, vector<int>& instances)
{

if(ann)
	{
	
	//add data to previous

	//cout<<inputs.size()<<" "<<outputs.size()<<" "<<fitnesses.size()<<endl;

	if(loglevel>4)
		cout<<"adding new data to the pool to learn"<<endl;


	for (unsigned int i=0;i<inputs.size();++i)
		{
/*		cout<<"input ";
		for(unsigned int j=0;j<inputs[i].size();j++)
			cout<<inputs[i][j]<<" ";
		cout<<endl;
*/

		vector<double> combinedinput;

		combinedinput.reserve(inputs[i].size()+outputs[i].size());


		combinedinput.insert(combinedinput.end(),inputs[i].begin(),inputs[i].end());


	
		unsigned int osize=outputs[i].size();

/*
		cout<<"output ";
		for(unsigned int j=0;j<osize;j++)
			cout<<outputs[i][j]<<" ";
		cout<<endl;

*/
		combinedinput.insert(combinedinput.end(),outputs[i].begin(),outputs[i].end());

		//cout<<combinedinput.size()<<endl;
/*		
		cout<<"combined ";
		for(unsigned int j=0;j<combinedinput.size();j++)
			cout<<combinedinput[j]<<" ";
		cout<<fitnesses[i]<<endl;
*/		

		m_inputs->push_back(combinedinput);

		vector<double> realoutput;
		realoutput.push_back(fitnesses[i]);
		

		m_outputs->push_back(realoutput);

		}

	


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
		for(unsigned int i=0;i<fitnesses.size();++i)
			logfile<<"fitness "<<i<<" "<<fitnesses[i]<<endl;


		logfile.close();
		}


	
	fann_randomize_weights(ann, 0,0.2);
	
	unsigned int s=m_inputs->size();

	if(loglevel>4)
		cout<<"create FANN train with callback (this takes a while)"<<endl;

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


mappingLearnerFANNIndirect::mappingLearnerFANNIndirect(unsigned int numinput, unsigned int numoutput):num_input(numinput+numoutput),num_output(1),dim_parameters(numoutput)

	{

	stringstream logfilename;
	
	m_inputs=new vector<vector<double> >;
	m_outputs=new vector<vector<double> >;


	unsigned int num_layers = 3;
//	unsigned int num_neurons_hidden = num_input*num_output;
	unsigned int num_neurons_hidden = num_input;
//	unsigned int num_neurons_hidden = 0;
	

	logfilename<<"learner_log"<<".txt";

	ofstream logfile;

	logfile.open(logfilename.str().c_str());

	logfile<<"FANN learner "<<endl;
	logfile<<"dimensions input "<<num_input<<" output "<<num_output<<" hidden "<<num_neurons_hidden<<endl;

	logfile.close();

	desired_error = (const float) 0.01;

	printf("Creating network.\n");
	ann = fann_create_standard(num_layers, num_input, num_neurons_hidden, num_output);
//	ann = fann_create_standard(num_layers, num_input, num_output);

//	fann_set_activation_steepness_hidden(ann, 1);
	fann_set_activation_steepness_output(ann, 1);

	fann_set_activation_function_hidden(ann, FANN_SIGMOID);
//	fann_set_activation_function_output(ann, FANN_LINEAR);

	fann_set_train_stop_function(ann, FANN_STOPFUNC_MSE);

	fann_randomize_weights(ann, 0,0.2);


	}


mappingLearnerFANNIndirect::~mappingLearnerFANNIndirect()
{
fann_destroy(ann);
}

double mappingLearnerFANNIndirect::train(vector<vector<double> >& inputs, vector<vector<double> >& outputs)
	{

	return 0;
	

	}



double mappingLearnerFANNIndirect::run(vector<double>& input, vector<double>& output)
{

if (ann)
{

	optimizer quickoptimizer(dim_parameters,-1); //we use our own quickoptimizer to optimize the mapping, everything is implemented to handle the parameters anyway

	vector<double> parameters(dim_parameters,0);
	double bestfitness=invalidresult;
	output.resize(dim_parameters);
	
	fann_type* finput = new fann_type[num_input];

//	cout<<"mappingLearnerFANNIndirect::run"<<endl;
//	cout<<" input: "<<input[0]<<" "<<input[1]<<endl;


	for(int i=0;i<indirectlearneroptimizelength;++i) //optimize the mapping
//	for(int i=0;i<100;++i) //optimize the mapping
		{
		quickoptimizer.getHint(parameters);

//		cout<<"parameters: "<<parameters[0]<<" "<<parameters[1]<<endl;
		
		unsigned int isize=input.size();

		for(unsigned int j=0;j<isize;++j)  //put input and output to the combined input
				finput[j]=(fann_type)input[j];

		for(unsigned int j=0;j<dim_parameters;++j)
				finput[isize+j]=(fann_type)parameters[j];

/*		cout<<"concatenated input: ";
		for(unsigned int j=0;j<num_input;++j)
			cout<<finput[j]<<" ";
		cout<<endl;

*/	

		fann_type* foutput=fann_run(ann,finput);


		double fitness=(double)(*foutput);

//		cout<<"fitness: "<<fitness<<endl;

		quickoptimizer.fitnessOfLastHint(fitness);
		if (fitness<bestfitness)
			{
			bestfitness=fitness;
			for(unsigned int n=0;n<dim_parameters;++n)
				output[n]=parameters[n]; //at return this will just return with the best parameter
			}
	

		}

			
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


return bestfitness;
}

return -1;


}

void mappingLearnerFANNIndirect::save(char *filename)
{

if(ann)
	fann_save(ann,filename);
}


bool mappingLearnerFANNIndirect::load(char *filename)
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
