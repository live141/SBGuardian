default: all

all: goldsrc goldsrc-standalone src-original src-css src-csgo src-orangebox src-orangeboxvalve

src-original:
	@make -f Makefile.src ENGINE=original
src-css:
	@make -f Makefile.src ENGINE=css
src-csgo:
	@make -f Makefile.src ENGINE=csgo
src-orangebox:
	@make -f Makefile.src ENGINE=orangebox
src-orangeboxvalve:
	@make -f Makefile.src ENGINE=orangeboxvalve
goldsrc:
	@make -f Makefile.goldsrc
goldsrc-standalone:
	@make -f Makefile.goldsrc STANDALONE=true
clean:
	@make -f Makefile.goldsrc clean
	@make -f Makefile.goldsrc clean STANDALONE=true
	@make -f Makefile.src clean ENGINE=original
	@make -f Makefile.src clean ENGINE=css
	@make -f Makefile.src clean ENGINE=csgo
	@make -f Makefile.src clean ENGINE=orangeboxvalve
	@make -f Makefile.src clean ENGINE=orangebox
