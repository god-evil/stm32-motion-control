                AREA    RESET, Code, READONLY,ALIGN=4
								IMPORT InitApp
								IMPORT |Load$$ER_IROM1$$Limit|
								IMPORT |Load$$RW_IRAM1$$Limit|
								DCD InitApp
								DCD |Load$$ER_IROM1$$Limit|	;RO Code��С,����ȷ�������С,����ȷ��Hex�����ƴ����RAM��ʼ��λ��
								DCD |Load$$RW_IRAM1$$Limit|	;RW����λ��,����ȷ��Hex�����ƴ����RAM����λ��
								DCD 365											;��־��Ƭ�����Ѿ����ڴӵ�Dll
								END

									 
									 
									 
									 