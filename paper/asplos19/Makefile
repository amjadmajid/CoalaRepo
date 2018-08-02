
FIGS = SRC = $(wildcard figures/*.tex) 
all: figs
	pdflatex paper.tex
	bibtex paper
	pdflatex paper.tex
	pdflatex paper.tex

figs:
	cd texFigCode; \
	for i in ./*.tex; do echo "Building $$i"; pdflatex $$i; done; \
	cp *.pdf ../figures/

clean:
	-rm paper.aux paper.bbl paper.blg paper.log paper.out paper.pdf


#	pdflatex crc_example.tex; \
#	pdflatex dynamic-chain-2.tex; \
#	pdflatex taskification_example.tex; \
#	pdflatex crc_example_war.tex; \
#	pdflatex dynamic-chain.tex; \
#	pdflatex sram-buffer.tex; \
