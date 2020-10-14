                AREA    RESET, Code, READONLY,ALIGN=4
								IMPORT InitApp
								IMPORT |Load$$ER_IROM1$$Limit|
								IMPORT |Load$$RW_IRAM1$$Limit|
								DCD InitApp
								DCD |Load$$ER_IROM1$$Limit|	;RO Code大小,用于确定程序大小,用于确定Hex二进制代码的RAM初始化位置
								DCD |Load$$RW_IRAM1$$Limit|	;RW结束位置,用于确定Hex二进制代码的RAM结束位置
								DCD 365											;标志单片机中已经存在从的Dll
								END

									 
									 
									 
									 