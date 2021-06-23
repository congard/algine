#ifndef ALGINE_MODULE_BLEND
#define ALGINE_MODULE_BLEND

#define blendAdditive(p1, p2) (p1 + p2)
#define blendScreen(p1, p2) (1 - (1 - p1) * (1 - p2))

#endif //ALGINE_MODULE_BLEND
