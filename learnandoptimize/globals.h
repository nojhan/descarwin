#ifndef GLOBALS_H
#define GLOBALS_H

#include <vector>
#include <string>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;


const int num_parameters=12;
const int num_features=13;
int numberofiterations=1000;


vector<string> paramnames;
vector<double> minvalues;

vector<double> maxvalues;
vector<double> defaultvalues;


int resubmittime=50*60; //in 30 minutes something should happen with at least one of the iterations, but we give 50 minutes

string whatislasttry;

const int iterationsperparameter=11;

const int MAXRESUBMITNUMBER=3;

const unsigned int numberoftrials=5; //genetransferer

int numepochsperiteration=50; 

unsigned int loglevel=1;

double initialsigma=0.3;


const int ratioOptimizerversusLearner=5;



void checkexitandreadconfig()
{


ifstream stopfile;

char temp[10000];

stopfile.open("stop.txt");

if(stopfile.is_open())
	{
	system("echo 0 > workers_exit.txt");
	system("./cleanupafteriteration.sh > /dev/null");
	exit(-1);
	}


ifstream configfile;

//cout<<"config file"<<endl;

configfile.open("config.txt");

if(configfile.is_open())
	{
	unsigned int newloglevel=1;	
	configfile>>newloglevel;
	if (newloglevel!=loglevel)
		{
		loglevel=newloglevel;
		cout<<"loglevel changed to "<<loglevel<<endl;
		}
	configfile.getline(temp,1000); //comments
	int newnmbit=0;
	//cout<<newnmbit;
	configfile>>newnmbit;
	if ((newnmbit>0) && (numberofiterations!=newnmbit))
		{
		numberofiterations=newnmbit;
		cout<<"number of iterations changed to "<<numberofiterations<<endl;
		}
	configfile.getline(temp,1000); //comments
	cout<<endl;


	int p_numepochsperiteration=0;
	//cout<<p_numepochsperiteration;
	configfile>>p_numepochsperiteration;
	if ((p_numepochsperiteration>0) && (numepochsperiteration!=p_numepochsperiteration))
		{
		numepochsperiteration=p_numepochsperiteration;
		cout<<"numepochsperiteration changed to "<<numepochsperiteration<<endl;
		}
	configfile.getline(temp,1000); //comments
	cout<<endl;



	
	
	}
else 
	{
//	cout<<"no configfile"<<endl;
	loglevel=1;
	}

}



#endif
