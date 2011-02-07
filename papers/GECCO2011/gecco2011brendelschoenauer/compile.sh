rm  gecco2011brendelschoenauer.pdf

pdflatex  gecco2011brendelschoenauer
#bibtex  gecco2011brendelschoenauer
#bibtex sigproc
bibtex  gecco2011brendelschoenauer


#pdflatex  gecco2011brendelschoenauer


if [ $1 -gt 0 ]; then
acroread gecco2011brendelschoenauer.pdf
fi
