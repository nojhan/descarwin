#ifndef GLOBALS_H
#define GLOBALS_H

#include <vector>
#include <string>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

enum LearningModelType { FANN, SharkFFNET, FANNIndirect, RankSVMIndirect};

LearningModelType learningModelType;

const int invalidresult=100000;

string modelfile("ANN.saved");

int num_parameters=12;
int num_features=13;
int numberofiterations=1000;

int numrestarts=5;

int useGeneTransferer=1;
int useLearner=1;

int normalizeindirect=0;


vector<string> paramnames;
vector<double> minvalues;

vector<double> maxvalues;
vector<double> defaultvalues;


int resubmittime=50*60; //in 30 minutes something should happen with at least one of the iterations, but we give 50 minutes

string whatislasttry;

int iterationsperparameter=11;

const int MAXRESUBMITNUMBER=3;

const unsigned int numberoftrials=5; //genetransferer

int numepochsperiteration=50; 

unsigned int loglevel=1;

double initialsigma=0.3;


int ratioOptimizerversusLearner=5;
int indirectlearneroptimizelength=10000; //number of steps the indirect learner can spend to optimize



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

ofstream outconfigfile;



outconfigfile.open("config.check");

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

outconfigfile<<loglevel<<" //loglevel" <<endl;



	unsigned int new_num_parameters=num_parameters;	
	configfile>>new_num_parameters;
	if (new_num_parameters!=num_parameters)
		{
		num_parameters=new_num_parameters;
		cout<<"num_parameters changed to "<<loglevel<<endl;
		}
	configfile.getline(temp,1000); //comments

outconfigfile<< num_parameters<<" //num_parameters" <<endl;

	unsigned int new_num_features=num_features;	
	configfile>>new_num_features;
	if (new_num_features!=num_features)
		{
		num_features=new_num_features;
		cout<<"num_features changed to "<<loglevel<<endl;
		}
	configfile.getline(temp,1000); //comments

outconfigfile<<num_features <<" //num_features" <<endl;


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

outconfigfile<<numberofiterations <<" //numberofiterations" <<endl;

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

outconfigfile<< numepochsperiteration<<" //numepochsperiteration" <<endl;

	int p_iterationsperparameter=0;
	//cout<<p_iterationsperparameter;
	configfile>>p_iterationsperparameter;
	if ((p_iterationsperparameter>0) && (iterationsperparameter!=p_iterationsperparameter))
		{
		iterationsperparameter=p_iterationsperparameter;
		cout<<"iterationsperparameter changed to "<<iterationsperparameter<<endl;
		}
	configfile.getline(temp,1000); //comments
	cout<<endl;

outconfigfile<< iterationsperparameter<<" //iterationsperparameter" <<endl;


	int p_useGeneTransferer=0;
	//cout<<p_useGeneTransferer;
	configfile>>p_useGeneTransferer;
	if ((p_useGeneTransferer!=useGeneTransferer) && (p_useGeneTransferer>-1))
		{
		useGeneTransferer=p_useGeneTransferer;
		cout<<"useGeneTransferer changed to "<<useGeneTransferer<<endl;
		}
	configfile.getline(temp,1000); //comments
	cout<<endl;

outconfigfile<<useGeneTransferer <<" //useGeneTransferer" <<endl;

	int p_useLearner=0;
	//cout<<p_useLearner;
	configfile>>p_useLearner;
	if ((p_useLearner!=useLearner) && (p_useLearner>-1))
		{
		useLearner=p_useLearner;
		cout<<"useLearner changed to "<<useLearner<<endl;
		}
	configfile.getline(temp,1000); //comments
	cout<<endl;

outconfigfile<<useLearner <<" //useLearner" <<endl;

	int p_ratioOptimizerversusLearner=0;
	//cout<<p_ratioOptimizerversusLearner;
	configfile>>p_ratioOptimizerversusLearner;
	if ((p_ratioOptimizerversusLearner!=ratioOptimizerversusLearner) && (p_ratioOptimizerversusLearner>0))
		{
		ratioOptimizerversusLearner=p_ratioOptimizerversusLearner;
		cout<<"ratioOptimizerversusLearner changed to "<<ratioOptimizerversusLearner<<endl;
		}
	configfile.getline(temp,1000); //comments
	cout<<endl;


outconfigfile<<ratioOptimizerversusLearner <<" //ratioOptimizerversusLearner" <<endl;


	learningModelType=FANN;

	string learningModelTypeString;
	
	configfile>>learningModelTypeString;
	//cout<<learningModelTypeString.c_str()<<endl;
	if ((learningModelTypeString.compare("SharkFFNET")==0) && (learningModelType!=SharkFFNET))
		{
		learningModelType=SharkFFNET;
		cout<<"learningModelType "<<learningModelTypeString<<endl;
		}
	if ((learningModelTypeString.compare("FANNIndirect")==0) && (learningModelType!=FANNIndirect))
		{
		learningModelType=FANNIndirect;
		cout<<"learningModelType "<<learningModelTypeString<<endl;
		}
	if ((learningModelTypeString.compare("RankSVMIndirect")==0) && (learningModelType!=RankSVMIndirect))
		{
		learningModelType=RankSVMIndirect;
		cout<<"learningModelType "<<learningModelTypeString<<endl;
		}


	configfile.getline(temp,1000); //comments
	cout<<endl;


outconfigfile<<learningModelType <<" //learningModelType" <<endl;


	int p_normalizeindirect=0;
	//cout<<p_normalizeindirect;
	configfile>>p_normalizeindirect;
	if ((p_normalizeindirect!=normalizeindirect) && (p_normalizeindirect>0))
		{
		normalizeindirect=p_normalizeindirect;
		cout<<"normalizeindirect changed to "<<normalizeindirect<<endl;
		}
	configfile.getline(temp,1000); //comments
	cout<<endl;


outconfigfile<<normalizeindirect <<" //normalizeindirect" <<endl;



string p_modelfile;
	//cout<<p_normalizeindirect;
	configfile>>p_modelfile;
	if (p_modelfile.length()!=0)
		{
		modelfile=p_modelfile;
		cout<<"modelfile changed to "<<modelfile<<endl;
		}
	configfile.getline(temp,1000); //comments
	cout<<endl;


outconfigfile<<modelfile <<" //modelfile" <<endl;


	int p_numrestarts=0;
	//cout<<p_numrestarts;
	configfile>>p_numrestarts;
	if ((p_numrestarts!=numrestarts) && (p_numrestarts>0))
		{
		numrestarts=p_numrestarts;
		cout<<"numrestarts changed to "<<numrestarts<<endl;
		}
	configfile.getline(temp,1000); //comments
	cout<<endl;


outconfigfile<<numrestarts <<" //numrestarts" <<endl;


	int p_indirectlearneroptimizelength=0;
	//cout<<p_numrestarts;
	configfile>>p_indirectlearneroptimizelength;
	if ((p_indirectlearneroptimizelength!=indirectlearneroptimizelength) && (p_indirectlearneroptimizelength>0))
		{
		indirectlearneroptimizelength=p_indirectlearneroptimizelength;
		cout<<"indirectlearneroptimizelength changed to "<<indirectlearneroptimizelength<<endl;
		}
	configfile.getline(temp,1000); //comments
	cout<<endl;


outconfigfile<<indirectlearneroptimizelength <<" //indirectlearneroptimizelength" <<endl;





outconfigfile.close();
	
	
	}
else 
	{
//	cout<<"no configfile"<<endl;
	loglevel=1;
	}

}



#endif
