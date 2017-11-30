#$ENV{TEXINPUTS}='./:./chapters/:./template/:$ENV{TEXINPUTS}:';
#$ENV{BIBINPUTS}='./:./bib/:$ENV{BIBINPUTS}:';
#$ENV{BSTINPUTS}='./:./bib/:$ENV{BSTINPUTS}:';

$out_dir="out";

$pdflatex="lualatex -synctex=1 %O %S; ctags --fields=+l -R *.tex *.bib";
$preview_continuous_mode=1;
$dvi_mode=0;
$pdf_mode=1;
$force_mode=1;

$bibtex_silent_switch=1;
$biber_silent_switch=1;
$dvipdf_silent_switch=1;
$dvips_silent_switch=1;
$latex_silent_switch=1;
$makeindex_silent_switch=1;
$pdflatex_silent_switch=1;

