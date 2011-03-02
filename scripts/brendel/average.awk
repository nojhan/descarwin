
{ 
         count++;average=average+$5;  
         if (count >= 11) 
		{
            	print  $2 " " $4 " "  average/11; count=0; average=0;
		if ($4 == 1.0) print ""
         	} 
} 
END { 
         
}
