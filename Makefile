CXX = g++
CXX_FLAGS = -I. \
			-Wall \
			-Wextra \
			-std=c++20

CXX_SRC = $(shell find . -type f -name '*.cpp')

T10K_IMAGES = data/t10k-images-idx3-ubyte
T10K_LABELS = data/t10k-labels-idx1-ubyte
TRAIN_IMAGES = data/train-images-idx3-ubyte
TRAIN_LABELS = data/train-labels-idx1-ubyte

BIN = knn

build:
	$(CXX) $(CXX_FLAGS) $(CXX_SRC) -o $(BIN)

run: build
	./$(BIN) $(T10K_IMAGES) $(T10K_LABELS) $(TRAIN_IMAGES) $(TRAIN_LABELS)
