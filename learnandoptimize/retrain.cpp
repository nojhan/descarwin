#include "globals.h"
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include "time.h"
#include "instanceHandler.h"
#include "genetransferer.h"
#include "mappingLearnerFANN.h"
#include "mappingLearnerSharkFFNET.h"
#include "mappingLearnerFANNIndirect.h"
#include "mappingLearnerRankSVMIndirect.h"

using namespace std;


vector<instanceHandler> instanceHandlers;



const int oversamplingrate=5;


void normalize()
{

cout<<"normalize"<<endl;
unsigned int s=instanceHandlers.size();

double bestobjective=instanceHandlers[0].bestobjective;
unsigned int startposition=0;
unsigned int instancenumber=instanceHandlers[0].instancenumber;

for(unsigned int i=1;i<s;++i)
		{
		unsigned int newinstancenumber=instanceHandlers[i].instancenumber;
		if (instancenumber!=newinstancenumber)
			{
			cout<<"instance "<< instancenumber<<endl;
			for(unsigned int j=startposition;j<i;++j)
				{
				instanceHandlers[j].bestobjective=instanceHandlers[j].bestobjective/bestobjective-1;
				cout<<instanceHandlers[j].bestobjective<<endl;
				}
			startposition=i;
			instancenumber=newinstancenumber;
			bestobjective=instanceHandlers[i].bestobjective;
			}
		else
			{
			double o=instanceHandlers[i].bestobjective;
			if (bestobjective>o)
				bestobjective=o;
			}

		}	//for i

for(unsigned int j=startposition;j<s;++j) //last instance
				instanceHandlers[j].bestobjective=instanceHandlers[j].bestobjective/bestobjective-1;

}

void initialize(char* inputfile)
{


	ifstream instancesfile;


	int numinstances=0;
	
	

  	instancesfile.open (inputfile);

	if (instancesfile.is_open())
		{
		if (loglevel>2)
			cout<<inputfile<<endl;
		while (!instancesfile.eof())
			{	

					instanceHandler newinstance(0);
					newinstance.instancenumber=-1;
					newinstance.readfromStream(instancesfile);
					
			
					if ((newinstance.directory.size()) && (newinstance.instancenumber>0))
						{
						cout<<newinstance.instancenumber<<" add new"<<endl;
						instanceHandlers.push_back(newinstance);
							
							
						}
					
					
			} //while
		} //if
	else
	{
	cerr<<"Could not open "<<inputfile<<endl;
	exit(-1);
	}

}



void initializewithmerge(char* inputfile)
{


	ifstream instancesfile;


	int numinstances=0;
	

	

  	instancesfile.open (inputfile);

	if (instancesfile.is_open())
		{
		if (loglevel>2)
			cout<<inputfile<<endl;
		while (!instancesfile.eof())
			{	

					instanceHandler newinstance(0);
					newinstance.instancenumber=-1;
					newinstance.readfromStream(instancesfile);
					
					instanceHandler* lastinstance=NULL;
					unsigned int s=instanceHandlers.size();
					if(s)
						lastinstance=&(instanceHandlers[instanceHandlers.size()-1]);


			
					if ((newinstance.directory.size()) && (newinstance.instancenumber>0))
						{
						if((lastinstance)&&(lastinstance->instancenumber==newinstance.instancenumber)) //merge
							{
							cout<<lastinstance->instancenumber<<" "<<newinstance.instancenumber<<" merge"<<endl;
							lastinstance->bestparameters.push_back(newinstance.bestparameters[0]);
							}
						
						else
							{
							if (lastinstance)
								cout<<lastinstance->instancenumber<<" "<<newinstance.instancenumber<<" add new"<<endl;
							instanceHandlers.push_back(newinstance);
							
							}
						}

						
					
			} //while
		} //if
	else
	{
	cerr<<"Could not open "<<inputfile<<endl;
	exit(-1);
	}

}




void simplesample(vector<vector<double> >& inputs , vector<vector<double> > & outputs, vector<double> & fitnesses, vector<int>&  instances)
	{
		unsigned int s=instanceHandlers.size();

		

		for(unsigned int i=0;i<s;++i)
		{
		for(unsigned int j=0;j<instanceHandlers[i].bestparameters.size();++j) //we add all the best parameters, if there are several ones, because 			that is better for the ANN
			{
			inputs.push_back((vector<double>) instanceHandlers[i].features);
			outputs.push_back((vector<double>) instanceHandlers[i].bestparameters[j]);
			fitnesses.push_back(instanceHandlers[i].bestobjective);
			instances.push_back(instanceHandlers[i].id);
			}
		
		}	

	}

void oversample(vector<vector<double> > & inputs, vector<vector<double> > & outputs)
	{
	unsigned int s=instanceHandlers.size();

	unsigned int maxnumbestpar=0;
	for(unsigned int i=0;i<s;++i)
		if(instanceHandlers[i].bestparameters.size()>maxnumbestpar)
			maxnumbestpar=instanceHandlers[i].bestparameters.size();
	

	unsigned int oversamplesize=maxnumbestpar*oversamplingrate;

	cout<<"maxnumbestpar "<<maxnumbestpar<<" "<<"oversamplesize "<<oversamplesize<<endl;


		for(unsigned int i=0;i<s;++i)
		{
		unsigned int numsamples=0;	

		for(;;)
		{
			for(unsigned int j=0;j<instanceHandlers[i].bestparameters.size();++j) //we add all the best parameters, if there are several ones, because 			that is better for the ANN
				{
				inputs.push_back((vector<double>) instanceHandlers[i].features);
				outputs.push_back((vector<double>) instanceHandlers[i].bestparameters[j]);
				numsamples++;
				if (numsamples == oversamplesize)
					break;
				}
			if (numsamples >= oversamplesize)
				break;
		}

		}	



	}






int main ( int argc, char* argv[] )
{

	char savefilename[1000];

if (argc<2)
    {
    cout<<"Usage: "<<argv[0]<<" <input file> [number of epochs, default is 1000] [output file, default is AN.retrained] "<<endl;
    exit(-1);
    }



	mappingLearner* mappinglearner;

	checkexitandreadconfig();

if(learningModelType==SharkFFNET)
		mappinglearner= dynamic_cast<mappingLearner*>(new mappingLearnerSharkFFNet(num_features,num_parameters)); 
	else if (learningModelType==FANNIndirect)
		mappinglearner= dynamic_cast<mappingLearner*>(new mappingLearnerFANNIndirect(num_features,num_parameters));

	else if (learningModelType==RankSVMIndirect)
		mappinglearner= dynamic_cast<mappingLearner*>(new mappingLearnerRankSVMIndirect(num_features,num_parameters));
 
	else  //default FANN
		mappinglearner= dynamic_cast<mappingLearner*>(new mappingLearnerFANN(num_features,num_parameters)); 


	
	if (argc>2)
		{
		numepochsperiteration=atoi(argv[2]);
		cout<<"numepochsperiteration changed to "<<numepochsperiteration<<endl;
		}


	stringstream command;



		cout<<"Initialize instances"<<endl;

	if (mappinglearner->areYouDirectMapping()) //train directmapping learner
		{
		cout<<"Direct"<<endl;
		initializewithmerge(argv[1]);
		}	
	else
		{
		cout<<"Indirect"<<endl;
		initialize(argv[1]);
		if(normalizeindirect)		
			normalize();
		}


		cout<<"Training network."<<endl;


		vector<vector<double> > inputs;		
		vector<vector<double> > outputs;
		vector<double>  fitnesses;
		vector<int>  instances;


//		oversample(inputs , outputs,, fitnesses);
		simplesample(inputs , outputs, fitnesses, instances);

	double annerror=-1;

	if (mappinglearner->areYouDirectMapping()) //train directmapping learner
		{
		cout<<"Direct"<<endl;
		annerror=mappinglearner->train(inputs,outputs);
		}	
	else
		{
		cout<<"Indirect"<<endl;
		annerror=mappinglearner->train(inputs,outputs,fitnesses,instances);
		}


cout<<"Saving network."<<endl;


		if (argc>3)
			{
			mappinglearner->save(argv[3]);		
			}
		else
			mappinglearner->save(const_cast<char*>(modelfile.c_str()));		

		

	return 0;
}

