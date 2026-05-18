TARGET = petri-net-ed
LOGIN = xhencer00

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
	rm -f $(LOGIN).tgz

pack: clean
	tar -czf $(LOGIN).tgz src/ third_party/ res/ CMakeLists.txt Makefile Doxyfile