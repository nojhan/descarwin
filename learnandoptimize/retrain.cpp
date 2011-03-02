#include "globals.h"
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include "time.h"
#include "instanceHandler.h"
#include "mappingLearner.h"
#include "genetransferer.h"


using namespace std;


vector<instanceHandler> instanceHandlers;









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
					newinstance.instancenumber=0;
					newinstance.readfromStream(instancesfile);

			
					if ((newinstance.directory.size()) && (newinstance.instancenumber>0))
						instanceHandlers.push_back(newinstance);

						
					
			} //while
		} //if
	else
	{
	cerr<<"Could not open instances.txt"<<endl;
	exit(-1);
	}

}




int main ( int argc, char* argv[] )
{

	char savefilename[1000];



	

	if (argc>1)
		{
		numepochsperiteration=atoi(argv[1]);
		cout<<"numepochsperiteration changed to "<<numepochsperiteration<<endl;
		}


	checkexitandreadconfig(); //this is for config here

	mappingLearner mappinglearner(num_features,num_parameters);

	
	stringstream command;


	cout<<"Initialize instances"<<endl;



	initialize();

	unsigned int s=instanceHandlers.size();




		cout<<"Training network."<<endl;



		vector<vector<double> > inputs;		
		vector<vector<double> > outputs;

		for(unsigned int i=0;i<s;++i)
		{
		for(unsigned int j=0;j<instanceHandlers[i].bestparameters.size();++j) //we add all the best parameters, if there are several ones, because that is better for the ANN
			{
			inputs.push_back((vector<double>) instanceHandlers[i].features);
			outputs.push_back((vector<double>) instanceHandlers[i].bestparameters[j]);
			}
		}	



		double annerror=mappinglearner.train(inputs,outputs);

		if (argc>2)
			{
			mappinglearner.save(argv[2]);		
			}
		else
			mappinglearner.save("ANN.retrained");		

		

	return 0;
}

