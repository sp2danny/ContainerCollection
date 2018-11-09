
#include "asyn_kb.h"

extern void testsuit();
extern void testsuit_old();

int main()
{
	testsuit();
	AsynKB::WaitChar();
}
