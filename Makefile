TARGET = petri-net-ed
LOGIN = xhencer00-xvargas00

.PHONY: all run doxygen clean pack

all:
	$(MAKE) -C src TARGET=$(TARGET)

run: all
	./$(TARGET)

doxygen:
	@mkdir -p doc
	doxygen Doxyfile

clean:
	rm -rf build
	rm -f $(TARGET)
	rm -rf doc
	rm -f $(LOGIN).zip

pack: clean
	zip -r $(LOGIN).zip src/ third_party/ res/ /examples CMakeLists.txt Makefile Doxyfile NOTICE LICENCE