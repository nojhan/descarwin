function my_search=daex_revac(domainfolder, firstproblem,lastproblem, numiterations, daexrunningtime, numdaexrepetitions, p_statisticalmethod)

global numparameters;
numparameters=12;

global daexrepetitions;
daexrepetitions=numdaexrepetitions;

global daextime;
daextime=daexrunningtime;

global evaluations;

evaluations=0;

global numberoffirstproblem;
numberoffirstproblem=firstproblem;
global numberoflastproblem;
numberoflastproblem=lastproblem;

global statisticalmethod;

statisticalmethod=p_statisticalmethod;


addpath /users/tao/brendel/Desktop/revac_demo/lib_revac;
addpath /users/tao/brendel/Desktop/revac_demo/lib_revac/tools;
addpath /users/tao/brendel/Desktop/revac_demo;


default('search',[]);

search   = default(search, 'max_nof_values', numiterations);


oldfolder=cd(domainfolder);

delete('revac.log');

search = revac(@daex, search, numparameters);

logfile=fopen('revac.log','a+');


if ~isset(search, 'entropy')
  [search.entropy, ...
   search.density, ...
   search.total_density, ...
   search.total_distribution] = revac_entropy(search);
end;

if ~isset(search, 'trivial_entropy')
search.trivial_entropy = revac_trivial_entropy(search);
end;


if (logfile ~= -1)
	fprintf(logfile,'entropies: ');
	for i=1:numparameters
		fprintf(logfile,'%g ',search.entropy(i,numiterations-99));
	end;
	fprintf(logfile,'\n');

end;
if (logfile ~= -1)
	fprintf(logfile,'trivial entropy: ');
	
	fprintf(logfile,'%g ',search.trivial_entropy(numiterations-99));
	
	fprintf(logfile,'\n');

end;
fclose(logfile);

cd(oldfolder);

end
