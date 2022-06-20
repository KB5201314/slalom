App_Cpp_Files := sgxdnn_main.cpp json11.cpp aes-stream.cpp
App_Cpp_Files += Crypto.cpp
App_Cpp_Objects := $(App_Cpp_Files:%.cpp=bin/%.o)

App_Cpp_Include_Paths := -I../Include/eigen -I../Include/simde -I../Include/AArch64cryptolib -I../Include -Icrypto
App_C_Flags := -fPIC $(App_Cpp_Include_Paths) -O3 -Wno-enum-compare -fopenmp -DSIMDE_ENABLE_NATIVE_ALIASES
App_C_Flags += -march=armv8-a+simd+crypto

App_Cpp_Flags := -std=c++11 $(App_C_Flags)

Lib_Path := ../lib

all: $(Lib_Path)/sgxdnn.so

DEPDIR := .d
$(shell mkdir -p $(DEPDIR) >/dev/null)
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td

COMPILE.c = $(CC) $(DEPFLAGS) $(App_C_Flags) -c
COMPILE.cc = $(CXX) $(DEPFLAGS) $(App_Cpp_Flags) -c
OUTPUT_OPTION = -o $@ -pthread
POSTCOMPILE = @mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d && touch $@

bin/%.o : %.cpp
bin/%.o : %.cpp $(DEPDIR)/%.d
	$(COMPILE.cc) $< $(OUTPUT_OPTION)
	$(POSTCOMPILE)

bin/%.o: %.S
	$(CXX) -fPIC -c $< -o $@
	@echo "CC   <=  $<"

$(Lib_Path)/sgxdnn.so: $(App_Cpp_Objects)
	$(CXX) -fPIC -shared -o $@ $^ $(LDFLAGS) -pthread -fopenmp
	@echo "LINK =>  $@"

clean:
	rm -rf $(Lib_Path)/sgxdnn.so $(App_Cpp_Objects)


$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d

include $(wildcard $(patsubst %,$(DEPDIR)/%.d,$(basename $(App_Cpp_Files))))
