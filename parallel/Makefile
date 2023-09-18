CC = g++
BUILD_DIR = build
SRC_DIR = src
CFLAGS = -std=c++11 -pthread

IMAGE_FILTER_EXECUTABLE_FILE = ImageFilters.out

MAIN_IMAGE_FILTER = \
	$(BUILD_DIR)/main.o

IMAGE_FILTER_OBJECTS = \
	$(BUILD_DIR)/main.o

all: $(BUILD_DIR) $(IMAGE_FILTER_EXECUTABLE_FILE)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp -o $(BUILD_DIR)/main.o

$(IMAGE_FILTER_EXECUTABLE_FILE): $(IMAGE_FILTER_OBJECTS)
	$(CC) $(CFLAGS) $(IMAGE_FILTER_OBJECTS) -o $(IMAGE_FILTER_EXECUTABLE_FILE)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) *.o *.out