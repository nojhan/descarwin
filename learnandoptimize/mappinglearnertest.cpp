#include <stdio.h>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include "time.h"
#include "instanceHandler.h"
#include "mappingLearner.h"
#include "globals.h"

using namespace std;

bool testdefaultparameters=false;




vector<instanceHandler> instanceHandlers;



void readparamnamesfile()
{

ifstream paramnamesfile;

paramnamesfile.open("paramnames.txt");

for (unsigned int i=0;i<num_parameters;++i)
	{
	string featurename;
	paramnamesfile>>featurename;
	paramnames.push_back(featurename);
	cout<<featurename<<" ";

	double minvalue;
	paramnamesfile>>minvalue;
	minvalues.push_back(minvalue);
	cout<<minvalue<<" ";

	double maxvalue;
	paramnamesfile>>maxvalue;
	maxvalues.push_back(maxvalue);
	cout<<maxvalue<<" ";

	double defaultvalue;
	
	paramnamesfile>>defaultvalue;

	cout<<defaultvalue<<" ";


	defaultvalue=(defaultvalue-minvalue)/(maxvalue-minvalue); //normalize back

	cout<<defaultvalue<<endl;

	defaultvalues.push_back(defaultvalue);

	
	char temp[10000];
	paramnamesfile.getline(temp,10000);

	}

paramnamesfile.close();

}


void gatherInstanceResults()
{

unsigned int numberofreadyinstance=0;
unsigned int numinstances=instanceHandlers.size();


/* we start when the file is taken away seems better
for( unsigned int i=0;i<numinstances;++i)
	time(&(instanceHandlers[i].start));*/

ofstream statefile;


do
	{

	system("cp state.txt state_backup.txt");
	system("rm state.txt");

	numberofreadyinstance=0;

	for( unsigned int i=0;i<numinstances;++i)
		{
		

		if(!instanceHandlers[i].ready)
			{
			checkexitandreadconfig(); //before doing something time consuming
			instanceHandlers[i].readResult();
			if (instanceHandlers[i].ready)
				numberofreadyinstance++;
				
			}
		else
			numberofreadyinstance++;

		
		statefile.open("state.txt",ios_base::app);

		statefile<<instanceHandlers[i].numreadyiterations<<" ";
		
		statefile<<i;
		
		for(unsigned int j=0;j<iterationsperparameter;++j)
				statefile<<" "<<instanceHandlers[i].readyiterations[j];

		statefile<<endl;
		
		statefile.close();
			
		

		}

		statefile.open("state.txt",ios_base::app);
		statefile<<"number of ready instances "<<numberofreadyinstance<<endl;
		statefile.close();

		checkexitandreadconfig(); //just in case we would stuck in the loop. it should not happen



	}
	while(numberofreadyinstance<numinstances);
}





void initialize()
{

vector<instanceHandler> instancehandlersfromrunningtimesfile;
vector<instanceHandler> instancehandlersfrominstancesfile;

system("./cleanupafteriteration.sh > /dev/null");



	ifstream instancesfile,runningtimesfile;

  	runningtimesfile.open ("runningtimes.txt");

	int numinstances=0;

	//read in runningtimes


	if (runningtimesfile.is_open())
		{
		if (loglevel>2)
			cout<<"reading runningtimes.txt"<<endl;
		while (!runningtimesfile.eof())
		    {	
			checkexitandreadconfig();			
			string directory;
			runningtimesfile>>directory;
			if (!directory.size())
				break;
			cout<<directory<<" ";
			int instancenumber=-1;
			runningtimesfile>>instancenumber;
			cout<<instancenumber<<" ";
			int runningtime;
			int numberofgenerations;
			int iterationnumber;
			runningtimesfile>>iterationnumber; // this is iterationnumber
			runningtimesfile>>numberofgenerations; // this is number of generations
			runningtimesfile>>runningtime; //this is number of evaluations
			cout<<runningtime<<endl;
			char temp[10000];
			runningtimesfile.getline(temp,10000);
			
			if ((directory.size()) && (instancenumber>-1))
				{
				instanceHandler newinstance(-1);
				newinstance.directory=directory;
				newinstance.instancenumber=instancenumber;
				newinstance.runningtime=runningtime;
				instancehandlersfromrunningtimesfile.push_back(newinstance);	
				}

			} //while

		} //if
		else
		{
		cerr<<"Could not open runningitems.txt"<<endl;
		exit(-1);
		}


	runningtimesfile.close();	

  	instancesfile.open ("instances.txt");

	if (instancesfile.is_open())
		{
		if (loglevel>2)
			cout<<"reading instances.txt"<<endl;
		while (!instancesfile.eof())
			{	

					instanceHandler newinstance(0);
					newinstance.instancenumber=-1;
					instancesfile>>newinstance.directory;
					cout<<newinstance.directory<<" ";
					for(unsigned int i=0;i<num_features;++i)
						{
						instancesfile>>newinstance.features[i];
						cout<<newinstance.features[i]<<" ";
						}
					int instancenumber=-1;
					instancesfile>>instancenumber;
					cout<<instancenumber;
					cout<<endl;
					if ((newinstance.directory.size()) && (instancenumber>-1))
						{
						newinstance.instancenumber=instancenumber;
						instancehandlersfrominstancesfile.push_back(newinstance);
						}
						
					
			} //while
		} //if
	else
	{
	cerr<<"Could not open instances.txt"<<endl;
	exit(-1);
	}

cout<< instancehandlersfromrunningtimesfile.size()<<" "<<instancehandlersfrominstancesfile.size()<<endl;

instancesfile.close();

for(unsigned int i=0;i<instancehandlersfromrunningtimesfile.size();++i)
for(unsigned int j=0;j<instancehandlersfrominstancesfile.size();++j)
{
	if (loglevel>3)
		cout<< i <<" "<< j<<endl;
	instanceHandler* instancerunningtime=&(instancehandlersfromrunningtimesfile[i]);
	instanceHandler* instance=&(instancehandlersfrominstancesfile[j]);

	if (loglevel>3)
		cout<<"try to merge "<<instancerunningtime->directory<<" "<<instance->directory<<" "<<instancerunningtime->instancenumber<<" "<<instance->instancenumber<<endl;


	if((instancerunningtime->directory==instance->directory)&&(instancerunningtime->instancenumber==instance->instancenumber))
		{
		instance->id=numinstances;
		instance->runningtime=instancerunningtime->runningtime;
		instance->init();
		instance->tryparameters=defaultvalues;
		instance->bestobjective=-1; //invalid
		whatislasttry="initialize_with_default";
		if(testdefaultparameters)
			instance->startJob(iterationsperparameter);
		instanceHandlers.push_back(*instance);
		unsigned int s=instanceHandlers.size()-1;
		
		if (loglevel>2)
			cout<<"merged"<<endl;
		instanceHandlers[s].print();
		++numinstances;
		break; //do not search more matches
		}
}//for

if(testdefaultparameters)
	gatherInstanceResults();	



system("./cleanupafteriteration.sh > /dev/null");	

}







int main ( int argc, char* argv[] )
{

resubmittime=100*60; //we can wait longer here, since there is no iteration

if (argc>1)
	testdefaultparameters=atoi(argv[1]);

cout<<"testdefaultparameters="<<testdefaultparameters<<endl;




	
	cout<<"Load ANN"<<endl;

	checkexitandreadconfig();			

	double averageobjective=0;

	mappingLearner mappinglearner(num_features,num_parameters);

	mappinglearner.load("ANN.saved");
	
	
	stringstream command;

	command<<"hostname > iterations.log"<<endl;

	cout<<command.str().c_str();

	system(command.str().c_str());


	cout<<"Initialize instances"<<endl;

	readparamnamesfile();

	initialize();

	unsigned int s=instanceHandlers.size();

	if( !s )
		return(-1);


	for(unsigned int i=0;i<s;++i)
			averageobjective+=instanceHandlers[i].bestobjective;

		averageobjective/=s;

		cout<<" default average objective: " <<averageobjective<<endl;



		stringstream logfilename;

		logfilename<<"iterations"<<".log";

		ofstream logfile;


		logfile.open(logfilename.str().c_str(),ios_base::app);

		logfile<<"test default average objective: " <<averageobjective<<endl;
		
		logfile.close();


	

		printf("Use ANN\n");
		
		averageobjective=0;

		system("./cleanupafteriteration.sh > /dev/null");

		stringstream text2;

		text2<<"Epoch "<<" test_of_ANN";

		whatislasttry=text2.str();
		
		s=instanceHandlers.size();

		for(unsigned int i=0;i<s;++i)
			{
			checkexitandreadconfig();


			vector<double> parameters;

			mappinglearner.run(instanceHandlers[i].features, parameters);

			
			instanceHandlers[i].addHint(parameters);	
			
			
			instanceHandlers[i].startJob(iterationsperparameter);
			}


		gatherInstanceResults();

		for(unsigned int i=0;i<s;++i)
			averageobjective+=instanceHandlers[i].bestobjective;

		averageobjective/=s;

		cout<<" ANN average objective: " <<averageobjective<<endl;





		logfile.open(logfilename.str().c_str(),ios_base::app);

		logfile<<"test ANN average objective: " <<averageobjective<<endl;
		
		logfile.close();

	system("./cleanupafteriteration.sh > /dev/null");


	return 0;
}

