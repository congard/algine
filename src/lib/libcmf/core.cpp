/**
 * CMF core C++ version by congard
 * 
 * CMF (Congard's Model Format) - format for storing 3D models based on OBJ format syntax
 * My telegram: https://t.me/congard
 * My gitlab: https://gitlab.com/congard
 * @author congard
 * 
 * @core-version 1.0
 */

#ifndef CMF_CORE_CPP
#define CMF_CORE_CPP

#include <iostream>
#include <chrono>
#include <vector>

typedef char byte;
typedef unsigned char ubyte;

class Polygon;

struct FloatArrayHelper {
    float *array = nullptr;
    size_t size;
};

struct UInt2DArrayHelper {
    unsigned int **array = nullptr;
    size_t *sizes = nullptr;
    size_t size;
};

struct ByteArrayHelper {
    byte *array = nullptr;
    size_t size;
};

struct PolygonArrayHelper {
    Polygon *array = nullptr;
    size_t size;
};

class CMFTimer {
    private: 
        long startTime;

    public:
        void start(std::string message) {
            startTime = currentTimeMillis();
            std::cout << message << std::endl;
        }

        void stop(std::string message) {
            std::cout << message << currentTimeMillis() - startTime << "ms" << std::endl;
        }

        long currentTimeMillis() {
            return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        }
};

/*
 * class that may used when rendering
 */
class Polygon {
    public:
        static const int TYPE_TRIANGLE = 0x0004, 
		    TYPE_QUAD = 0x0007,
			TYPE_POLYGON = 0x0009;
        
        int type, start = -1, count = 0, pcount;

        Polygon(int pcount) {
		    this->pcount = pcount;
	    }

        Polygon() { /* empty */ }
};

/**
 * std::vector with pcount support
 */
template <typename T> class PolygonList : public std::vector<T> {
    public:
        int pcount;

        void set_pcount(int pcount) {
            this->pcount = pcount;
        }
};

#endif /* CMF_CORE_CPP */