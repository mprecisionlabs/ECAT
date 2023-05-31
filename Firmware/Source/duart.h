void init_duart();
int DUART_A_CHAR_AVAIL(void);
int DUART_B_CHAR_AVAIL(void);
void DUART_SEND_A(unsigned char c);
void DUART_SEND_B(unsigned char c);
unsigned int DUART_GET_A(void);
unsigned int DUART_GET_B(void);
void DUART_SET_OUTPUT(unsigned char c);
unsigned char DUART_GET_INPUT(void);
unsigned short DUART_GET_SRA(void);
unsigned short DUART_GET_SRB(void);

