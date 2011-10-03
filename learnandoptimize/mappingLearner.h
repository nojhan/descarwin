#ifndef MAPPINGLEARNER_H
#define MAPPINGLEARNER_H

#include "globals.h"

#include <stdio.h>
#include <vector>
#include "fann.h"
#include "fann_train.h"


using namespace std;


class mappingLearner //just an interface, i.e. an abstract base-class you must to inherit and overload the funcions
{

public:

 mappingLearner(unsigned int numinput, unsigned int numoutput);
 mappingLearner(){};
virtual ~mappingLearner(){};
virtual double train(vector<vector<double> >& inputs, vector<vector<double> >& outputs)=0;
virtual double train(vector<vector<double> >& inputs, vector<vector<double> >& outputs, vector<double>& fitnesses, vector<int>& instances)=0;
virtual double run(vector<double>& input, vector<double>& output)=0;
virtual void save(char *filename)=0;
virtual bool load(char *filename)=0;
virtual bool areYouDirectMapping()=0;


private:


};

#endif
