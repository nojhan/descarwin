BEGIN{bestmakespan=0; numbests=0;}
{

if( $1 != "id" && (bestmakespan==0 || $2 < bestmakespan))
    {
    bestmakespan=$2;
    bestparameters[0]=$0;
    numbests=1;}

else if( $1 != "id" &&  $2 == bestmakespan)
    {
    bestparameters[numbests]=$0;
    numbests++;
    }


} 
END{
for (i=0;i<numbests;i++)
    print bestparameters[i];
}
