#rm ipc2011.pdf

pdflatex ipc2011
bibtex ipc2011
bibtex ipc2011

#sed -i -e 's/citation{ /citation{/g' ipc2011.aux

pdflatex ipc2011
#pdflatex ipc2011
#pdflatex ipc2011
#pdflatex ipc2011

if [ $1 -gt 0 ]; then
acroread ipc2011.pdf
fi
