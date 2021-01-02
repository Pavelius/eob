#include "main.h"

BSDATAC(messagei, 256)

BSDATA(speechi) = {{"Say"},
{"Ask"},
};
assert_enum(speech, Ask)
INSTELEM(speechi)