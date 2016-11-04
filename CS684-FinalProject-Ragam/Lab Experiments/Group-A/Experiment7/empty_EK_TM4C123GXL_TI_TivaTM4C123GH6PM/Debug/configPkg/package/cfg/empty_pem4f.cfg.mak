# invoke SourceDir generated makefile for empty.pem4f
empty.pem4f: .libraries,empty.pem4f
.libraries,empty.pem4f: package/cfg/empty_pem4f.xdl
	$(MAKE) -f /home/diptesh/ccs_workspace/empty_EK_TM4C123GXL_TI_TivaTM4C123GH6PM/src/makefile.libs

clean::
	$(MAKE) -f /home/diptesh/ccs_workspace/empty_EK_TM4C123GXL_TI_TivaTM4C123GH6PM/src/makefile.libs clean

