default: all

all: goldsrc goldsrc-standalone src

src:
	@make -f Makefile.src
goldsrc:
	@make -f Makefile.goldsrc
goldsrc-standalone:
	@make -f Makefile.goldsrc STANDALONE=true
clean:
	@make -f Makefile.goldsrc clean
	@make -f Makefile.goldsrc clean STANDALONE=true
	@make -f Makefile.src clean
