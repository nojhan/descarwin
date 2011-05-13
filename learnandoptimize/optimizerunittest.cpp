#include "globals.h"
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include "time.h"
#include "instanceHandler.h"
#include "mappingLearnerFANN.h"
#include "mappingLearnerSVM.h"
#include "mappingLearnerSharkFFNET.h"
#include "genetransferer.h"


using namespace std;



double testfunction(vector<double>& arguments)
{
//return 1.1-sin(3.14*arguments[0])*sin(3.14*arguments[1]);

double x=10.0*(arguments[0]-0.5);
double y=10.0*(arguments[1]-0.5);

return 20.0 + x*x-10*cos(2*3.14*x)+y*y-10*cos(2*3.14*y);

}


int main ( int argc, char* argv[] )
{



mappingLearner* mappinglearner;

checkexitandreadconfig();


optimizer opt(2, 0);

vector<double> arguments(2,0);

double fitness;

for (unsigned int i=0;i<10000000;++i)
	{
	if (i%5==0)
//	if (0)
		{
		arguments[0]=(rand() % 100 )/100.0;
		arguments[1]=(rand() % 100 )/100.0;
		fitness=testfunction(arguments);
		cout<<"fitness: "<<fitness<<" "<<"sigma: "<<opt.getSigma()<<" addHint"<<endl;
		opt.addHint(arguments,fitness);
		
		}
	else
		{
		opt.getHint(arguments);
		fitness=testfunction(arguments);
		cout<<"fitness: "<<fitness<<" "<<"sigma: "<<opt.getSigma()<<endl;
		opt.fitnessOfLastHint(fitness);
		}
	}


cout<<"end fitness: "<<fitness<<" "<<"sigma: "<<opt.getSigma()<<endl;
cout<<"fitness should go down to around 1 or even 0"<<endl;




		

	return 0;
}


