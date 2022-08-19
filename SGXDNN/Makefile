App_Cpp_Files := sgxdnn_main.cpp json11.cpp aes-stream.cpp Crypto.cpp
App_Cpp_Objects := $(App_Cpp_Files:%.cpp=bin/%.o)
App_Cpp_Include_Paths := -I../Include/eigen -I../Include/simde -I../Include/AArch64cryptolib -I../Include -Icrypto
App_C_Flags := -fPIC $(App_Cpp_Include_Paths) -O3 -Wno-enum-compare -fopenmp -DSIMDE_ENABLE_NATIVE_ALIASES
App_C_Flags += -march=armv8-a+simd+crypto
App_Cpp_Flags := -std=c++11 $(App_C_Flags)


Lib_Path := ../lib

DEPDIR := .d
$(shell mkdir -p $(DEPDIR) >/dev/null)
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td

COMPILE.c = $(CC) $(DEPFLAGS) $(App_C_Flags) -c
COMPILE.cc = $(CXX) $(DEPFLAGS) $(App_Cpp_Flags) -c
OUTPUT_OPTION = -o $@ -pthread
POSTCOMPILE = @mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d && touch $@

all: $(Lib_Path)/sgxdnn.so

bin/%.o : %.cpp $(DEPDIR)/%.d
	$(COMPILE.cc) $< $(OUTPUT_OPTION)
	$(POSTCOMPILE)

bin/%.o: %.S
	$(CXX) -fPIC -c $< -o $@
	@echo "CC   <=  $<"

$(Lib_Path)/sgxdnn.so: $(App_Cpp_Objects)
	$(CXX) -fPIC -shared -o $@ $^ $(LDFLAGS) -pthread -fopenmp
	@echo "LINK =>  $@"


App_Ta_Cpp_Files := sgxdnn_main.cpp json11.cpp aes-stream.cpp Crypto.cpp optee-stub.cpp
App_Ta_Cpp_Objects := $(App_Ta_Cpp_Files:%.cpp=bin_ta/%.o)
App_Ta_C_Flags := -fPIC $(App_Cpp_Include_Paths) -O3 -Wno-enum-compare -DSIMDE_ENABLE_NATIVE_ALIASES
App_Ta_C_Flags += -march=armv8-a+simd+crypto
App_Ta_C_Flags += -fno-stack-protector	# fix undefined reference to __stack_chk_guard
App_Ta_C_Flags += -D_GLIBCXX_USE_CXX11_ABI=0	# fix undefined reference to std::__cxx11::*
App_Ta_C_Flags += -DUSE_SGX=1	# define USE_SGX, though we are using TrustZone actually
App_Ta_Cpp_Flags := -std=c++11 $(App_Ta_C_Flags)

COMPILE_TA.c = $(CC) $(DEPFLAGS) $(App_Ta_C_Flags) -c
COMPILE_TA.cc = $(CXX) $(DEPFLAGS) $(App_Ta_Cpp_Flags) -c
OUTPUT_OPTION_TA = -o $@

static: bin_ta/sgxdnn.a

bin_ta/%.o : %.cpp $(DEPDIR)/%.d
	$(COMPILE_TA.cc) $< -fno-exceptions $(OUTPUT_OPTION_TA)
	$(POSTCOMPILE)

bin_ta/sgxdnn.a: $(App_Ta_Cpp_Objects)
	$(AR) rcs $@ $^

clean:
	rm -rf $(Lib_Path)/sgxdnn.so $(App_Cpp_Objects) bin_ta/sgxdnn.a $(App_Ta_Cpp_Objects)


$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d

include $(wildcard $(patsubst %,$(DEPDIR)/%.d,$(basename $(App_Cpp_Files))))
