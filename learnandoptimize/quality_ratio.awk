BEGIN{
	averagequalityratio=0;ddefault=0;tuned=0; instance=-1; number=0; solvedbytuned=0;solvedbydefault=0;solvedbytuningandnotbydefault=0;
	print "instancenumber referencemakespan makespan qualityratio"
	}

{ 

#print $(NF-1) " " $4 " " instance;

if ($(NF-1)!="source_of_parameters" && $4!=instance && instance!=-1 && $2 < 100000) #new instance
		{

		#printf ddefault " " tuned " "

		if (ddefault) #there is a default solution
			{
			solvedbydefault++;
			if(tuned)
				ratio=qualityddefault/qualitytuned;
			else
				{ratio=0; qualitytuned=-1;}
			averagequalityratio+=ratio;
			number++;
			}
		else
			{
			qualityddefault=-1;
			ratio=-1;
			if (tuned)
				solvedbytuningandnotbydefault++;
			}
		if(tuned) #there is solution with tuned
			solvedbytuned++;

		ddefault=0;
		tuned=0;
		
		printf instance " " qualityddefault " " qualitytuned " ";
		print ratio " ";
		#printf averagequalityratio " ";

		instance= $4;

		} 


if ( instance == -1 && $(NF-1)!="source_of_parameters")
	instance = $4;

if( $(NF-1) == "initialize_with_default" && $2 < 100000)
	{
	ddefault=1;
	qualityddefault=$2;
	}


if( $(NF-2) == "Epoch" && $2 < 100000)
	{
	tuned=1;
	qualitytuned=$2;
	}



} 

END { 

#printf ddefault " " tuned " "

		if (ddefault) #there is a default solution
			{
			solvedbydefault++;
			if(tuned)
				ratio=qualityddefault/qualitytuned;
			else
				{ratio=0; qualitytuned=-1;}
			averagequalityratio+=ratio;
			number++;
			}
		else
			{
			qualityddefault=-1;
			ratio=-1;
			}
		if(tuned) #there is solution with tuned
			solvedbytuned++;

		
		printf instance " " qualityddefault " " qualitytuned " ";
		print ratio " ";

print "number of comparable instances " number;

print "average quality ratio " averagequalityratio/number;

print "instances solved by default " solvedbydefault;

print "instances solved by tuning " solvedbytuned;

print "instances solved by tuning but not by default " solvedbytuningandnotbydefault;

         
}
