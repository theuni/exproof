CXX         = c++
AR			= ar
ARFLAGS		= -cr
CXXFLAGS    = -O2 -g -Wall -Wextra -Wno-unused-variable
LDFLAGS     =

OBJS  += $(GENCONTEXT_OBJS)
PROGS += $(GENCONTEXT_PROG)

EXPROOF_OBJS  =
EXPROOF_OBJS += main.o
EXPROOF_OBJS += exproof.o
EXPROOF_OBJS += crypto/sha256.o
EXPROOF_OBJS += crypto/sha256_avx2.o
EXPROOF_OBJS += crypto/sha256_shani.o
EXPROOF_OBJS += crypto/sha256_sse4.o
EXPROOF_OBJS += crypto/sha256_sse41.o
EXPROOF_PROG = exproof


OBJS  += $(EXPROOF_OBJS)
PROGS += $(EXPROOF_PROG)


CXXFLAGS_INT = -std=c++17
CPPFLAGS_INT = -I.
LDFLAGS_INT  = -pthread

SSE41_CXXFLAGS = -msse4.1
AVX2_CXXFLAGS = -mavx -mavx2
SHANI_CXXFLAGS = -msse4 -msha

crypto/sha256_avx2.o: CXXFLAGS_INT += $(AVX2_CXXFLAGS)
crypto/sha256_avx2.o: CPPFLAGS_INT += -DENABLE_AVX2

crypto/sha256_shani.o: CXXFLAGS_INT += $(SHANI_CXXFLAGS)
crypto/sha256_shani.o: CPPFLAGS_INT += -DENABLE_SHANI

crypto/sha256_sse41.o: CXXFLAGS_INT += $(SSE41_CXXFLAGS)
crypto/sha256_sse41.o: CPPFLAGS_INT += -DENABLE_SSE41

crypto/sha256.o: CPPFLAGS_INT += -DENABLE_AVX2 -DENABLE_SHANI -DENABLE_SSE41 -DUSE_ASM

all: $(PROGS)

V=
_notat_=@
_notat_0=$(_notat_)
_notat_1=@\#
_at_=@
_at_0=$(_at_)
_at_1=
at = $(_at_$(V))
notat = $(_notat_$(V))

DEPDIR=.deps
DEPS = $(addprefix $(DEPDIR)/,$(OBJS:.o=.o.Tpo))
DIRS = $(dir $(DEPS))
DEPDIRSTAMP=$(DEPDIR)/.stamp


-include $(DEPS)

$(DEPDIRSTAMP):
	@mkdir -p $(dir $(DEPS))
	@touch $@

$(OBJS): | $(DEPDIRSTAMP)

$(EXPROOF_PROG): $(EXPROOF_OBJS)
	$(notat)echo LINK $@
	$(at)$(CXX) $(CXXFLAGS_INT) $(CXXFLAGS) $(LDFLAGS_INT) $(LDFLAGS) $^ -o $@

%.o: %.cpp
	$(notat)echo CXX $<
	$(at)$(CXX) $(CPPFLAGS_INT) $(CPPFLAGS) $(CXXFLAGS_INT) $(CXXFLAGS) $(CXXFLAGS_EXTRA) -c -MMD -MP -MF .deps/$@.Tpo $< -o $@

crypto/%.o: crypto/%.cpp
	$(notat)echo CXX $<
	$(at)$(CXX) $(CPPFLAGS_INT) $(CPPFLAGS) $(CXXFLAGS_INT) $(CXXFLAGS) -c -MMD -MP -MF .deps/$(@F).Tpo $< -o $@

clean:
	-rm -f $(PROGS)
	-rm -f $(OBJS)
	-rm -f $(DEPS)
	-rm -rf $(DEPDIR)
