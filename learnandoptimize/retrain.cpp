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

using namespace std;


vector<instanceHandler> instanceHandlers;

const int oversamplingrate=5;




void initialize()
{


	ifstream instancesfile;


	int numinstances=0;
	

	

  	instancesfile.open ("bestlogs.txt");

	if (instancesfile.is_open())
		{
		if (loglevel>2)
			cout<<"bestlogs.txt"<<endl;
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
	cerr<<"Could not open bestlogs.txt"<<endl;
	exit(-1);
	}

}




void simplesample(vector<vector<double> >& inputs , vector<vector<double> > & outputs)
	{
		unsigned int s=instanceHandlers.size();

		for(unsigned int i=0;i<s;++i)
		{
		for(unsigned int j=0;j<instanceHandlers[i].bestparameters.size();++j) //we add all the best parameters, if there are several ones, because 			that is better for the ANN
			{
			inputs.push_back((vector<double>) instanceHandlers[i].features);
			outputs.push_back((vector<double>) instanceHandlers[i].bestparameters[j]);
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






	mappingLearner* mappinglearner;

	checkexitandreadconfig();

	if(learningModelType==SharkFFNET)
		mappinglearner= dynamic_cast<mappingLearner*>(new mappingLearnerSharkFFNet(num_features,num_parameters)); 
	else
		mappinglearner= dynamic_cast<mappingLearner*>(new mappingLearnerFANN(num_features,num_parameters)); 

	
	if (argc>1)
		{
		numepochsperiteration=atoi(argv[1]);
		cout<<"numepochsperiteration changed to "<<numepochsperiteration<<endl;
		}


	stringstream command;


	cout<<"Initialize instances"<<endl;



	initialize();



			




		cout<<"Training network."<<endl;



		vector<vector<double> > inputs;		
		vector<vector<double> > outputs;

		oversample(inputs , outputs);




		double annerror=mappinglearner->train(inputs,outputs);

		if (argc>2)
			{
			mappinglearner->save(argv[2]);		
			}
		else
			mappinglearner->save("ANN.retrained");		

		

	return 0;
}

