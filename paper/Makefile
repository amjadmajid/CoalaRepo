all: figs
	pdflatex paper.tex
	bibtex paper
	pdflatex paper.tex
	pdflatex paper.tex

figs:
	pdflatex texFigCode/crc_example.tex
	pdflatex texFigCode/dynamic-chain-2.tex
	pdflatex texFigCode/taskification_example.tex
	pdflatex texFigCode/crc_example_war.tex
	pdflatex texFigCode/dynamic-chain.tex
	cp texFigCode/crc_example.pdf figures/
	cp texFigCode/dynamic-chain-2.pdf figures/
	cp texFigCode/taskification_example.pdf figures/
	cp texFigCode/crc_example_war.pdf figures/
	cp texFigCode/dynamic-chain.pdf figures/

clean:
	-rm paper.aux paper.bbl paper.blg paper.log paper.out paper.pdf
