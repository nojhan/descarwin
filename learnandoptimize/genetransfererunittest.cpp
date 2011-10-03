#include "globals.h"
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include "time.h"
#include "instanceHandler.h"
#include "mappingLearnerFANN.h"
#include "mappingLearnerSharkFFNET.h"
#include "genetransferer.h"


using namespace std;


vector<instanceHandler> instanceHandlers;

geneTransferer genetransferer;


void logtoiterationsfile(unsigned int iteration,char* typeofiterationphase, double annerror)
{

		double averageobjective=0;
		unsigned int s=instanceHandlers.size();

		for(unsigned int i=0;i<s;++i)
			averageobjective+=instanceHandlers[i].bestobjective;

		averageobjective/=s;

		cout<<" average objective: " <<averageobjective<<endl;


		stringstream logfilename;

		logfilename<<"iterations"<<".log";

		ofstream logfile;


		logfile.open(logfilename.str().c_str(),ios_base::app);

		logfile<<" iteration: "<<iteration<<" "<<typeofiterationphase<<" average objective: " <<averageobjective<<" annerror "<<annerror<<endl;
		
		logfile.close();
}



void readparamnamesfile()
{

ifstream paramnamesfile;

paramnamesfile.open("paramnames.txt");

if (!paramnamesfile.is_open())
	exit(-1);

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


/* this is not really gathering anything, just simulating*/



ofstream statefile;

cout<<"gatherInstanceResults"<<endl;


do
	{

	system("cp state.txt state_backup.txt");
	system("rm state.txt");

	numberofreadyinstance=0;

	for( unsigned int i=0;i<numinstances;++i)
		{
		

		//if(!instanceHandlers[i].ready)
			{
			checkexitandreadconfig(); //before doing something time consuming
			
			unsigned int r= rand() % 4 ;

			//cout<<r<<endl;

			if (r == 3) //new best value
				{
				genetransferer.instanceBestParameterChanged(i);
				instanceHandlers[i].bestparameters.clear();
				instanceHandlers[i].bestparameters.push_back(instanceHandlers[i].tryparameters);
				instanceHandlers[i].isthisdefaultparameters=false; //this is at least second improvement, cannot be the default parameters
				instanceHandlers[i].doesthiscontaindefaultparameters=false; //this is at least second improvement, cannot be the default parameters
				}
			if ( r== 2) //add best value
				{
				instanceHandlers[i].isthisdefaultparameters=false; 
				instanceHandlers[i].bestparameters.push_back(instanceHandlers[i].tryparameters);
				}


			numberofreadyinstance++;
			instanceHandlers[i].ready=1;
				
			}
		/*else
			numberofreadyinstance++;*/

		
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

	system("cp state.txt state_backup.txt");

cout<<"gatherInstanceResults end"<<endl;

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
//		instance->startJob(iterationsperparameter);
		instanceHandlers.push_back(*instance);
		unsigned int s=instanceHandlers.size()-1;
		
		if (loglevel>2)
			cout<<"merged"<<endl;
		instanceHandlers[s].print();
		++numinstances;
		break; //do not search more matches
		}
}//for


gatherInstanceResults();	



system("./cleanupafteriteration.sh > /dev/null");	

}




int main()
{




	mappingLearner* mappinglearner;

	checkexitandreadconfig();

	if(learningModelType==SharkFFNET)
		mappinglearner= dynamic_cast<mappingLearner*>(new mappingLearnerSharkFFNet(num_features,num_parameters)); 
	else
		mappinglearner= dynamic_cast<mappingLearner*>(new mappingLearnerFANN(num_features,num_parameters)); 

	
	stringstream command;

	command<<"hostname > iterations.log"<<endl;

	cout<<command;

	system(command.str().c_str());


	cout<<"Initialize instances"<<endl;

	readparamnamesfile();

	initialize();

	logtoiterationsfile(0,"initialize", 0);
	


	unsigned int s=instanceHandlers.size();

	if( !s )
		return(-1);

	genetransferer.init(s,&instanceHandlers);

	cout<< " testing distances "<<endl;

	unsigned int numberoffails=0;

	for (unsigned int k=0;k<s;++k)
			{
			if (distances[k][k]>0.0001) // numerical errors
				{
				cout<< k<< " " << k<< " fail"<<endl;
				++numberoffails;
				}
			else
				cout<< k<< " " << k<< " OK"<<endl;
			}

	for (unsigned int k=0;k<s;++k)
	for (unsigned int j=0;j<s;++j)
			{
			if (distances[k][j]<-0.0001) // numerical errors
				{
				cout<< k<< " " << j<< " fail"<<endl;
				++numberoffails;
				}
			else
				cout<< k<< " " << j<< " OK"<<endl;
			}

	

	for (unsigned int i=0;i<s;++i)
	for (unsigned int j=0;j<s;++j)
	for (unsigned int k=0;k<s;++k)
		if((i!=j) && (j!=k))
			{
			if (distances[i][j]+distances[j][k]+0.00001<distances[i][k]) // numerical errors
				{
				cout<< i<< " "<< j<< " " << k<< " fail ";
				cout<<distances[i][j]<<" "<<distances[j][k]<<" "<<distances[i][j]+distances[j][k]<<" "<<distances[i][k]<<endl;
				++numberoffails;
				}
			else
				cout<< i<< " "<< j<< " " << k<< " OK"<<endl;
			}

	cout<< " total errors in distances "<< numberoffails<<endl;

	

	for (unsigned iteration=0;iteration<numberofiterations;iteration++)
		{
		printf("Epoch %d.\n",iteration);


		printf("Use geneTransferer;\n");
		
		system("./cleanupafteriteration.sh > /dev/null");

		stringstream text3;

		text3<<"Epoch "<<iteration<<" geneTransferer;";

		whatislasttry=text3.str();
		
		s=instanceHandlers.size();

		for(unsigned int i=0;i<s;++i)
			{
			checkexitandreadconfig();
			
			unsigned int number=0;
			int source=genetransferer.getNextInstance(i,number);

			if (source >= 0) //fail to get is indicated by -1
				{
				

				vector<double>& parameters=instanceHandlers[source].bestparameters[number];
				
				instanceHandlers[i].addHint(parameters);	
				//instanceHandlers[i].startJob(iterationsperparameter);
				}
			//no else, if failed we can just do nothing.
			
			

			}


		gatherInstanceResults(); //this is prepared that not all jobs are started

		logtoiterationsfile(iteration,"genetransfer", 0);





		}
	
	stringstream command2;
	
	command2<<"hostname > iterations.log"<<endl;

	cout<<command2;

	system(command2.str().c_str());

	system("./cleanupafteriteration.sh > /dev/null");

	return 0;
}

