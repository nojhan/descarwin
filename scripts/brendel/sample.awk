{ 
if (( rand() > 0.2 ))
    print $0 > "train.txt"
else
    print $0 > "test.txt"


} 
