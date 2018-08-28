/**
 * CMF Loader C++ version by congard
 * Little endian code
 * 
 * CMF (Congard's Model Format) - format for storing 3D models based on OBJ format syntax
 * My telegram: https://t.me/congard
 * My gitlab: https://gitlab.com/congard
 * @author congard
 * 
 * @loader-version 1.0
 */

#ifndef CMF_LOADER_CPP
#define CMF_LOADER_CPP

#include <iostream>
#include <cstring>
#include "core.cpp"

#define VERTEX_DATA 1
#define NORMAL_DATA 2
#define TEXTURE_DATA 3
#define FACE_DATA 4

#define STATE_LOADING_BEGIN 5

#define VERSION_CODE 0          // start 0
#define CMF_VERSION_CODE 1      // start 0
#define VERSION 1.0f            // start 1.0
#define CMF_VERSION 2.0f        // start 1.0

#define DATA_SIZE_BYTE 1
#define DATA_SIZE_SHORT 2
#define DATA_SIZE_INTEGER 4

class CMFLoader {
    private:
        ByteArrayHelper cmf;
        CMFTimer timer, cmfTimer;

        unsigned short getUShort(ubyte b1, ubyte b2) {
            return b1 << 8 | b2;
        }

        unsigned int getUInt(ubyte b1, ubyte b2, ubyte b3, ubyte b4) {
            return b1 << 24 | b2 << 16 | b3 << 8 | b4;
        }

        float getFloat(ubyte b1, ubyte b2, ubyte b3, ubyte b4) {
            float result;
            ubyte bytes[4] = {b4, b3, b2, b1};
            std::memcpy(&result, &bytes, 4);
            return result;
        }

        void exp(std::string message) {
            std::cout << message << std::endl;
            std::terminate();
        }

        void log(std::string message) {
            if (logging) std::cout << message << std::endl;
        }

        void getDTBC(int *dtbc, ubyte ub) {
            dtbc[0] = ub >> 6;			// data type (2 bit, max: 3)
            dtbc[1] = ub & 0b00111111;	// blocks count (6 bit, max: 63)
        }

    public:
        FloatArrayHelper vertices, normals, texCoords;
        UInt2DArrayHelper faces;
        ByteArrayHelper footer;
        bool logging = false;

        ~CMFLoader() {
            delete[] cmf.array;
            delete[] vertices.array;
            delete[] normals.array;
            delete[] texCoords.array;
            for (size_t i = 0; i < faces.size; i++) delete[] faces.array[i];
            delete[] faces.array;
            delete[] faces.sizes;
            if (footer.array != nullptr) delete[] footer.array;
        }

        void loadCMF(const char *path) {
            FILE *file = fopen(path, "rb");
            fseek(file, 0, SEEK_END);
            cmf.size = ftell(file);
            fseek(file, 0, SEEK_SET);
            cmf.array = new byte[cmf.size];
            fseek(file, 0, SEEK_SET);
            fread(cmf.array, 1, cmf.size, file);
            fclose(file);
        }

        void load() {
            unsigned int cmfVer = getUInt(cmf.array[0], cmf.array[1], cmf.array[2], cmf.array[3]);
            if (cmfVer != CMF_VERSION_CODE) 
                exp("Required CMF version code: " + std::to_string(cmfVer) + 
                    "\nCurrent loader CMF version code: " + std::to_string(CMF_VERSION_CODE) + 
					"\ngitlab.com/congard/libcmf-cpp\n");

            log("CMF size = " + std::to_string(cmf.size) + " bytes" + 
                "\nCMF file version code: " + std::to_string(cmfVer) +
				"\nCMF file version: " + std::to_string(getFloat(cmf.array[4], cmf.array[5], cmf.array[6], cmf.array[7])) + 
				"\nLoader CMF version code: " + std::to_string(CMF_VERSION_CODE) +
				"\nLoader CMF version: " + std::to_string(CMF_VERSION) + 
				"\nLoader version code: " + std::to_string(VERSION_CODE) +
				"\nLoader version: " + std::to_string(VERSION) + 
				"\n\nTelegram: t.me/congard\n" + 
				"Gitlab: gitlab.com/congard\n" + 
				"Email: dbcongard@gmail.com\n" + 
				"Support via Payeer: P36281059\n");

            if (logging) cmfTimer.start("Loading CMF file started");

            size_t currentState = STATE_LOADING_BEGIN;
		    size_t size = 0; // count of vertices / normals / texture coordinates

            for (size_t i = 8; i < cmf.size; i++) {
                if (currentState == STATE_LOADING_BEGIN) {
                    // reading header
                    currentState = cmf.array[i];
                    size = getUInt(cmf.array[i + 1], cmf.array[i + 2], cmf.array[i + 3], cmf.array[i + 4]);
                    i += 4;
                    log("Header read, result is  type " + std::to_string(currentState) + "  size " + std::to_string(size) + (currentState == FACE_DATA ? " blocks" : " vert"));
                } else if (currentState == VERTEX_DATA) {
                    /* --- vertex data --- */

                    if (logging) timer.start("Loading vertex data     type 1  size " + std::to_string(size * 12) + " bytes");

                    vertices.array = new float[size * 3];
                    vertices.size = size * 3;
                    for (size_t j = 0; j < size; j++) {
                        vertices.array[j * 3 + 0] = getFloat(cmf.array[i + 0], cmf.array[i + 1], cmf.array[i + 2], cmf.array[i + 3]);
                        vertices.array[j * 3 + 1] = getFloat(cmf.array[i + 4], cmf.array[i + 5], cmf.array[i + 6], cmf.array[i + 7]);
                        vertices.array[j * 3 + 2] = getFloat(cmf.array[i + 8], cmf.array[i + 9], cmf.array[i + 10], cmf.array[i + 11]);
                        i += 12;
                    }
                    i--; // because in cycle body i++, in cycle "j" i+= 12
                    currentState = STATE_LOADING_BEGIN;

                    if (logging) timer.stop("Vertex data loaded in ");
                } else if (currentState == NORMAL_DATA) {
                    /* --- normal data --- */

                    if (logging) timer.start("Loading normal data     type 2  size " + std::to_string(size * 12) + " bytes");

                    normals.array = new float[size * 3];
                    normals.size = size * 3;
                    for (size_t j = 0; j < size; j++) {
                        normals.array[j * 3 + 0] = getFloat(cmf.array[i + 0], cmf.array[i + 1], cmf.array[i + 2], cmf.array[i + 3]);
                        normals.array[j * 3 + 1] = getFloat(cmf.array[i + 4], cmf.array[i + 5], cmf.array[i + 6], cmf.array[i + 7]);
                        normals.array[j * 3 + 2] = getFloat(cmf.array[i + 8], cmf.array[i + 9], cmf.array[i + 10], cmf.array[i + 11]);
                        i += 12;
                    }
                    i--; // because in cycle body i++, in cycle "j" i+= 12
                    currentState = STATE_LOADING_BEGIN;

                    if (logging) timer.stop("Normal data loaded in ");
                } else if (currentState == TEXTURE_DATA) {
                    /* --- texture coordinates data --- */

                    if (logging) timer.start("Loading texture data    type 3  size " + std::to_string(size * 8) + " bytes");

                    texCoords.array = new float[size * 2];
                    texCoords.size = size * 2;
                    for (size_t j = 0; j < size; j++) {
                        texCoords.array[j * 2 + 0] = getFloat(cmf.array[i + 0], cmf.array[i + 1], cmf.array[i + 2], cmf.array[i + 3]);
                        texCoords.array[j * 2 + 1] = getFloat(cmf.array[i + 4], cmf.array[i + 5], cmf.array[i + 6], cmf.array[i + 7]);
                        i += 8;
                    }
                    i--; // because in cycle body i++, in cycle "j" i+= 8
                    currentState = STATE_LOADING_BEGIN;

                    if (logging) timer.stop("Texture data loaded in ");
                } else if (currentState == FACE_DATA) {
                    /* --- face data --- */

                    /*
				     * faces: 
				     * [
				     * 		[pcount = 3; fcount = 2;
				     * 			v1, n1, tx1,  v2, n2, tx2,  v3, n3, tx3, // face 1 (triangle)
				     * 			v4, n4, tx4,  v5, n5, tx5,  v6, n6, tx6  // face 2 (triangle)
				     * 		],
				     * 		[pcount = 4; fcount = 1;
				     * 			v1, n1, tx1,  v2, n2, tx2,  v3, n3, tx3,  v4, n4, tx4 // face 3 (quad)
				     * 		]
				     * ]
				     * 
				     * face point - vertex/normal/texCoord indices
				     * pcount - number of face points
				     * fcount - number of polygons whose number of vertices is pcount (all pcount / pcount)
				     * 
				     */

                    ubyte pcount;
                    size_t counter = 0,
                        bytesSize = 0, // faces size in bytes
                        tmpSize, // size of current face block in bytes
                        tmpCount = 0, // total fcount
                        fcount, // current faces count
                        /* --- byte indices [0; 255] --- */
                        byteTypeStart = 0, byteTypeEnd = 0,
                        /* --- short indices [256; 65535] --- */
                        shortTypeStart = 0, shortTypeEnd = 0,
                        /* --- integer indices [65536; Integer.MAX_VALUE] --- */
                        intTypeStart = 0, intTypeEnd = 0,
                        blocksCounter = 0,
                        dataSize = 0;
                    int *dtbc = new int[2];

                    /* --- calculating size --- */
                    log("Calculating faces size (in bytes)");
                    for (size_t j = i; j < cmf.size;) { // The body looks like this because we dont do j-- in that(*) field
                        if (counter == size) break; // so as not to get to the footer

                        /* --- determine the data type --- */
                        if (blocksCounter == 0) {
                            getDTBC(dtbc, cmf.array[j]);
                            dataSize = dtbc[0] == 0 ? DATA_SIZE_BYTE : dtbc[0] == 1 ? DATA_SIZE_SHORT : DATA_SIZE_INTEGER;
                            blocksCounter = dtbc[1];

                            switch (dtbc[0]) {
                            case 0:
                                byteTypeEnd = byteTypeStart = j;
                                break;
                            case 1:
                                shortTypeEnd = shortTypeStart = j;
                                break;
                            case 2:
                                intTypeEnd = intTypeStart = j;
                                break;
                            }

                            j++;
                        }

                        pcount = cmf.array[j]; // points count to make polygon. For example, 4 points - quad, 3 points - triangle, etc
                        fcount = getUInt(cmf.array[j + 1], cmf.array[j + 2], cmf.array[j + 3], cmf.array[j + 4]); // faces count
                        tmpCount += fcount;
                        // * 3 * dataSize because 1 point - 3 * dataSize bytes (vertex/normal/texture: dataSize bytes per one)
                        tmpSize = pcount * fcount * 3 * dataSize;
                        bytesSize += tmpSize;
                        j += 5 + tmpSize; // (*), 5 bytes - first is pointsCount and second facesCount
                        counter++;
                        blocksCounter--;

                        if (dataSize == DATA_SIZE_BYTE)
                            byteTypeEnd += tmpSize;
                        else if (dataSize == DATA_SIZE_SHORT)
                            shortTypeEnd += tmpSize;
                        else if (dataSize == DATA_SIZE_INTEGER)
                            intTypeEnd += tmpSize;

                        log(std::to_string(fcount) + (pcount == 3 ? " triangles found" : pcount == 4 ? " quads found" : " polygons found, vertices count = " + std::to_string((int)pcount)));
                    }

                    delete[] dtbc;

                    log(
                        "Faces size calculated           size " + std::to_string(bytesSize) + " bytes\nFaces count is " + std::to_string(tmpCount) +
                        "\nByte type faces start " + std::to_string(byteTypeStart) + " end " + std::to_string(byteTypeEnd) +
                        "\nShort type faces start " + std::to_string(shortTypeStart) + " end " + std::to_string(shortTypeEnd) +
                        "\nInteger type faces start " + std::to_string(intTypeStart) + " end " + std::to_string(intTypeEnd) +
                        "\nLoading faces"
                    );

                    faces.array = new unsigned int*[tmpCount];
                    faces.sizes = new size_t[tmpCount];
                    faces.size = tmpCount;

                    /* --- loading faces --- */
                    counter = 0; // counter 1
                    while (i < cmf.size) {
                        if (size == 0) {
                            // read footer
                            footer.array = new byte[cmf.size - i];
                            footer.size = cmf.size - i;
                            std::copy(cmf.array + i, cmf.array + cmf.size, footer.array);
                            i = cmf.size; // loading done
                            return;
                        }

                        if (byteTypeStart == i) {
                            i++; // skipping dtbc byte
                            dataSize = DATA_SIZE_BYTE;
                        } else if (shortTypeStart == i) {
                            i++; // skipping dtbc byte
                            dataSize = DATA_SIZE_SHORT;
                        } else if (intTypeStart == i) {
                            i++; // skipping dtbc byte
                            dataSize = DATA_SIZE_INTEGER;
                        }

                        // reading header
                        pcount = cmf.array[i]; // points count
                        fcount = getUInt(cmf.array[i + 1], cmf.array[i + 2], cmf.array[i + 3], cmf.array[i + 4]);
                        i += 5; // pointer to faces data (header skipped)
                        // reading faces
                        for (size_t j = 0; j < fcount; j++) {
                            faces.array[counter] = new unsigned int[pcount * 3]; // vertex/normal/texture - 3
                            faces.sizes[counter] = pcount * 3;

                            for (size_t l = 0; l < faces.sizes[counter]; l += 3) {
                                switch (dataSize) {
                                case DATA_SIZE_BYTE:
                                    faces.array[counter][l + 0] = (ubyte)cmf.array[i + 0];
                                    faces.array[counter][l + 1] = (ubyte)cmf.array[i + 1];
                                    faces.array[counter][l + 2] = (ubyte)cmf.array[i + 2];
                                    break;
                                case DATA_SIZE_SHORT:
                                    faces.array[counter][l + 0] = getUShort(cmf.array[i + 0], cmf.array[i + 1]);
                                    faces.array[counter][l + 1] = getUShort(cmf.array[i + 2], cmf.array[i + 3]);
                                    faces.array[counter][l + 2] = getUShort(cmf.array[i + 4], cmf.array[i + 5]);
                                    break;
                                case DATA_SIZE_INTEGER:
                                    faces.array[counter][l + 0] = getUInt(cmf.array[i + 0], cmf.array[i + 1], cmf.array[i + 2], cmf.array[i + 3]);
                                    faces.array[counter][l + 1] = getUInt(cmf.array[i + 4], cmf.array[i + 5], cmf.array[i + 6], cmf.array[i + 7]);
                                    faces.array[counter][l + 2] = getUInt(cmf.array[i + 8], cmf.array[i + 9], cmf.array[i + 10], cmf.array[i + 11]);
                                    break;
                                }

                                i += dataSize * 3;
                            }
                            counter++;
                        }
                        size--; // size - count of faces blocks
                    }
                }
            }

            if (logging) cmfTimer.stop("CMF file loaded in ");
        }
};

#endif /* CMF_LOADER_CPP */