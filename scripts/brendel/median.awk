function median(c,v,  j) { 
           asort(v,j); 
           if (c % 2) return j[(c+1)/2]; 
           else return (j[c/2+1]+j[c/2])/2.0; 
        } 
{ 
         count++;values[count]=$5;  
         if (count >= 11) 
			{ 
           		print  $2 " " $4 " "  median(count,values); count=0; 
			if ($4 == 1.0) print ""
         		}	 
} 
END { 
         
}
