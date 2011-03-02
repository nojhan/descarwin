BEGIN{bestmakespan=0; bestparameters=""}
{
if( bestmakespan==0 || $2 < bestmakespan)
    {
    bestmakespan=$2;
    bestparameters=$0;
    }


} 
END{print bestparameters}
