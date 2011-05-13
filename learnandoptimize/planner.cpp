#include <stdio.h>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include "time.h"
#include "instanceHandler.h"
#include "mappingLearnerFANN.h"
#include "mappingLearnerSharkFFNET.h"

using namespace std;

vector<double> features;
vector<double> parameters;


void correctcrazyparameters() //this sound silly, why did we not train with these limits in the first place? because we did not think it over
//moreover, large values of nbOffspring seem to be OK in general, but for 15 mins it seems to be risky
//especially, if we consider that the test set can produce higher values iven if the train set did not, simply by extrapolation
{
cout<<"possible parameter corrections"<<endl;
cout<<parameters[1]<<endl;
if (parameters[1]<0.1)
	parameters[1]=0.1; //mutrate

cout<<parameters[6]<<endl;
if (parameters[6]<0.1) //	pMeanAveragePerMutation
	parameters[6]=0.1;

cout<<parameters[1]<<endl;

if (parameters[11]>0.5) // nbOffspring	
	parameters[11]=0.5;

	
}

void writeparamsfile()
{
ofstream paramsfile;


paramsfile.open("parametre");

if (parameters.size()==num_parameters)
	{

	for (unsigned int i=0;i<num_parameters;++i)
		{

		paramsfile<<"--"<<paramnames[i]<<"=";
		cout<<"--"<<paramnames[i]<<"=";

		double value=value=minvalues[i]+parameters[i]*(maxvalues[i]-minvalues[i]); 

		paramsfile<<value<<" #"<<parameters[i]<<endl;
		
		cout<<value<<" #"<<parameters[i]<<endl;

		}
	}
else
	{

	for (unsigned int i=0;i<num_parameters;++i)
		{

		paramsfile<<"--"<<paramnames[i]<<"=";
		cout<<"--"<<paramnames[i]<<"=";

		double value=value=minvalues[i]+defaultvalues[i]*(maxvalues[i]-minvalues[i]); 

		paramsfile<<value<<" #"<<defaultvalues[i]<<endl;
		cout<<value<<" #"<<defaultvalues[i]<<endl;

		}
	}

}


void readfeatures()
{

ifstream featuresfile;
featuresfile.open ("features.txt");


if (featuresfile.is_open()) 
	{
	features.resize(num_features);

	for(unsigned int i=0;i<num_features;++i)
		{
		featuresfile>>features[i];
		cout<<features[i]<<" ";
		}
	}

}


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





int main ( int argc, char* argv[] )
{


if (argc<6)
	{
	cout<<"Usage: "<<argv[0]<<" <domain file> <problem file> <ANN file> <resultfile> <time limit>"<<endl;
	exit(-1);
	}

char* domainfile=argv[1];
char* problemfile=argv[2];
char* ANNfile=argv[3];
char* resultfile= argv[4];
char* timelimit=argv[5];

	mappingLearner* mappinglearner;

	checkexitandreadconfig();

	if(learningModelType==SharkFFNET)
		mappinglearner= dynamic_cast<mappingLearner*>(new mappingLearnerSharkFFNet(num_features,num_parameters)); 
	else
		mappinglearner= dynamic_cast<mappingLearner*>(new mappingLearnerFANN(num_features,num_parameters)); 


	parameters.clear();

	cout<<"read paramsfile"<<endl;

	readparamnamesfile();



	cout<<"Generate featurefile"<<endl;

	
	stringstream command;

	
	command<<"generatefeaturesforoneproblem.sh "<<domainfile<<" "<<problemfile<<" features.txt"<<endl;

	cout<<command.str().c_str();

	system(command.str().c_str());

	
	cout<<"Load features"<<endl;
	
	readfeatures();

	cout<<endl;
	cout<<"Load mappinglearner"<<endl;

	

	bool succes=mappinglearner->load(ANNfile);
	
	



	cout<<"Use ANN"<<endl;
		

	if (succes)
		mappinglearner->run(features, parameters);

	
	cout<<"Write paramsfile"<<endl;
	
	correctcrazyparameters(); 

	writeparamsfile();

	cout<<"Execute DAEx"<<endl;

	stringstream command2;

	
	command2<<"RunDAEx.sh "<<domainfile<<" "<<problemfile<<" "<<resultfile<<" "<<timelimit<<endl; 

	cout<<command2.str().c_str();

	system(command2.str().c_str());
		


	return 0;
}

