rm  icaps2011.pdf

pdflatex  icaps2011
bibtex  icaps2011
bibtex  icaps2011r


pdflatex  icaps2011


if [ $1 -gt 0 ]; then
acroread icaps2011.pdf
fi
