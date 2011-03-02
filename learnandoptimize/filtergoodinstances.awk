{ 
text  = $2 " " $1

command= "grep \" " text "\" goodlogs.txt | wc -l"


while ( ( command | getline result ) > 0 ) { }

if (result > 0)
    print $0


}
