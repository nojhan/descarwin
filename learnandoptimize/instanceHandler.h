#ifndef INSTANCEHANDLER_H
#define INSTANCEHANDLER_H


#include "globals.h"
#include <stdio.h>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>


#include "time.h"
#include "optimizer.h"

using namespace std;




double getMedian(vector<double>& input)
{
sort(input.begin(),input.end()); 
unsigned int middle=(unsigned int)((double)input.size()/2.0 - 0.5); //we assume that the size is even

return input[middle]; 

} 






class instanceHandler
{
public:

instanceHandler(unsigned int pid);

~instanceHandler(){};

void optimizeStep();
void addHint(vector<double>& parameters);
void startJob(int numberofiteration=iterationsperparameter);
int readResult(); // return -1=failed 0=not ready 1=ready 2=bestresult added 3=new best result
void init();

double getRealTryParameterValue(unsigned int i) {double value=minvalues[i]+tryparameters[i]*(maxvalues[i]-minvalues[i]); return value;}
double getRealBestParameterValue(unsigned int i) {double value=minvalues[i]+bestparameters[0][i]*(maxvalues[i]-minvalues[i]); return value;}

void print();

void logbest();
void loglasttry();
void logheader();
void readfromStream(ifstream& instancesfile);


unsigned int id;

unsigned int resubmitted;
unsigned int numreadyiterations;
unsigned int taken;
bool ready;
bool isthisdefaultparameters;
bool doesthiscontaindefaultparameters;

time_t start;


string basedirectory;


unsigned int instancenumber;
unsigned int runningtime;
string directory;
double bestobjective;

vector<vector<double> > bestparameters;

vector<double> tryparameters;

optimizer myoptimizer;


vector<double> features;
vector<unsigned int> readyiterations;
double agregatedresult;
vector<double> allresults;

bool isthisoptimizerhint;


};


void instanceHandler::readfromStream(ifstream& instancesfile)
{

	instancesfile>>id;
	cout<<id<<" ";
	instancesfile>>agregatedresult; 
	instancesfile>>runningtime; 
					
	instancesfile>>instancenumber;
	cout<<instancenumber<<" ";
			
	instancesfile>>directory;
	cout<<directory<<" ";
	allresults.resize(iterationsperparameter);

	for (unsigned int i=0; i<iterationsperparameter;++i)
			instancesfile>>allresults[i]; 

	features.resize(num_features);
						
	for(unsigned int i=0;i<num_features;++i)
		{
		instancesfile>>features[i];
		cout<<features[i]<<" ";
		}
	bestparameters.resize(1);
	bestparameters[0].resize(num_parameters);
	for(unsigned int i=0;i<num_parameters;++i) 
		{
		double p;
		instancesfile>>bestparameters[0][i];
		cout<<bestparameters[0][i]<<" ";
		}
	char temp[10000];
	instancesfile.getline(temp,10000); //trash :)
	cout<<endl;



}


void instanceHandler::optimizeStep()
{
myoptimizer.getHint(tryparameters);
isthisoptimizerhint=true;

}

void instanceHandler::logheader()
{
stringstream logfilename;

logfilename<<"log_"<<id<<".txt";

ofstream logfile;


logfile.open(logfilename.str().c_str());

logfile<<"id"<<" ";
logfile<<"agregatedmakespan"<<" ";
logfile<<"runningtime"<<" ";
logfile<<"instancenumber"<<" ";

logfile<<"directory"<<" ";



for(unsigned i=0;i<iterationsperparameter;++i) 
	logfile<<"makespan#"<<i<<" ";


for(unsigned i=0;i<num_features;++i) 
	logfile<<"features#"<<i<<" ";


for(unsigned int i=0;i<num_parameters;++i) 
	logfile<<paramnames[i]<<" ";

for(unsigned int i=0;i<num_parameters;++i) 
	logfile<<paramnames[i]<<" ";


logfile<<"source_of_parameters ";

logfile<<"sigma_of_CMA-ES";

logfile<<endl;

logfile.close();

}

void instanceHandler::loglasttry()
{

stringstream logfilename;

logfilename<<"log_"<<id<<".txt";

ofstream logfile;


logfile.open(logfilename.str().c_str(),ios_base::app);

logfile<<id<<" ";
logfile<<agregatedresult<<" ";
logfile<<runningtime<<" ";
logfile<<instancenumber<<" ";

logfile<<directory<<" ";



for(unsigned i=0;i<iterationsperparameter;++i) 
	logfile<<allresults[i]<<" ";


for(unsigned i=0;i<num_features;++i) 
	logfile<<features[i]<<" ";


for(unsigned int i=0;i<num_parameters;++i) 
	logfile<<(double)tryparameters[i]<<" ";

for(unsigned int i=0;i<num_parameters;++i) 
	logfile<<(double)getRealTryParameterValue(i)<<" ";




logfile<<whatislasttry<<" ";

logfile<<myoptimizer.getSigma()<<" ";

logfile<<endl;

logfile.close();

}




instanceHandler::instanceHandler(unsigned int pid): id(pid), tryparameters(num_parameters,0.5), myoptimizer(num_parameters,pid), features(num_features,0.0),isthisoptimizerhint(false)
{
bestobjective=-1;
bestparameters.push_back(tryparameters);
allresults=vector<double>(iterationsperparameter,0);
ready=true;
isthisdefaultparameters=true;
doesthiscontaindefaultparameters=true;

};



void instanceHandler::logbest()
{

stringstream logfilename;

logfilename<<"log_"<<id<<".txt";

ofstream logfile;


logfile.open(logfilename.str().c_str(),ios_base::app);

logfile<<id<<" ";
logfile<<bestobjective<<" ";
logfile<<runningtime<<" ";
logfile<<instancenumber<<" ";

logfile<<directory<<" ";



for(unsigned i=0;i<iterationsperparameter;++i) 
	logfile<<allresults[i]<<" ";


for(unsigned i=0;i<num_features;++i) 
	logfile<<features[i]<<" ";


for(unsigned int i=0;i<num_parameters;++i) 
	logfile<<(double)bestparameters[0][i]<<" ";

for(unsigned int i=0;i<num_parameters;++i) 
	logfile<<(double)getRealBestParameterValue(i)<<" ";

logfile<<bestparameters.size();

logfile<<" first_bestparameters";

logfile<<endl;

logfile.close();

}




void instanceHandler::init()
{


stringstream bd;



bd<<"./instances/"<<id;

basedirectory=bd.str().c_str();



stringstream command1;

command1<<"mkdir ./instances"<<endl;

cout<<command1.str()<<endl;

system(command1.str().c_str());


stringstream command2;

command2<<"mkdir "<<basedirectory.c_str()<<endl;

cout<<command2.str()<<endl;

system(command2.str().c_str());

logheader();
	
}


void instanceHandler::addHint(vector<double>& parameters)
{
tryparameters=parameters;
isthisoptimizerhint=false;
}



int instanceHandler::readResult() // return -1=failed 0=not reade 1=ready 2=bestresult added 3=new best result
{

int r=0;

if(!taken)
	{
	if (loglevel>1)	
		cout<<" checktaken "<<id<<endl;
	stringstream paramfilereadyname;
	
	stringstream paramfilename;
	
	paramfilename<<basedirectory.c_str()<<"/parameters_";
	paramfilename<<id;
	paramfilename<<".ready";
	
	ifstream paramfilefileready;
	
	paramfilefileready.open(paramfilename.str().c_str());


	if(!paramfilefileready.is_open())
		{
		if (loglevel>2)
			cout<<" taken "<<id<<endl;
		time(&start); //we start the timer when the file is taken
		taken=1;
		return 0; //it was just taken now, let us wait!
		}
	else
		return 0; //until it is not taken, there is no reason to check result	
	}

for(unsigned int i=0;i<iterationsperparameter;++i)
	{
	
	if(!readyiterations[i]) //if this iteration is not yet ready
		{
		
		checkexitandreadconfig();

		stringstream resultfilereadyname;

		resultfilereadyname<<basedirectory.c_str()<<"/result_";
		resultfilereadyname<<id;
		resultfilereadyname<<"_";
		resultfilereadyname<<i;
		resultfilereadyname<<".ready";
		if (loglevel>1)
			cout<<"checking file "<<resultfilereadyname.str()<<endl;

		ifstream resultfileready;
		resultfileready.open(resultfilereadyname.str().c_str());

		bool failed=false;
		bool nochecktime=false;

		if(resultfileready.is_open())
			{

			resultfileready.close();

			stringstream resultname;

			resultname<<basedirectory.c_str()<<"/result_";
			resultname<<id;
			resultname<<"_";
			resultname<<i;
			resultname<<".txt";

			double result=0;
			double checkresult=0;	
			
			if (loglevel>1)
				cout<<"reading"<<resultname.str()<<endl;

			ifstream resultfile;

			resultfile.open(resultname.str().c_str());
	
			if (resultfile.is_open())
				{
				resultfile>>result;
				resultfile.close();
				if (loglevel>1)
					cout<<"result "<<result<<endl;
				}


			system("sleep 1"); 
		
			checkresult=0; //we check to be very sure. we need this, because one mistake makes evrything wrong

			resultfile.open(resultname.str().c_str());
	
			if (resultfile.is_open())
				{
				resultfile>>checkresult;
				resultfile.close();
				if (loglevel>1)
					cout<<"checkresult "<<checkresult<<endl;
				if(!checkresult)
					nochecktime=true; //it is really zero, let us not waist time$

				}
			
			if((result)&&(result==checkresult))
				{
				cout<<"accepted for "<<id<<" iteration "<<i<<"result "<<result <<endl;
				++numreadyiterations;
				agregatedresult+=result;
				allresults[i]=result;
				readyiterations[i]=1;
				time(&start); //there is progress, reset timeout
				if (loglevel>1)
					cout<<"time"<<start<<endl;
				stringstream command;
				command<<"rm ";
				command<<resultname.str();
				if (loglevel>2)
					cout<<command.str()<<endl;
				system(command.str().c_str());

				stringstream command2;
				command2<<"rm ";
				command2<<resultfilereadyname.str();
				if (loglevel>2)
					cout<<command2.str()<<endl;
				system(command2.str().c_str());
				
				}
			else	
				failed=true;
				
			} //if(resultfileready.is_open())
			else	failed=true;



		if (failed)
			{
				time_t now;
				if (loglevel>1)
					cout<<"failed"<<endl;
				time(&now);
				if (loglevel>2)
					cout<<"time start "<<start<<" time now "<<now<<endl;
				double diff=difftime(now, start);
				if ((diff>resubmittime) || (nochecktime))
					{
					if (resubmitted>MAXRESUBMITNUMBER)
						{
						cout<<"reached maximal resubmit exiting this instance "<< id << " iteration " << i <<endl;
						numreadyiterations=iterationsperparameter; // this ends this instance without result
						readyiterations[i]=2;
						ready=true;
						return -1; //failed
						}
					cout<<"resubmiting beacuse of missing "<< id << " iteration " << i <<endl;
					startJob(i); //resubmit this iteration
					time(&start);
					resubmitted++;
					} 	

			}

		}//if(!readyiterations[i]) 

	} //for(unsigned int i=0;i<iterationsperparameter;++i)

if (numreadyiterations>=iterationsperparameter)
	{

	agregatedresult=getMedian(allresults); //median

	loglasttry();

	cout<<"agregatedresult for "<<id<<"	 "<<agregatedresult<<endl;

	if (agregatedresult == bestobjective) //we store multiple best parameters, since that is useful for the learner
		{
		bestparameters.push_back(tryparameters);
		if (bestobjective>=0)
			isthisdefaultparameters=false; //this is at least second cannot be the default parameters
		r=2; //return 2 indicating bestparameters added

		}


	if ((agregatedresult<bestobjective) || (bestobjective < 0))
		{

		if (bestobjective>=0)
			{
			isthisdefaultparameters=false; //this is at least second improvement, cannot be the default parameters
			doesthiscontaindefaultparameters=false; //this is at least second improvement, cannot be the default parameters
			r=3; //return 3 indicating new bestparameter added
			}
		else 
			{
			r=1; //this is the case for the result od the default parameter
			}



		bestobjective=agregatedresult;
		bestparameters.clear();
		bestparameters.push_back(tryparameters);
		
			
		}

	if (loglevel>2)
		logbest();


	if(isthisoptimizerhint)
		{
		myoptimizer.fitnessOfLastHint(agregatedresult);
		}
	else
		{
		myoptimizer.addHint(tryparameters, agregatedresult);
		}

	ready=true;

	} //if (numreadyiterations==iterationsperparameter)

return r;

}


void instanceHandler::startJob(int numberofiteration) //numberofiteration==iterationsperparameter indicates that all iterations are submited, numberofiteration<iterationsperparameter indicates a specific iteration is resubmited
{


if(numberofiteration==iterationsperparameter) //first submission
	{
	readyiterations=vector<unsigned 	int>(iterationsperparameter,0);
	numreadyiterations=0;
	agregatedresult=0;
	resubmitted=0;
	}

stringstream paramfilename;

paramfilename<<basedirectory.c_str()<<"/parameters_";
paramfilename<<id;

string command("cp parameter.base ");

command+=paramfilename.str();

cout<<command<<endl;

system(command.c_str());

ofstream paramsfile;


paramsfile.open(paramfilename.str().c_str(),ios_base::app);

for (unsigned int i=0;i<num_parameters;++i)
	{

	paramsfile<<"--"<<paramnames[i]<<"=";

	double value=getRealTryParameterValue(i);

	paramsfile<<value<<" #"<<tryparameters[i]<<endl;

	}

paramsfile<<"--maxEval="<<runningtime<<endl;


paramsfile<<directory<<endl; 
paramsfile<<instancenumber<<endl;
paramsfile<<numberofiteration<<endl;


paramsfile.close();

paramfilename<<".ready";

paramsfile.open(paramfilename.str().c_str(),ios_base::app);

paramsfile.close();

if(numberofiteration==iterationsperparameter) //first submission
	system("sleep 1");

taken=0;
ready=false;

}





void instanceHandler::print()
{
cout<<id<<" ";
cout<<bestobjective<<" ";
cout<<runningtime<<" ";
cout<<instancenumber<<" ";

cout<<directory<<" ";

for(unsigned i=0;i<num_features;++i) 
	cout<<features[i]<<" ";


for(unsigned int i=0;i<num_parameters;++i) 
	cout<<(double)bestparameters[0][i]<<" ";


cout<<endl;

}

#endif
