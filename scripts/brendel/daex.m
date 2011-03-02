function result = daex(parameters,state)

global numberoffirstproblem;
global numberoflastproblem;
global daextime;
global daexrepetitions;
global evaluations;
global statisticalmethod;
global numparameters;



fprintf(1,'state');
state

if (numberoffirstproblem<1)
	problems=textread('./train.txt','%s');
else
	problems={};
	for problem=numberoffirstproblem:numberoflastproblem
		str=sprintf('%d',problem); 
		problems{end+1}=str;
	end

end


copyfile('parameter.base','parametre');

parameterfile=fopen('parametre','a+');

fprintf(parameterfile,'--pCross=%f # -C : Probability of Crossover\n',parameters(1));

fprintf(parameterfile,'--pMut=%f # -M : Probability of Mutation\n',parameters(2));

fprintf(parameterfile,'--mut1Rate=%d  # -1 : Relative rate for atoms modification mutation 1.\n',int8(10*parameters(3)));
fprintf(parameterfile,'--mut2Rate=%d  # -1 : Relative rate for atoms modification mutation 2.\n',int8(10*parameters(4)));
fprintf(parameterfile,'--mut3Rate=%d  # -1 : Relative rate for atoms modification mutation 3.\n',int8(10*parameters(5)));
fprintf(parameterfile,'--mut4Rate=%d  # -1 : Relative rate for atoms modification mutation 4.\n',int8(10*parameters(6)));

fprintf(parameterfile,'--pMeanAveragePerMutation=%f # -A : Mean probability to change fluent of the  state in mutation\n',parameters(7));

fprintf(parameterfile,'--rayonBoule=%d                           # -r : rayon de la boule pour ajout d un atome dans un etat \n',int32(10*parameters(8)));

fprintf(parameterfile,'--stationMax=%d                           # -r : rayon de la boule pour ajout d un atome dans un etat \n',int32(5+45*parameters(9))); %[5,50] 

fprintf(parameterfile,'--maxb=%d                           # -r : rayon de la boule pour ajout d un atome dans un etat \n',int32(100+9900*parameters(10))); %[100,10000]

fprintf(parameterfile,'--popSize=%d                           # -r : rayon de la boule pour ajout d un atome dans un etat \n',int32(10+490*parameters(11))); %[10,500]

fprintf(parameterfile,'--nbOffspring=%d                           # -r : rayon de la boule pour ajout d un atome dans un etat \n',int32(100+4900*parameters(12))); %[100,5000]



fclose(parameterfile);

    
evaluations=evaluations+1;


command=sprintf('RunAllExperiments_local.sh p p %d %d %d %d %d',numberoffirstproblem,numberoflastproblem,daexrepetitions,daextime,state(1));

fprintf(1,'%s\n',command);

system(command);





for daexiteration=1:daexrepetitions

	result=0;

	for problem=1:size(problems)

		makespan=realmax;

		resultfile=sprintf('%d/Res%s/makespan%s_%d.soln',state(1),problems{problem},problems{problem},daexiteration);



		fprintf(1,'%s\n',resultfile);

		fileID=fopen(resultfile);

		if (fileID > 0)
			fclose(fileID);
			makespan=textread(resultfile, '%f');
		end

	
		if (isempty(makespan))
			makespan=realmax;
		end


		result=result-makespan;

		logfile=fopen('revac.log','a+');

		if (logfile ~= -1)

			fprintf(logfile,'daex run: %g ',evaluations);

			fprintf(logfile,'%s ',problems{problem});

			fprintf(logfile,'%g ',daexiteration);
			
			for i=1:numparameters
				fprintf(logfile,'%g ',parameters(i));
			end;

			fprintf(logfile,'%g\n',makespan);

		else
			message=ferror(logfile);
			fprintf(1,'could not lof to logfile %s', message);
		end

		fclose(logfile);


		fprintf(1,'%g ',evaluations);

		fprintf(1,'%s ',problems{problem});

		fprintf(1,'%g ',daexiteration);

		for i=1:numparameters
			fprintf(1,'%g ',parameters(i));
		end;


		fprintf(1,'%g\n',makespan);

	end; 

result=result/(numberoflastproblem-numberoffirstproblem+1);

results(daexiteration)=result

end; 

results

if (strcmp(statisticalmethod,'median'))

result=median(results)

else

result=mean(results)

end;




logfile=fopen('revac.log','a+');

if (logfile ~= -1)

	fprintf(logfile,'accumulated makespan: ');

	for i=1:numparameters
		fprintf(logfile,'%g ',parameters(i));
	end;


	fprintf(logfile,'%g\n',-result);



	fprintf(1,'accumulated makespan: ');

	for i=1:numparameters
		fprintf(1,'%g ',parameters(i));
	end;


	fprintf(1,'%g\n',result);

else
	message=ferror(logfile);
	fprintf(1,'could not lof to logfile %s', message);
end


fclose(logfile);

command=sprintf('rm -rfd %d',state(1));

fprintf(1,'%s\n',command);

system(command);


end
