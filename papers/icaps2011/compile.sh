paper=icaps2011_cameraready

rm  ${paper}.pdf

pdflatex  ${paper}
#bibtex  gecco2011brendelschoenauerworkshop
bibtex  ${paper}


#pdflatex  ${paper}


if [ $1 -gt 0 ]; then
acroread ${paper}.pdf
fi
