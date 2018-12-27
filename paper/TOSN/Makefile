BUILD_DIR   := build
FIG_DIR     := figures
TEXFIG_DIR  := tex-figures
DOC_CLASS   := jpaper
MAIN        := paper

TEXFIGS     := $(wildcard $(TEXFIG_DIR)/*.tex)
TEXFIGS_PDF := $(patsubst $(TEXFIG_DIR)/%,$(FIG_DIR)/%,$(TEXFIGS:.tex=.pdf))
FIGS_PDF    := $(filter-out $(TEXFIGS_PDF),$(wildcard $(FIG_DIR)/*.pdf))

.PHONY: dir bw clean

all: dir $(TEXFIGS_PDF) $(FIGS_PDF) $(DOC_CLASS).cls $(MAIN).tex
	TEXMFOUTPUT="$(BUILD_DIR)"
	pdflatex -output-directory=$(BUILD_DIR) $(MAIN)
	bibtex $(BUILD_DIR)/$(MAIN)
	pdflatex -output-directory=$(BUILD_DIR) $(MAIN)
	pdflatex -output-directory=$(BUILD_DIR) $(MAIN)
	mv $(BUILD_DIR)/$(MAIN).pdf $(MAIN).pdf
	@ echo "PDF produced: $(MAIN).pdf"

dir:
	mkdir -p $(BUILD_DIR)

$(FIG_DIR)/%.pdf: $(TEXFIG_DIR)/%.tex
	pdflatex -output-directory=$(BUILD_DIR) $<
	mv $(BUILD_DIR)/$*.pdf $@

bw: all
	@ echo "Converting to B/W..."
	@ gs \
	-sOutputFile=$(MAIN)-bw.pdf \
	-sDEVICE=pdfwrite \
	-sColorConversionStrategy=Gray \
	-dProcessColorModel=/DeviceGray \
	-dCompatibilityLevel=1.4 \
	-dNOPAUSE \
	-dBATCH \
	-dAutoRotatePages=/None \
	$(MAIN).pdf

clean:
	- rm -rf $(BUILD_DIR)
	- rm $(TEXFIGS_PDF)
	- rm $(MAIN)*.pdf
