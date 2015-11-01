build:
	$(MAKE) -C src

run: build
	xterm -e ./excss-configurator
