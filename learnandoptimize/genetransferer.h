#ifndef GENETRANSFERER_H
#define GENETRANSFERER_H

#include <iomanip>
#include "globals.h"
#include <time.h>
#include "instanceHandler.h"

using namespace std;

vector <vector < double > > distances;
vector< double> featureweights;

int currentinstance=-1;	

bool mylessthan (unsigned int i,unsigned int j) { return (distances[currentinstance][i]<distances[currentinstance][j]); }

double euclideandistance(vector<double>& a,vector<double>& b)
	{
		
	double d=0;
	for(unsigned int i=0;i<num_features;++i)
		{
		double dd=a[i]-b[i];
		dd*=featureweights[i]; 
		d+=dd*dd;
		}
	d/=num_features;//normalize just so that the numbers remain similar with more features
	d=sqrt(d);
	return d;
	}


void readfeatureweightsfile()
{

stringstream logfilename;
	

logfilename<<"genetransferer_log"<<".txt";

ofstream logfile;

logfile.open(logfilename.str().c_str());


ifstream featureweightsfile;

featureweightsfile.open("featureweights.txt");

if (!featureweightsfile.is_open())
	exit(-1);

logfile<<"Feature weigths: ";

for (unsigned int i=0;i<num_features;++i)
	{
	double w;
	featureweightsfile>>w;
	logfile<<w<<" ";
	featureweights.push_back(w);
	char temp[10000];
	featureweightsfile.getline(temp,10000);
	}

featureweightsfile.close();
logfile<<endl;

logfile.close();
}


class geneTransferer
{

public:

geneTransferer(){/*readfeatureweightsfile();*/}; //now we compute them
~geneTransferer(){};

void init(int numinstances,vector<instanceHandler>* pInstanceHandlers);
int getRandomInstance(int forthisinstance);
int getNextInstance(unsigned int forthisinstance, unsigned int& number);
void instanceBestParameterChanged(unsigned int thisinstance);


private:

int numberofinstances;
vector<instanceHandler>* instanceHandlers;

vector <vector <unsigned int> > instancesinOrder;
vector <vector <unsigned int> > numberTried; //how many best parameters did I already transfer

};

void geneTransferer::init(int numinstances,vector<instanceHandler>* pInstanceHandlers)
{



numberofinstances=numinstances;  
instanceHandlers=pInstanceHandlers;
srand (time(NULL) );

stringstream logfilename;
	

logfilename<<"genetransferer_log"<<".txt";

ofstream logfile;

logfile.open(logfilename.str().c_str());

logfile<<"GeneTransferer "<<endl;
logfile<<"number of instances "<< numberofinstances <<endl;
logfile<<"distances"<<endl;

vector<double> featuremeans(num_features,0);


for (unsigned int i=0;i<numberofinstances;++i)
	{
	vector <double > row(numberofinstances,0);
	distances.push_back(row);
	vector <unsigned int > row2(numberofinstances,0);
	instancesinOrder.push_back(row2);
	numberTried.push_back(row2);
	for (unsigned int j=0;j<numberofinstances;++j)
		instancesinOrder[i][j]=j;

	for(unsigned int f=0;f<num_features;++f)
		featuremeans[f]+=(*instanceHandlers)[i].features[f]; //compute feature means
	
	}

logfile<<"featuremeans: ";

	for(unsigned int f=0;f<num_features;++f)
		{
		featuremeans[f]/=numberofinstances; //compute feature means
		featureweights.push_back(0); 
		logfile<<featuremeans[f]<<" ";
		}

logfile<<endl;



for (unsigned int i=0;i<numberofinstances;++i)
	{
	
	for(unsigned int f=0;f<num_features;++f)
		{
		double d=(*instanceHandlers)[i].features[f]-featuremeans[f];
		featureweights[f]+=d*d; //standard deviation
		}


	}

logfile<<"standard deviations and featureweigts: ";

for(unsigned int f=0;f<num_features;++f)
		{
		featureweights[f]/=numberofinstances-1; //compute standard deviation
		featureweights[f]=sqrt(featureweights[f]);
		logfile<<featureweights[f]<<" ";
		if (featureweights[f]) //do not divide by zero zero does not matter
			featureweights[f]=1/featureweights[f]; 
		logfile<<featureweights[f]<<",";
		}

logfile<<endl;

for (unsigned int i=0;i<numberofinstances;++i)
	{


	for (unsigned int j=0;j<=i;++j)
		{
		double d=euclideandistance((*instanceHandlers)[i].features,(*instanceHandlers)[j].features);
		distances[i][j]=d;
		distances[j][i]=d;
		logfile<<setprecision(8)<<d<<" ";
		}
	logfile<<endl;	
}




for (unsigned int i=0;i<numberofinstances;++i)
	{
	currentinstance=i;
	sort(instancesinOrder[i].begin(),instancesinOrder[i].end(),mylessthan);
	}


for (unsigned int i=0;i<numberofinstances;++i)
	{
	for (unsigned int j=0;j<numberofinstances;++j)
		logfile<<instancesinOrder[i][j]<<" ";
	logfile<<endl;
	}



logfile.close();

}

void geneTransferer::instanceBestParameterChanged(unsigned int thisinstance)
{

stringstream logfilename;
logfilename<<"genetransferer_log"<<".txt";

ofstream logfile;

logfile.open(logfilename.str().c_str(),ios_base::app);

logfile<<"instance changed "<< thisinstance<<endl;

for (unsigned int i=0;i<numberofinstances;++i) 
	numberTried[i][thisinstance]=0;

logfile.close();
}


int geneTransferer::getNextInstance(int unsigned forthisinstance, unsigned int& number)
{

number=0;

stringstream logfilename;
logfilename<<"genetransferer_log"<<".txt";

ofstream logfile;

logfile.open(logfilename.str().c_str(),ios_base::app);

logfile<<" For this instance "<<forthisinstance<<endl;

for (unsigned int i=1;i<numberofinstances;++i) //we can not take itself
	{
	unsigned int candidate=instancesinOrder[forthisinstance][i];

	if (loglevel > 2)
		logfile<<"Candidate instance "<<candidate<<endl;



	bool isthisdefaultparameters=(*instanceHandlers)[candidate].isthisdefaultparameters;
	if (isthisdefaultparameters)
		{
		if (loglevel > 2)
			logfile<<"default parameters rejected " <<endl;
		continue;		
		}

	bool doesthiscontaindefaultparameters=(*instanceHandlers)[candidate].doesthiscontaindefaultparameters; //here we have several best parameters, but it contains the default

	unsigned int tried=numberTried[forthisinstance][candidate];

	if(doesthiscontaindefaultparameters)
		tried++; //default does not count

	if (loglevel > 2)
		logfile<<"tried " <<tried<<endl;
	unsigned int available=(*instanceHandlers)[candidate].bestparameters.size();

	if (loglevel > 2)
		logfile<<"available "<<available<<endl;

	
	if(available>tried)
		{
		number=tried;
		++numberTried[forthisinstance][candidate];
		logfile<<"Accepted "<<candidate<< " "<< number<<endl;
		return candidate;
		}

	}


logfile.close();
return -1;
}


int geneTransferer::getRandomInstance(int forthisinstance)
{
	stringstream logfilename;
	logfilename<<"genetransferer_log"<<".txt";

	ofstream logfile;

	logfile.open(logfilename.str().c_str(),ios_base::app);

	logfile<<whatislasttry<<endl; //info about iteration


for(unsigned int tryal=0;tryal<numberoftrials;++tryal)
	{
	


  	int candidate = rand() % numberofinstances;

	bool isthisdefaultparameters=(*instanceHandlers)[candidate].isthisdefaultparameters;

	if ( (candidate !=forthisinstance) && !( isthisdefaultparameters)) //do not transfer default parameters!
	//if  (candidate !=forthisinstance) 
		{
		logfile<<"transfer destination "<< forthisinstance << " source " << candidate <<endl;
		logfile.close();
		return candidate;
		}
	
	if (loglevel > 2)
			logfile<<"refuted destination "<< forthisinstance << " source " << candidate<< " default? "<<isthisdefaultparameters<<endl;


	
	

	}

logfile.close();

return -1; //no succes


}

#endif
