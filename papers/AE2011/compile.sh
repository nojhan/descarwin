papername=ae2011

rm  ${papername}.pdf

pdflatex  ${papername}
bibtex  ${papername}

pdflatex ${papername}


if [ $1 -gt 0 ]; then
acroread ${papername}.pdf
fi
