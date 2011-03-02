function median(c,v,  j) { 
           asort(v,j); 
           if (c % 2) return j[(c+1)/2]; 
           else return int((j[c/2+1]+j[c/2])/2.0); 
        } 
BEGIN{instance=0;}

{
if (instance == 0)
    {
    instance=$2;
    domain=$1;
    }

if ($2 != instance)
			{ 
           		print  domain " " instance " " $3 " "  median(count,genvalues) " " median(count,evalvalues);
           		count=0; 
           		domain=$1;
           		instance=$2;
         		}
         		
count++; genvalues[count]=$4; evalvalues[count]=$5;

} 
END { 
print  domain " " instance " " $3 " "  median(count,genvalues) " " median(count,evalvalues);
}
