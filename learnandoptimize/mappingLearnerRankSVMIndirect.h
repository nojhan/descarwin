#ifndef mappingLearnerRankSVMIndirect_H
#define mappingLearnerRankSVMIndirect_H

#include "globals.h"
#include "optimizer.h"


#include <stdio.h>
#include <vector>
#include "mappingLearner.h"

extern "C" {
#include "svm_light/svm_common.h"
#include "svm_struct_api.h"
#include "svm_struct/svm_struct_common.h"
}

using namespace std;
	



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

STRUCTMODEL model; 
STRUCT_LEARN_PARM sparm;

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

if(1)
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

		

		}

	
	datafile.close();
	

system("./svm_rank_learn -c 1 -t 2 -g 0.1 learner_data.txt SVMRank.model");

model=read_struct_model("SVMRank.model",&sparm);

cout<<"Classify"<<endl;

system("./svm_rank_classify learner_data.txt SVMRank.model learner_predictions.txt ");


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




	}

	return error;


}


mappingLearnerRankSVMIndirect::mappingLearnerRankSVMIndirect(unsigned int numinput, unsigned int numoutput):num_input(numinput+numoutput),num_output(1),dim_parameters(numoutput)//,training_data(false)

	{

	stringstream logfilename;
	
	m_inputs=new vector<vector<double> >;
	m_outputs=new vector<vector<double> >;
	

	logfilename<<"learner_log"<<".txt";

	ofstream logfile;

	logfile.open(logfilename.str().c_str());

	logfile<<"RankSVM learner "<<endl;
	logfile<<"dimensions input "<<num_input<<" output "<<num_output<<endl;

	logfile.close();


	printf("Creating RankSVM.\n");

	sparm.custom_argc=0;

	cout<<"svm_struct_classify_api_init"<<endl;
	svm_struct_classify_api_init(0,NULL);
	cout<<"parse_struct_parameters_classify"<<endl;
	parse_struct_parameters_classify(&sparm);



	}


mappingLearnerRankSVMIndirect::~mappingLearnerRankSVMIndirect()
{
//delete ;
}

double mappingLearnerRankSVMIndirect::train(vector<vector<double> >& inputs, vector<vector<double> >& outputs)
	{

	return 0;
	

	}


double mappingLearnerRankSVMIndirect::run(vector<double>& input, vector<double>& output)
	{


	if (1)
	{

	stringstream datarowinput;


	unsigned int isize=input.size();

	for(unsigned int j=0;j<isize;++j)  //put input and output to the combined input
				datarowinput<<" "<<j+1<<":"<<input[j];

	//this is constant, we can prepare this




		optimizer quickoptimizer(dim_parameters,-1); //we use our own quickoptimizer to optimize the mapping, everything is implemented to handle the parameters anyway

		vector<double> parameters(dim_parameters,0);
		double bestfitness=invalidresult;
		output.resize(dim_parameters);
	

	//	cout<<"mappingLearnerRankSVMIndirect::run"<<endl;
	//	cout<<" input: "<<input[0]<<" "<<input[1]<<endl;

	cout<<"indirectlearneroptimizelength "<<indirectlearneroptimizelength<<endl;

		for(int i=0;i<indirectlearneroptimizelength;++i) //optimize the mapping
			{
			quickoptimizer.getHint(parameters);

			//		cout<<"parameters: "<<parameters[0]<<" "<<parameters[1]<<endl;


		
			stringstream datarow;

			datarow<<"1 qid:1";

			datarow<<datarowinput.str().c_str();

			for(unsigned int j=0;j<dim_parameters;++j)
					datarow<<" "<<isize+j+1<<":"<< parameters[j];

			datarow<<endl;

	/*
			cout<<"concatenated input: ";
				cout<<datarow.str().c_str();
			cout<<endl;
	*/
		


			PATTERN sample;
			LABEL label;
			WORD *words;
			char comment[1000];
			label.loss=1;
			label.totdoc=1; //we have one sample
			sample.totdoc=1; //we have one sample
			
			
			long max_words_doc=num_input,wpos,queryid=0,slackid=0,max_docs=1;
			double doc_label,costfactor;
			
			
			sample.doc = (DOC **)my_malloc(sizeof(DOC *)*max_docs);
			words = (WORD *)my_malloc(sizeof(WORD)*(max_words_doc+10));

			//cout<<"parse_document"<<endl;

			parse_document((char*)datarow.str().c_str(),words,&doc_label,&queryid,&slackid,&costfactor,&wpos,max_words_doc,(char**)&comment);

			//cout<<"create_example"<<endl;

			(sample.doc)[0] = create_example(0,queryid,slackid,costfactor,create_svector(words,comment,1.0));
			free(words);
			LABEL y;

			//cout<<"classify_struct_example"<<endl;

			y=classify_struct_example(sample, &model, &sparm);

			free(sample.doc);

			double fitness=y.classinfo[0];

			//cout<<"library call fitness: "<<fitness<<endl;
		




			quickoptimizer.fitnessOfLastHint(fitness);
			if (fitness<bestfitness)
				{
				bestfitness=fitness;
				for(unsigned int n=0;n<dim_parameters;++n)
					output[n]=parameters[n]; //at return this will just return with the best parameter
				}
	

			} //for i

			

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
	} //if (1)

	return -1;


	}

void mappingLearnerRankSVMIndirect::save(char *filename)
{

write_struct_model(filename,&model,&sparm);



/* this was the executable call implementation

stringstream command;
command<<"cp SVMRank.model "<<filename;

system(command.str().c_str());
*/

}


bool mappingLearnerRankSVMIndirect::load(char *filename)
{




/* this was the executable call implementation
stringstream command;
command<<"cp "<<filename<<" SVMRank.model"<<endl;

system(command.str().c_str());	*/

model=read_struct_model(filename,&sparm);

return true;


}

#ifdef RUNEXE //this was the implementation with calling the exe, very slow. 

double runEXE(vector<double>& input, vector<double>& output)
	{


	if (1)
	{

	stringstream datarowinput;


	unsigned int isize=input.size();

	for(unsigned int j=0;j<isize;++j)  //put input and output to the combined input
				datarowinput<<" "<<j+1<<":"<<input[j];

	//this is constant, we can prepare this




		optimizer quickoptimizer(dim_parameters,-1); //we use our own quickoptimizer to optimize the mapping, everything is implemented to handle the parameters anyway

		vector<double> parameters(dim_parameters,0);
		double bestfitness=invalidresult;
		output.resize(dim_parameters);
	

	//	cout<<"mappingLearnerRankSVMIndirect::run"<<endl;
	//	cout<<" input: "<<input[0]<<" "<<input[1]<<endl;

	cout<<"indirectlearneroptimizelength "<<indirectlearneroptimizelength<<endl;

		for(int i=0;i<indirectlearneroptimizelength;++i) //optimize the mapping
			{
			quickoptimizer.getHint(parameters);

			//		cout<<"parameters: "<<parameters[0]<<" "<<parameters[1]<<endl;

			stringstream datafilename;

			datafilename<<"learner_data"<<".txt";

			ofstream datafile;

			datafile.open(datafilename.str().c_str());


		
			stringstream datarow;

			datarow<<"1 qid:1";

			datarow<<datarowinput.str().c_str();

			for(unsigned int j=0;j<dim_parameters;++j)
					datarow<<" "<<isize+j+1<<":"<< parameters[j];

			datarow<<endl;

	/*
			cout<<"concatenated input: ";
				cout<<datarow.str().c_str();
			cout<<endl;
	*/
		

			datafile<<datarow.str().c_str();
		
			datafile.close();
		

			system("./svm_rank_classify learner_data.txt SVMRank.model learner_predictions.txt > learner_classify.log");

			stringstream predictionsfilename;

			predictionsfilename<<"learner_predictions.txt";
	
			ifstream predictionsfile;

			//cout<<"opening "<<predictionsfilename.str().c_str()<<endl;

			predictionsfile.open(predictionsfilename.str().c_str(),ios::in);

			if(!predictionsfile)
				cout<<"error opening learner_predictions.txt"<<endl;
	

			double fitness=invalidresult;

			bool s=predictionsfile>>fitness;

			if(!s)
				cout<<"error reading learner_predictions.txt"<<endl;


			predictionsfile.close();


	

			cout<<"executable call fitness: "<<fitness<<endl;

			PATTERN sample;
			LABEL label;
			WORD *words;
			char comment[1000];
			label.loss=1;
			label.totdoc=1; //we have one sample
			sample.totdoc=1; //we have one sample
			
			
			long max_words_doc=num_input,wpos,queryid=0,slackid=0,max_docs=1;
			double doc_label,costfactor;
			
			
			sample.doc = (DOC **)my_malloc(sizeof(DOC *)*max_docs);
			words = (WORD *)my_malloc(sizeof(WORD)*(max_words_doc+10));

			//cout<<"parse_document"<<endl;

			parse_document((char*)datarow.str().c_str(),words,&doc_label,&queryid,&slackid,&costfactor,&wpos,max_words_doc,(char**)&comment);

			//cout<<"create_example"<<endl;

			(sample.doc)[0] = create_example(0,queryid,slackid,costfactor,create_svector(words,comment,1.0));
			free(words);
			LABEL y;

			//cout<<"classify_struct_example"<<endl;

			y=classify_struct_example(sample, &model, &sparm);

			free(sample.doc);

			//cout<<"library call fitness: "<<y.classinfo[0]<<endl;


			quickoptimizer.fitnessOfLastHint(fitness);
			if (fitness<bestfitness)
				{
				bestfitness=fitness;
				for(unsigned int n=0;n<dim_parameters;++n)
					output[n]=parameters[n]; //at return this will just return with the best parameter
				}
	

			} //for i

			

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
	} //if (1)

	return -1;


	}

#endif




#endif

