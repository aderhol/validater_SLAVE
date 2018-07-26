# invoke SourceDir generated makefile for uartecho.pem4f
uartecho.pem4f: .libraries,uartecho.pem4f
.libraries,uartecho.pem4f: package/cfg/uartecho_pem4f.xdl
	$(MAKE) -f C:\Users\Adam\workspace_v8\validater_SLAVE/src/makefile.libs

clean::
	$(MAKE) -f C:\Users\Adam\workspace_v8\validater_SLAVE/src/makefile.libs clean

