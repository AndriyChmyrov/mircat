#ifndef NDEBUG
#define DEBUG(text) mexPrintf("%s:%d - %s\n",__FILE__,__LINE__,text);	// driver actions

#define MEXMESSAGE(error) mexMessage(__FILE__,__LINE__,error)
#define MEXENTER mexEnter(__FILE__,__LINE__)
#else
#ifdef DEBUG
#undef DEBUG
#endif
#define DEBUG(text)

#define MEXMESSAGE(error) mexMessage(error)
#define MEXENTER mexEnter()
#endif
#define MEXLEAVE mexLeave()

const WORD StringLength = 256;

extern int laser;		// lasers

#ifndef NDEBUG
void mexEnter(const char* file, const int line);
void mexMessage(const char* file, const int line, uint32_t error);
#else
void mexEnter(void);
void mexMessage(uint32_t error);
#endif
void mexLeave(void);
void mexCleanup(void);

double	getScalar(const mxArray* array);
mxArray*	getParameter(const char* name);
void	setParameter(const char* name, const mxArray* field);
