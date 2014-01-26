# LaTeX tools
export DOC_SCRIPTS:=$(RGSRC)/pkg/doc/scripts/
export PATH:=/usr/local/openrg/bin:$(PATH)
export TEXMFCNF TEXMF
export LOCAL_PNG_PATH=/usr/local/share/lib/latex2html/icons/
JPEG2PS=jpeg2ps
PS2EPS=ps2eps
RUN_LATEX=$(DOC_SCRIPTS)runlatex.pl
STRIP_LATEX=$(DOC_SCRIPTS)strip_latex
CONF_FILES=$(RGSRC)/linux/.config $(BUILDDIR)/.deliver
PDFLATEX=pdflatex
DOCS_DIR=$(RGSRC)/pkg/doc

# latex variables

PS_SRC:=$(WORD_SRC:%.doc=%.ps) $(PPT_SRC:%.ppt=%.ps) $(VISIO_SRC:%.vsd=%.ps)
PS_EPS:=$(PS_SRC:%.ps=%.eps)
FH_EPS:=$(FH_SRC:%.fh8=%.eps)
FH_PNG:=$(FH_SRC:%.fh8=%.png)

JPEG_EPS:=$(JPEG_SRC:%.jpeg=%.eps)
BMP_EPS:=$(BMP_SRC:%.bmp=%.eps)
BMP_PNG:=$(BMP_SRC:%.bmp=%.png)
PNG_EPS:=$(PNG_SRC:%.png=%.eps)
IMAGES_EPS:=$(JPEG_EPS) $(BMP_EPS) $(PS_EPS) $(PNG_EPS)
IMAGES_PNG:=$(BMP_PNG) $(FH_PNG)

# List of tex files to produce
TEX_TARGET+=$(TEX_SRC)
TEX_TARGETS=$(TEX_TARGET:%.tex=%.pdf) $(TEX_TARGET:%.tex=%.ps) \
	$(TEX_HTML) $(TEX_SLIDE)

# Targets are also sources
TEX_SOURCE+=$(TEX_SRC) $(TEX_SRC1)
TEX_ALLSRC=$(TEX_PRINT) $(TEX_ONLINE)
TEX_PS:=$(TEX_SOURCE:%.tex=%.ps)
TEX_PDF:=$(TEX_SOURCE:%.tex=%.pdf)
TEX_HTML:=$(TEX_SOURCE:%.tex=html_%)
TEX_PRINT=$(patsubst %.tex,%_print.tex,$(TEX_SOURCE))
TEX_ONLINE=$(patsubst %.tex,%_online.tex,$(TEX_SOURCE))
TEX_PRINT_PS=$(patsubst %.tex,%_print.ps,$(TEX_SOURCE))
TEX_ONLINE_PS=$(patsubst %.tex,%_online.ps,$(TEX_SOURCE))
TEX_HTML_DVI=$(patsubst %,%.dvi,$(TEX_HTML))
TEX_PRINT_DVI=$(patsubst %.tex,%_print.dvi,$(TEX_SOURCE))
TEX_ONLINE_DVI=$(patsubst %.tex,%_online.dvi,$(TEX_SOURCE))
TEX_PRINT_PDF=$(patsubst %.tex,%_print.pdf,$(TEX_SOURCE))
TEX_LINKS=$(patsubst %,$(RGSRC)/pkg/doc/user_manual/%,$(UM_TEX_LINK))
IMG_LINKS+=$(patsubst %,$(RGSRC)/pkg/doc/user_manual/images/%,$(UM_IMG_LINK))
ONLINE_AUX=$(TEX_SOURCE:%.tex=%_online.aux)
TEX_INCLUDE+=version.tex definitions.tex
LATEX_DEP=$(IMAGES_EPS) $(FH_EPS) $(TEX_INCLUDE) $(CONF_FILES)
IFDEFS_FILE=run_time_ifdefs.tex
	
ifdef CONFIG_LSP_DIST
  LSP_TYPE=\\LSPtrue
else
  LSP_TYPE=\\LSPfalse
endif

ifdef CONFIG_RG_TUTORIAL
  TUTORIAL=\\TUTORIALtrue
else
  TUTORIAL=\\TUTORIALfalse
endif

CREATE_IFDEFS_FILE=$(shell echo "%% This is an auto-generated file by docs.mak" > $(IFDEFS_FILE)) $(shell echo $1 >> $(IFDEFS_FILE)) $(shell echo $2 >> $(IFDEFS_FILE)) $(shell echo $(LSP_TYPE) >> $(IFDEFS_FILE)) $(shell echo $(TUTORIAL) >> $(IFDEFS_FILE))

ifneq ($(UM_IMG_LINK)$(IMG_LINKS),)
  ifneq ($(IS_DISTRIBUTION),y)
    ARCHCONFIG_FIRST_TASKS+=do_um_img_links
  endif
endif
ifneq ($(UM_TEX_LINK)$(TEX_LINKS),)
  ARCHCONFIG_FIRST_TASKS+=do_um_tex_links
endif

# Add to clean target all documentation related files
DOC_CLEAN+=texput.log \
	$(IMAGES_EPS) $(IMAGES_PNG) \
	$(TEX_PDF) $(TEX_PS) $(TEX_DVI) \
	$(TEX_HTML:%=%.{tex,aux,lof,log,out,toc,ps,dvi}) \
	$(TEX_ALLSRC:%.tex=%.{tex,aux,lof,log,out,toc,ps,pdf,dvi}) \
	$(TEX_SLIDE:%.pdf=%.{pdf,out,aux,log}) \
 	$(PS_EPS:%.eps=%.png) $(IFDEFS_FILE)

ifneq ($(TEX_SOURCE)$(TEX_SLIDE),)
  CLEAN+=$(DOC_CLEAN) version.tex definitions.tex *.glo *.ilg *.gls *.aux *.bak
endif

CLEAN+=$(TEX_HTML)

docs: $(DOCS_SUBDIRS_) $(TEX_TARGETS)

docs_print: $(TEX_PRINT_PDF)

# latex source code compilation

$(JPEG_EPS) : %.eps : %.jpeg
	$(JPEG2PS) -o $@ $<

$(BMP_EPS) : %.eps : %.bmp
	convert $< $@

$(BMP_PNG) : %.png : %.bmp
	convert $< $@

$(PNG_EPS) : %.eps : %.png
	convert $< $@

$(FH_PNG) : %.png : %.eps
	convert $< $@

$(PS_EPS) : %.eps : %.ps
	$(PS2EPS) < $< > $@

$(TEX_ONLINE_DVI) : %.dvi : %.tex $(LATEX_DEP)
	$(call CREATE_IFDEFS_FILE,\\PRINTfalse,\\HTMLDOCfalse)
	$(RUN_LATEX) $<

$(TEX_PRINT_DVI) : %.dvi : %.tex $(LATEX_DEP)
	$(call CREATE_IFDEFS_FILE,\\PRINTtrue,\\HTMLDOCfalse)
	$(RUN_LATEX) $<

$(TEX_HTML_DVI) : %.dvi : %.tex $(LATEX_DEP)
	$(RUN_LATEX) $<
	
$(TEX_ONLINE_PS) $(TEX_PRINT_PS) : %.ps : %.dvi
	dvips -Pcmz -Pamz -j0 -o $@ $<

$(TEX_PRINT) : %_print.tex : %.tex
	cp $< $@

$(TEX_ONLINE) : %_online.tex : %.tex
	cp $< $@

$(TEX_PDF) : %.pdf : %_online.ps
	ps2pdf -dMaxSubsetPct=100 -dEmbedAllFonts=true -dSubsetFonts=true -dAutoFilterColorImages=false -dAutoFilterGrayImages=false -dColorImageFilter=/FlateEncode -dGrayImageFilter=/FlateEncode -dModoImageFilter=/FlateEncode $< $@

$(TEX_PRINT_PDF) : %.pdf : %.ps
	ps2pdf -dMaxSubsetPct=100 -dEmbedAllFonts=true -dSubsetFonts=true -dAutoFilterColorImages=false -dAutoFilterGrayImages=false -dColorImageFilter=/FlateEncode -dGrayImageFilter=/FlateEncode -dModoImageFilter=/FlateEncode $< $@

$(TEX_PS) : %.ps : %_print.ps
	cp $< $@

# Making sure we have the correct version of latex2html
L2H_JUNGO_VERSION=Jungo_Patch_1.2
L2H_VERSION=$(shell latex2html --version)
IS_JUNGO_VERSION=$(filter $(L2H_JUNGO_VERSION),$(L2H_VERSION))

html_%.tex:
	cp $(@:html_%=%) $@
	$(call CREATE_IFDEFS_FILE,\\PRINTfalse,\\HTMLDOCtrue)
	texexpand $@ > $@.expand
	$(STRIP_LATEX) $@.expand > $@.stripped
	sed -e 's/[$$]\(.\)[$$]/\1/g' $@.stripped | \
	sed -e 's/[\\]dag//g' > $@
	@rm -f $@.expand $@.stripped

$(TEX_HTML): html_%: html_%.tex html_%.dvi $(if $(IS_JUNGO_VERSION),,install_latex2html)
	latex2html -address="<a href=\"http://www.jungo.com\">Jungo Software Technologies</a>" $<
	cp -a $(LOCAL_PNG_PATH) $@/
	for f in `ls $@/*.html` ; do \
	  cat $$f | sed -e 's/file:.usr.local.share.lib.latex2html.//g' > \
	  $$f.tmp;  mv -f $$f.tmp $$f; \
	done

$(TEX_SLIDE) : %.pdf : %.tex
	$(PDFLATEX) $< 
	$(PDFLATEX) $< 
	$(PDFLATEX) $< 

$(CONF_FILES):
	@touch $(BUILDDIR)/.deliver

install_latex2html:
	@echo "*** ERROR: latex2html (jungo version) is not installed !!!"
	@echo "*** Please Do: rt jpkg update latex2html"
	@false

version.tex: $(RGSRC)/pkg/include/rg_version_data.h
	$(RGSRC)/pkg/doc/scripts/version.awk < $< > $@

do_um_img_links: 
	$(RG_LN) $(IMG_LINKS) ./images

do_um_tex_links:
	$(RG_LN) $(TEX_LINKS) ./

definitions.tex: 
	$(RG_LN) $(RGSRC)/pkg/doc/scripts/definitions.tex definitions.tex

.PHONY: install_latex2html

