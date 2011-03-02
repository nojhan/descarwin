function result = tuning_core(parameters,state)

global numberoffirstproblem;
global numberoflastproblem;
global daextime;
global daexrepetitions;
global evaluations;
global parameternames;
global parametermultipliers;

if (numberoffirstproblem<1)
	problems=textread('./train.txt','%s');
else
	problems={};
	for problem=numberoffirstproblem:numberoflastproblem
		str=sprintf('%d',problem); 
		problems{end+1}=str;
	end

end


copyfile('parameters.base','parametre');

s=size(parameters);
s=s(2);

parameterfile=fopen('parametre','a+');

for p=1:s

	fprintf(parameterfile,'%s=%f \n',parameternames{p},parametermultipliers(p)*parameters(p));

end;


fclose(parameterfile);

    
evaluations=evaluations+1;


command=sprintf('RunAllExperiments_local.sh p p %d %d %d %d',numberoffirstproblem,numberoflastproblem,daexrepetitions,daextime);

fprintf(1,'%s\n',command);

system(command);





for daexiteration=1:daexrepetitions

result=0;

	for problem=1:size(problems)

		makespan=realmax;

		resultfile=sprintf('Res%s/makespan%s_%d.soln',problems{problem},problems{problem},daexiteration);



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

		logfile=fopen('tuning.log','a+');

		if (logfile ~= -1)

			fprintf(logfile,'daex run: %g ',evaluations);

			fprintf(logfile,'%s ',problems{problem});

			fprintf(logfile,'%g ',daexiteration);

			for p=1:s

				fprintf(logfile,'%g ',parameters(p));

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

			for p=1:s

				fprintf(1,'%g ',parameters(p));

			end;

		fprintf(1,'%g\n',makespan);

	end; 
result=result/(numberoflastproblem-numberoffirstproblem+1);

result 

results(daexiteration)=result;

end; 

results

result=median(results)


logfile=fopen('tuning.log','a+');

if (logfile ~= -1)

	fprintf(logfile,'accumulated makespan: ');


	for p=1:s

		fprintf(logfile,'%g ',parameters(p));

	end;


	fprintf(logfile,'%g\n',-result);



	fprintf(1,'accumulated makespan: ');

	for p=1:s
		fprintf(1,'%g ',parameters(p));
	end;


	fprintf(1,'%g\n',result);

else
	message=ferror(logfile);
	fprintf(1,'could not lof to logfile %s', message);
end


fclose(logfile);


end
