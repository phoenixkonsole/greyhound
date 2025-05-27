#include "testament.h"

const char * const greyhound_version = "3.3 (Dev"
#if defined(CI_BUILD)
	" CI #" CI_BUILD
#endif
	")"
	;
const int greyhound_version_major = 3;
const int greyhound_version_minor = 3;
