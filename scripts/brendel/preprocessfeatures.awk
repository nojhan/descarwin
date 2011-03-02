{ 
if (( $1 == "features"))
    {
#           		printf $2 " "
           		for (i=1; i<=NF; i++) 
	           		if (( $i + 0 ) == $i ) printf $i " "
	           	print " "
    }
} 
