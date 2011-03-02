#ifndef OPTIMIZER_H
#define OPTIMIZER_H


#include "globals.h"

#include <vector>
#include <EALib/ChromosomeCMA.h>

using namespace std;



class optimizer
{
public:
optimizer(unsigned int dimension,unsigned int id);
~optimizer();

void getHint(vector<double>& parameters);
void fitnessOfLastHint(double fitness);
void addHint(vector<double>& parameters, double fitness);
double getSigma(){return bestparameters.getSigma();};


private:

unsigned int m_dimension;

ChromosomeCMA bestparameters;
ChromosomeCMA tryparameters;
double bestfitness;

unsigned int m_id;

double punishmentterm;

};


optimizer::optimizer(unsigned int dimension, unsigned int id):m_dimension(dimension),bestfitness(0),m_id(id)
{
bestparameters.init(dimension,initialsigma,0,1);
tryparameters.init(dimension,initialsigma,0,1);
bestparameters.mutate();


stringstream logfilename;

logfilename<<"optimizer_log_"<<m_id<<".txt";

ofstream logfile;

logfile.open(logfilename.str().c_str());

logfile<<"CMA-ES optimizer # "<<id<<endl;

logfile.close();


}


optimizer::~optimizer()
{

}



void optimizer::getHint(vector<double>& parameters)
{
tryparameters=bestparameters;
tryparameters.mutate();

punishmentterm=0;

parameters.resize(tryparameters.size());

stringstream logfilename;

logfilename<<"optimizer_log_"<<m_id<<".txt";

ofstream logfile;

logfile.open(logfilename.str().c_str(),ios_base::app);

logfile<<"getHint ";

for (unsigned n=0;n<tryparameters.size();++n)
	{
	parameters[n]=tryparameters[n];
	if ( tryparameters[n]<0 )
		{
		punishmentterm-=tryparameters[n]; 
            	parameters[n]=0.0;
		}
	if ( tryparameters[n]>1 )
		{
		punishmentterm+=tryparameters[n]-1; 
            	parameters[n]=1.0;
		}
	logfile<<tryparameters[n]<<"->"<<parameters[n]<<" ";
	}

logfile<<punishmentterm<<endl;
logfile.close();

}



/*
void optimizer::getHint(vector<double>& parameters)
{
tryparameters=bestparameters;
tryparameters.mutate();

for (unsigned n=0;n<tryparameters.size();++n)
            tryparameters[n]=min(1.0,max(0.0,tryparameters[n]));

parameters=(vector<double>)tryparameters;

}*/


/*void optimizer::getHint(vector<double>& parameters)
{


bool isininterval=true;

do
	{

	tryparameters=bestparameters;
	tryparameters.mutate();

	isininterval=true;

	for (unsigned n=0;n<tryparameters.size();++n)
			if (( tryparameters[n]<0 ) || (tryparameters[n]>1.0))
				isininterval=false;
	if (isininterval)
		parameters=(vector<double>)tryparameters;
	else
		{
		bestparameters.updateGlobalStepsize(false);

		stringstream logfilename;

		logfilename<<"optimizer_log_"<<m_id<<".txt";

		ofstream logfile;

		logfile.open(logfilename.str().c_str(),ios_base::app);
		logfile<<"hint oustide of interval "<<endl;
		for (unsigned n=0;n<tryparameters.size();++n)		
			logfile<<tryparameters[n]<<" ";
		logfile<<endl;

		logfile.close();
		}
	}
while(isininterval==false);

	

}*/




void optimizer::fitnessOfLastHint(double fitness)
{

stringstream logfilename;

logfilename<<"optimizer_log_"<<m_id<<".txt";

ofstream logfile;

logfile.open(logfilename.str().c_str(),ios_base::app);

logfile<<"fitness "<<fitness;

fitness+=punishmentterm;

logfile<<" punishment "<<punishmentterm<<" corrected fitness "<<fitness<<endl;


if ((fitness<=bestfitness)||(bestfitness==0))
	{
	bestfitness=fitness;
        bestparameters=tryparameters;
	
	logfile<<"new best result "<<bestfitness<<endl;
	
	

         if (loglevel>1)
	         cout<<"updateCovariance "<<endl;
         bestparameters.updateCovariance();
         if (loglevel>1)	
	         cout<<"updateGlobalStepsize "<<endl;
	
         bestparameters.updateGlobalStepsize(true);
	

	cout<<"new best result "<<bestfitness<<endl;
	

	}
else	
	{
	
	bestparameters.updateGlobalStepsize(false);
	logfile<<"updateGlobalStepsize(false)"<<endl;
	
	}

logfile.close();	

}


void optimizer::addHint(vector<double>& parameters, double fitness)
{

punishmentterm=0;

//if ((fitness<bestfitness)||(bestfitness==0))
	{
	tryparameters=bestparameters;
	tryparameters.mutate(); // we do as we got this as a hint, but ovverride it
	
	for (unsigned n=0;n<tryparameters.size();++n)
        	tryparameters[n]=parameters[n]; //override
			fitnessOfLastHint(fitness);	
	}



}

#endif
