function grid(domainfolder, firstproblem,lastproblem, daexrunningtime,numdaexrepetitions)

global numparameters;
numparameters=8;

global daexrepetitions;
daexrepetitions=numdaexrepetitions;

global daextime;
daextime=daexrunningtime;

global numberoffirstproblem;
numberoffirstproblem=firstproblem;
global numberoflastproblem;
numberoflastproblem=lastproblem;

global parameternames;
global parametermultipliers;



addpath /users/tao/brendel/Desktop/revac_demo/lib_revac;
addpath /users/tao/brendel/Desktop/revac_demo/lib_revac/tools;
addpath /users/tao/brendel/Desktop/revac_demo;


oldfolder=cd(domainfolder);

delete('tuning.log');

state=0;

parametermultipliers(1)=1;
parametermultipliers(2)=1;

parameternames={' ';' '};


parameternames{1}='--mut1Rate';
parameternames{2}='--mut2Rate';

for i=0:10
	for j=0:10

parameters(1)=i;
parameters(2)=j;

tuning_core(parameters,state);

	end;
end;





cd(oldfolder);

end
