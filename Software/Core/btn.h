#ifndef TOGLEBTN
#define TOGLEBTN

class TOGLE_BTN
{
private:

	HWND parent;
	HWND hBtn;
	int  id;
	int  id_title;

	unsigned int  state;
	unsigned int  states;

	const char **names;

	TOGLE_BTN(TOGLE_BTN &){}									 // disable this public call
	TOGLE_BTN & operator = (TOGLE_BTN &) {return *this;}// disable this public call

	BOOL SetState(unsigned int s);
	BOOL SetState(const char *s);
	unsigned int  GetState() const {return state;}

	unsigned int NextState();


public:

 	TOGLE_BTN(){parent = 0; hBtn = 0; state = 0; states = 0; names = 0;}
	~TOGLE_BTN(){};

	BOOL Create(HWND hp,int res_id,const char **n,int tid = 0);
	BOOL Show(BOOL s);
	BOOL Enable(BOOL s);
	BOOL GetText(char *s)const;

	TOGLE_BTN & operator = (unsigned int s) {SetState(s); return *this;}
	TOGLE_BTN & operator = (const char *s)  {SetState(s); return *this;}

	operator unsigned int() {return GetState();}

	BOOL operator == (unsigned int s) {return (s == state);}
	BOOL operator != (unsigned int s) {return (s != state);}
	TOGLE_BTN & operator ++(){NextState(); return *this;}		//pre
	unsigned int operator ++(int){unsigned s = state; NextState(); return s;}	//post

};
#endif
