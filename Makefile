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

#Debug targets
src-original-debug:
	@make -f Makefile.src ENGINE=original DEBUG=true
src-css-debug:
	@make -f Makefile.src ENGINE=css DEBUG=true
src-csgo-debug:
	@make -f Makefile.src ENGINE=csgo DEBUG=true
src-orangebox-debug:
	@make -f Makefile.src ENGINE=orangebox DEBUG=true
src-orangeboxvalve-debug:
	@make -f Makefile.src ENGINE=orangeboxvalve DEBUG=true
goldsrc-debug:
	@make -f Makefile.goldsrc DEBUG=true
goldsrc-standalone-debug:
	@make -f Makefile.goldsrc STANDALONE=true DEBUG=true

#Profile targets
src-original-profile:
	@make -f Makefile.src ENGINE=original PROFILE=true
src-css-profile:
	@make -f Makefile.src ENGINE=css PROFILE=true
src-csgo-profile:
	@make -f Makefile.src ENGINE=csgo PROFILE=true
src-orangebox-profile:
	@make -f Makefile.src ENGINE=orangebox PROFILE=true
src-orangeboxvalve-profile:
	@make -f Makefile.src ENGINE=orangeboxvalve PROFILE=true
goldsrc-profile:
	@make -f Makefile.goldsrc PROFILE=true
goldsrc-standalone-profile:
	@make -f Makefile.goldsrc STANDALONE=true PROFILE=true

clean:
	@make -f Makefile.goldsrc clean
	@make -f Makefile.goldsrc clean STANDALONE=true
	@make -f Makefile.src clean ENGINE=original
	@make -f Makefile.src clean ENGINE=css
	@make -f Makefile.src clean ENGINE=csgo
	@make -f Makefile.src clean ENGINE=orangeboxvalve
	@make -f Makefile.src clean ENGINE=orangebox
