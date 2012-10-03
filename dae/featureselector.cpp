	#include <iostream>
#include <set>
#include <algorithm>    // for set_intersection, set_union
#include <functional>   // for less
#include <string>
#include "utils/pddl_load.h" 
#include <utils/eoParser.h> 
#include "src_cpt/dae.h" 




using namespace daex;
using namespace std;



struct ltstr
{
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2) < 0;
  }
};


void histogramfeatures(vector<unsigned int>& histogram, double& mean, double& median, double& firstquartilearg, double& thirdquartilearg, double& minrange, double& maxrange)
{

unsigned int histogramlength=histogram.size();

int minarg=-1;
int maxarg=-1;

unsigned int num=0;

unsigned int i;

vector<int> allvalues;

mean=0;


for(i=0;i<histogramlength;++i)
	{
	unsigned int h=histogram[i];	
	
	   if(h && minarg==-1)
		minarg=i;
	    if(h)
		maxarg=i;
	mean+=h*(i+1);
	num+=h;
	}

mean/=num;

maxarg++; 

unsigned int counter=0;

num/=2;

for(i=minarg; i<maxarg, counter<num;++i)
	counter+=histogram[i];

	median=i;


double nb_firstquartile=num/2;

//cout<< " nb_firstquartile " <<nb_firstquartile;


 firstquartilearg=-1;

counter=0;

for(i=minarg; i<maxarg && counter<nb_firstquartile;++i)
	counter+=histogram[i];

//now i ha the realy value, the real index in the vector is one less!

if ((counter==nb_firstquartile)||(i==1)) //1 corresponds to 0 in the vector
	firstquartilearg=i;
else firstquartilearg=(double)(i*histogram[i-1]+i-1*histogram[i-2])/(double)(histogram[i-1]+histogram[i-2]); //linear interpolation

//cout<< " firstquartilearg raw " <<firstquartilearg;

firstquartilearg=(firstquartilearg-minarg)/double(maxarg-minarg); //normalize

 thirdquartilearg=-1;

counter=0;

for(i=maxarg; i>minarg && counter<nb_firstquartile;--i)
	counter+=histogram[i-1];
i++; //now i corresponds to the real value, which is one more than the index in the vector

if ((counter==nb_firstquartile)||(i==maxarg)) //maxarg is also the real value, not max index!
	thirdquartilearg=i;
else thirdquartilearg=(double)(i*histogram[i-1]+(i+1)*histogram[i])/(double)(histogram[i-1]+histogram[i]); //linear interpolation

//cout<< " thirdquartilearg raw " <<thirdquartilearg;

thirdquartilearg=(thirdquartilearg-minarg)/double(maxarg-minarg); //normalize

minrange=(double)minarg/(double)histogramlength;
maxrange=(double)(maxarg)/(double)histogramlength; //maxarg ose one more


}







void extractsomefeatures()
{


//cout<<endl;

 set<const char*> predicatenames;
 set<const char*> objectnames;
 set<const char*> typenames;



// pour chaque Atom
    for( unsigned int i=0; i < fluents_nb; ++i) 
	{

      /*
      std::cout << i;
      std::cout << "\t" << fluents[i]->atom->predicate->terms_nb;
      std::cout << "\t" << fluents[i]->atom->predicate->name;
      std::cout << std::endl;
      */


        predicatenames.insert(fluents[i]->atom->predicate->name);

	//cout<<" predicate " << fluents[i]->atom->predicate->name<<endl;

        for( unsigned int j=0; j < fluents[i]->atom->terms_nb; ++j )
		{
		//cout<<" object " << fluents[i]->atom->terms[j]->name<<endl;
		objectnames.insert(fluents[i]->atom->terms[j]->name);

           	 // pour chaque Type

            	for( unsigned int k=0; k < fluents[i]->atom->terms[j]->types_nb; ++k ) 
			{
			//cout<<" type " << fluents[i]->atom->terms[j]->types[k]->name<<endl;
                	typenames.insert( fluents[i]->atom->terms[j]->types[k]->name );
			} // for k type

        	} // for j objet


    	} // for i atom

cout<<" predicates "<<predicatenames.size();
cout<<" objects "<<objectnames.size();
cout<<" types "<<typenames.size();


vector<unsigned int> termshistogram; 
vector<unsigned int> typeshistogram; 


// pour chaque Atom
    for( unsigned int i=0; i < fluents_nb; ++i) 
	{
	unsigned int t=fluents[i]->atom->terms_nb;
	if(!(termshistogram.size()>t))
		termshistogram.resize(t+1,0);

	++termshistogram[t];

        for( unsigned int j=0; j < fluents[i]->atom->terms_nb; ++j )
		{
		unsigned int ty=fluents[i]->atom->terms[j]->types_nb;
		if(!(typeshistogram.size()>ty))
			typeshistogram.resize(ty+1,0);

		++typeshistogram[ty];

          	

        	} // for j objet


    	} // for i atom

double mean, median, minrange, maxrange, firstquartile, thirdquartile;

histogramfeatures(termshistogram, mean, median, firstquartile, thirdquartile, minrange, maxrange);





/*

cout<<endl;

for(unsigned int i=0; i < termshistogram.size(); ++i)
    cout<<" "<<termshistogram[i];
cout<<endl;

cout<<endl;

for(unsigned int i=0; i < typeshistogram.size(); ++i)
    cout<<" "<<typeshistogram[i];
cout<<endl;
*/


cout<<" termsmedianpermean "<<double(median)/mean;
/*
cout<<" termsminrange "<<minrange;
cout<<" termsmaxrange "<<maxrange;*/ //no meaning, constant
cout<<" termsfirstquartile "<<firstquartile;
cout<<" termslastquartile "<<thirdquartile;

histogramfeatures(typeshistogram, mean, median, firstquartile, thirdquartile, minrange, maxrange);


cout<<" typesmedianpermean "<<double(median)/mean;
/*
cout<<" typesminrange "<<minrange;
cout<<" typesmaxrange "<<maxrange; */ // no meaning, constant

cout<<" typesfirstquartile "<<firstquartile;
cout<<" typeslastquartile "<<thirdquartile;


vector<unsigned int> mutexhistogram(fluents_nb,0); 

int nb_mutex_pairs=0;

for(unsigned int i=0; i < fluents_nb; ++i)
    {
    int nummutexes=0;
    for(unsigned int j=0; j < fluents_nb; ++j)
	{
	if (fmutex(fluents[i], fluents[j])) 
	    ++nummutexes;
	}
    nb_mutex_pairs+=nummutexes;


    ++mutexhistogram[nummutexes];


    }

/*
cout<<endl;

for(unsigned int i=0; i < fluents_nb; ++i)
    cout<<" "<<mutexhistogram[i];
cout<<endl;
*/

double ratio=(double)nb_mutex_pairs/fluents_nb/fluents_nb;

cout<<" mutexdensity "<<ratio;



histogramfeatures(mutexhistogram, mean, median, firstquartile, thirdquartile, minrange, maxrange);


cout<<" mutexmedianpermean "<<double(median)/mean;
cout<<" mutexminrange "<<minrange;
cout<<" mutexmaxrange "<<maxrange;
cout<<" mutexfirstquartile "<<firstquartile;
cout<<" mutexlastquartile "<<thirdquartile;







}





int main ( int argc, char* argv[] )
{

eoParser parser(argc, argv);

std::string domain = parser.createParam( (std::string)"p01-domain.pddl", "domain", "PDDL domain file", 'D', "Problem", true ).value();
std::string instance = parser.createParam( (std::string)"p01.pddl", "instance", "PDDL instance file", 'I', "Problem", true ).value();


pddlLoad* pddload;

pddload=new pddlLoad( domain,instance,  SOLVER_YAHSP,  HEURISTIC_H1);


cout<<"features ";




CPTPredicateArray predicatearray;

predicatearray=cptGetPredicates();

cout<<" predicates "<< predicatearray.nb;

CPTFluentArray fluentarray;

fluentarray=cptGetInit();

cout<<" initialatoms "<< fluentarray.nb;

cout<<" fluents "<< fluents_nb;

fluentarray=cptGetGoal();

cout<<" goals "<< fluentarray.nb;


fluentarray=cptGetInit();

extractsomefeatures();

cout<<endl;

const ChronoPartition & chronopartition=pddload->chronoPartitionAtom();

vector<unsigned int> chronopartitionhistogram(chronopartition.size(),0); 

std::map<TimeVal, std::vector< daex::Atom*> >::const_iterator it=chronopartition.begin();


for (unsigned int i=0;it!=chronopartition.end();++it,++i)
{
chronopartitionhistogram[i]=(*it).second.size();
//cout<<chronopartitionhistogram[i]<<" ";
}

double mean, median, minrange, maxrange, firstquartile, thirdquartile;

histogramfeatures(chronopartitionhistogram, mean, median, firstquartile, thirdquartile, minrange, maxrange);


cout<<" chronomedianpermean "<<double(median)/mean;
cout<<" chronominrange "<<minrange;
cout<<" chronomaxrange "<<maxrange;
cout<<" chronofirstquartile "<<firstquartile;
cout<<" chronolastquartile "<<thirdquartile;



return 0;
}

