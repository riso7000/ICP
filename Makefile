# Name of your final executable
TARGET = petri-net-ed
# Your university login/ID for packing
LOGIN = xhencer00

.PHONY: all run doxygen clean pack

# Default target: calls make inside the src directory
all:
	$(MAKE) -C src TARGET=$(TARGET)

# Runs the compiled application
run: all
	./$(TARGET)

# Generates Doxygen documentation into /doc
doxygen:
	@mkdir -p doc
	doxygen Doxyfile

# Cleans up all compilation artifacts and documentation
clean:
	rm -rf build
	rm -f $(TARGET)
	rm -rf doc
	rm -f $(LOGIN).tgz
	$(MAKE) -C src clean

# Packs the assignment into the required archive format
pack: clean
	tar -czf $(LOGIN).tgz src/ third_party/ res/ CMakeLists.txt Makefile Doxyfile