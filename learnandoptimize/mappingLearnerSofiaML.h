#ifndef mappingLearnerRankSVMIndirect_H
#define mappingLearnerRankSVMIndirect_H

#include "globals.h"
#include "optimizer.h"
#include "sofia-ml-methods.h"


#include <stdio.h>
#include <vector>
#include "mappingLearner.h"

using namespace std;
using namespace sofia_ml;



class mappingLearnerRankSVMIndirect: public mappingLearner
{

public:

mappingLearnerRankSVMIndirect(unsigned int numinput, unsigned int numoutput);
~mappingLearnerRankSVMIndirect();
double train(vector<vector<double> >& inputs, vector<vector<double> >& outputs);
virtual double train(vector<vector<double> >& inputs, vector<vector<double> >& outputs, vector<double>& fitnesses, vector<int>& instances);
double run(vector<double>& input, vector<double>& output);
void save(char *filename);
bool load(char *filename);
bool areYouDirectMapping();

double annerror;

private:

SfWeightVector* sfWeightVector;
SfDataSet training_data;

unsigned int num_input;
unsigned int num_output;
unsigned int dim_parameters;

};

bool mappingLearnerRankSVMIndirect::areYouDirectMapping()
{
return false;
};

double mappingLearnerRankSVMIndirect::train(vector<vector<double> >& inputs, vector<vector<double> >& outputs, vector<double>& fitnesses, vector<int>& instances)
{
float error=0;

if(sfWeightVector)
	{
	
	//add data to previous

	//cout<<inputs.size()<<" "<<outputs.size()<<" "<<fitnesses.size()<<endl;

	if(loglevel>4)
		cout<<"adding new data to the pool to learn"<<endl;


	stringstream datafilename;

	datafilename<<"learner_data"<<".txt";

	ofstream datafile;

	datafile.open(datafilename.str().c_str());

	cout<<"inputs.size() "<<inputs.size()<<endl;


	for (unsigned int i=0;i<inputs.size();++i)
		{

		stringstream datarow;
	
		datarow<<fitnesses[i];

		datarow<<" qid:"<<instances[i];

		unsigned int isize=inputs[i].size();

		//cout<<osize<<endl;

		for(unsigned int j=0;j<isize;++j)
			datarow<<" "<<j+1<<":"<<inputs[i][j];

		
		unsigned int osize=outputs[i].size();

		//cout<<osize<<endl;

		for(unsigned int j=0;j<osize;++j)
			datarow<<" "<<isize+j+1<<":"<<outputs[i][j];

		//cout<<combinedinput.size()<<endl;
		
		//cout<<datarow.str().c_str()<<endl;
		
		datarow<<endl;
		
		datafile<<datarow.str().c_str();

		training_data.AddVector(datarow.str());

		}

	
	datafile.close();
	
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


	
	
	
	cout<<"StochasticClassificationAndRankLoop learner "<<ROMMA<<"eta "<<PEGASOS_ETA<< "lambda "<<0.1<<"c "<<0<< "rank step prob"<<0.5<<"iterations "<< numepochsperiteration<<endl;




	

	//StochasticClassificationAndRankLoop(training_data, learner_type, eta_type, lambda, c, rank_step_probability, iterations, w);
	StochasticClassificationAndRankLoop(training_data, ROMMA, PEGASOS_ETA, 0.1, 0, 0.5, numepochsperiteration, sfWeightVector); 
//	StochasticClassificationAndRankLoop(training_data, SGD_SVM, PEGASOS_ETA, 0.1, 1, 0.5, 1000000, sfWeightVector); 

//	StochasticRankLoop(training_data,learner_type, eta_type, lambda, c, iterations,w);

//	StochasticRankLoop(training_data,ROMMA, PEGASOS_ETA, 0.1, 1, 100000,sfWeightVector); 

//StochasticOuterLoop(training_data, ROMMA, PEGASOS_ETA, 0.1, 1, 100000, sfWeightVector); 



  vector<float> predictions;

    SvmPredictionsOnTestSet(training_data, *sfWeightVector, &predictions);

	error=0;

	cout<<training_data.NumExamples()<<endl;

	vector<vector<int> > outcomes(3,vector<int>(3,0));

	cout<<"table of outcomes"<<endl;

	
    for (int i = 0; i < training_data.NumExamples(); ++i) 
    for (int j = 0; j < i; ++j) 
		{
			if (instances[i]==instances[j])
				{
				int o1,o2;
				//cout<<predictions[j]<<" ";
				if(predictions[i]>predictions[j])
					o1=0;
				if(predictions[i]<predictions[j])
					o1=2;

				if(predictions[i]==predictions[j])
					o1=1;

				if( training_data.VectorAt(i).GetY()>training_data.VectorAt(j).GetY())
					o2=0;
				if( training_data.VectorAt(i).GetY()<training_data.VectorAt(j).GetY())
					o2=2;

				if( training_data.VectorAt(i).GetY()==training_data.VectorAt(j).GetY())
					o2=1;
			
				//cout<<o1<<" "<<o2<<endl;
				++outcomes[o1][o2];
				}

    		}
		
		cout<<endl;

	    for (int i = 0; i < 3; ++i) 
		{
	    	for (int j = 0; j < 3; ++j) 
			cout<<outcomes[i][j]<<" ";
		cout<<endl;
		}




	}

	return error;


}


mappingLearnerRankSVMIndirect::mappingLearnerRankSVMIndirect(unsigned int numinput, unsigned int numoutput):num_input(numinput+numoutput),num_output(1),dim_parameters(numoutput),training_data(false)

	{

	stringstream logfilename;
	
	m_inputs=new vector<vector<double> >;
	m_outputs=new vector<vector<double> >;

	sfWeightVector=new SfWeightVector(num_input+1);
	

	logfilename<<"learner_log"<<".txt";

	ofstream logfile;

	logfile.open(logfilename.str().c_str());

	logfile<<"RankSVM learner "<<endl;
	logfile<<"dimensions input "<<num_input<<" output "<<num_output<<endl;

	logfile.close();


	printf("Creating RankSVM.\n");


	}


mappingLearnerRankSVMIndirect::~mappingLearnerRankSVMIndirect()
{
delete sfWeightVector;
}

double mappingLearnerRankSVMIndirect::train(vector<vector<double> >& inputs, vector<vector<double> >& outputs)
	{

	return 0;
	

	}



double mappingLearnerRankSVMIndirect::run(vector<double>& input, vector<double>& output)
{

if (sfWeightVector)
{

stringstream datarowinput;


unsigned int isize=input.size();

for(unsigned int j=0;j<isize;++j)  //put input and output to the combined input
			datarowinput<<" "<<j+1<<":"<<input[j];

//this is constant, we can prepare this

/*
cout<<"test linearity"<<endl;

for (int i=0;i<100;++i)
	{

	stringstream datarow;

	double x=i*0.01;

	datarow<<"1 qid:1";

	datarow<<datarowinput.str().c_str();
	
	//cout<<"create"<<endl;
	datarow<<" 3:"<<x<<" 4:0.5";
	cout<< datarow.str().c_str();
	SfDataSet test_data(false);
	//cout<<"add"<<endl;
	test_data.AddVector(datarow.str());
	vector<float> predictions;
	//cout<<"prediction"<<endl;
	SvmPredictionsOnTestSet(test_data, *sfWeightVector, &predictions);
	double fitness=predictions[0];

	cout<<" "<<fitness<<endl;

	
	}*/




	optimizer quickoptimizer(dim_parameters,-1); //we use our own quickoptimizer to optimize the mapping, everything is implemented to handle the parameters anyway

	vector<double> parameters(dim_parameters,0);
	double bestfitness=invalidresult;
	output.resize(dim_parameters);
	

//	cout<<"mappingLearnerRankSVMIndirect::run"<<endl;
//	cout<<" input: "<<input[0]<<" "<<input[1]<<endl;


	for(int i=0;i<indirectlearneroptimizelength;++i) //optimize the mapping
//	for(int i=0;i<100;++i) //optimize the mapping
		{
		quickoptimizer.getHint(parameters);

//		cout<<"parameters: "<<parameters[0]<<" "<<parameters[1]<<endl;
		
		stringstream datarow;

		datarow<<"1 qid:1";

		datarow<<datarowinput.str().c_str();

		for(unsigned int j=0;j<dim_parameters;++j)
				datarow<<" "<<isize+j+1<<":"<< parameters[j];


/*		cout<<"concatenated input: ";
			cout<<datarow.str().c_str();
		cout<<endl;
*/
		
		SfDataSet test_data(false);
		test_data.AddVector(datarow.str());

		vector<float> predictions;

		SvmPredictionsOnTestSet(test_data, *sfWeightVector, &predictions);
		
	


		double fitness=predictions[0];

//		cout<<"fitness: "<<fitness<<endl;

		quickoptimizer.fitnessOfLastHint(fitness);
		if (fitness<bestfitness)
			{
			bestfitness=fitness;
			for(unsigned int n=0;n<dim_parameters;++n)
				output[n]=parameters[n]; //at return this will just return with the best parameter
			}
	

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


return bestfitness;
}

return -1;


}

void mappingLearnerRankSVMIndirect::save(char *filename)
{

if(sfWeightVector)
	{

	std::fstream model_stream;
	  model_stream.open(filename, std::fstream::out);
	  if (!model_stream) 
		{
		std::cerr << "Error opening model output file " << filename << std::endl;
	    	exit(1);
  		}
	  std::cerr << "Writing model to: " << filename << std::endl;
	  model_stream << sfWeightVector->AsString() << std::endl;
	  model_stream.close();
	  std::cerr << "   Done." << std::endl;
	}

}


bool mappingLearnerRankSVMIndirect::load(char *filename)
{
string fn(filename);

  if (sfWeightVector != NULL) {
    delete sfWeightVector;
  }

  std::fstream model_stream;
  model_stream.open(filename, std::fstream::in);
  if (!model_stream) {
    std::cerr << "Error opening model input file " << filename << std::endl;
    exit(1);
  }

  std::cerr << "Reading model from: " << filename << std::endl;
  string model_string;
  std::getline(model_stream, model_string);
  model_stream.close();
  std::cerr << "   Done." << std::endl;
  
  sfWeightVector = new SfWeightVector(model_string);

if (sfWeightVector)
	return true;
return false;


}

#endif

