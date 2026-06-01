CXX := g++
CXXFLAGS := -Wno-narrowing -fPIC
LIBS := -lmpfr -lgmp -lraylib -ldl

BUILD_DIR := build

TARGET := $(BUILD_DIR)/main
LIB_TARGETS := $(BUILD_DIR)/funcs/func.so $(BUILD_DIR)/funcs/func1.so $(BUILD_DIR)/funcs/func2.so $(BUILD_DIR)/funcs/func3.so

SRCS := main.cpp \
		math/IntervalUtils.cpp \
		math/Newton.cpp \
		ui/Button.cpp \
		ui/InputBox.cpp \
		ui/OutputBox.cpp

all: $(TARGET) $(LIB_TARGETS)

$(TARGET): $(addprefix $(BUILD_DIR)/, $(SRCS:.cpp=.o))
	@mkdir -p $(dir $@)
	$(CXX) $^ -o $@ $(LIBS)

$(BUILD_DIR)/funcs/%.so: $(BUILD_DIR)/funcs/%.o
	@mkdir -p $(dir $@)
	$(CXX) -shared -o $@ $< $(LIBS)

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -r $(BUILD_DIR)

.PHONY: all clean
