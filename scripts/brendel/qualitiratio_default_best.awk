BEGIN{
	averagequalityratio=0;default=0;tuned=0; instance=-1; number=0;
	print "instancenumber referencemakespan makespan qualityratio"
	}

{ 

#print $54 " " $4 " " instance;

if ($54!="source_of_parameters" && $4!=instance && instance!=-1 && default && $2 < 100000)
		{

		#printf default " " tuned " "

		


		if(tuned)
			ratio=qualitydefault/qualitytuned;
		else
			{ratio=0; qualitytuned=-1;}


		default=0;
		tuned=0;
		
		printf instance " " qualitydefault " " qualitytuned " ";
		print ratio " ";
		averagequalityratio=averagequalityratio+ratio;
		number=number+1;
		#printf averagequalityratio " ";

		instance= $4;

		} # $4

if ($54!="source_of_parameters" && $4!=instance && instance!=-1 &&  tuned && default == 0 && $2 < 100000) #drop

	{
	default=0;
	tuned=0;
	instance= $4;
	
	}

if ( instance == -1 && $54!="source_of_parameters")
	instance = $4;

if( $54 == "initialize_with_default" && $2 < 100000)
	{
	default=1;
	qualitydefault=$2;
	}

#printf $54 " " $4 " "
if( $54 == "Epoch" && $2 < 100000)
	{
	tuned=1;
	qualitytuned=$2;
	}



} 

END { 

if(tuned)
	ratio=qualitydefault/qualitytuned;
else
	{ratio=0; qualitytuned=-1;}
		
if(default)
{
	printf instance " " qualitydefault " " qualitytuned " ";
	print ratio " ";
	averagequalityratio=averagequalityratio+ratio;
	number=number+1;
	}

printf number " ";

print averagequalityratio/number;
         
}
