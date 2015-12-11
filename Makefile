SRC_C := $(wildcard source/*.c)
SRC_CPP := $(wildcard source/*.cpp)
OBJS := $(addprefix build/, $(notdir $(SRC_C:%.c=%.o))) $(addprefix build/, $(notdir $(SRC_CPP:%.cpp=%.o)))
TARGET := game.exe
CFLAGS := -Iinclude -Wall
CPPFLAGS := -Iinclude -Wall -std=c++11
LDLIBS = -lfmod -lm -lglfw3 -lglew32s -lglu32 -lopengl32 -lmingw32 -lgdi32 -luser32 -lkernel32

$(TARGET): build/dirs $(OBJS)
	g++ $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

run: $(TARGET)
	./$(TARGET)

-include $(OBJS:.o=.o.d)

build/%.o: source/%.c build/dirs
	gcc -c $(CFLAGS) $< -o $@
	@gcc -MM $(CFLAGS) $< > $@.d
	@cp -f $@.d $@.d.tmp
	@sed -e 's/.*://' -e 's/\\$$//' < $@.d.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> $@.d
	@rm -f $@.d.tmp

build/%.o: source/%.cpp build/dirs
	g++ -c $(CPPFLAGS) $< -o $@
	@g++ -MM $(CPPFLAGS) $< > $@.d
	@cp -f $@.d $@.d.tmp
	@sed -e 's/.*://' -e 's/\\$$//' < $@.d.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> $@.d
	@rm -f $@.d.tmp

clean:
	@rm -rf build
	@rm -f $(TARGET)
	@rm -f build/dirs

build/dirs:
	@mkdir -p build
	@touch build/dirs
