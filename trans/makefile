#
# Operating System and Developing system specific macros
#

# target library file full name
TRGTLIB = libosiptrans.a

#
# Target dependent macros
#

MODULES = OSIP_TransManager OSIP_TransactionSM OSIP_Transaction \
	    OSIP_Event OSIP_Dialog OSIP_Core OSIP_Transport

BUILD_RELEASE = 2
include ../makefile.inc

$(TRGTLIB) : $(OBJECTS)
	$(AR) -r $(TRGTLIB) $(OBJECTS)

clean:
	$(RM) -f $(OBJECTS)
	$(RM) -f $(TRGTLIB)
