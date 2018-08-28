/**
 * CMF2Arrays C++ version by congard
 * 
 * CMF (Congard's Model Format) - format for storing 3D models based on OBJ format syntax
 * My telegram: https://t.me/congard
 * My gitlab: https://gitlab.com/congard
 * @author congard
 * 
 * @arraysmaker-version 1.0
 */

#ifndef CMF_ARRAYSMAKER_CPP
#define CMF_ARRAYSMAKER_CPP

#include <iostream>
#include "core.cpp"

class CMF2Arrays {
    private:
        /* --- input data --- */
	    FloatArrayHelper *vertices, *normals, *texCoords;
	    UInt2DArrayHelper *faces;
		/* --- timers --- */
		CMFTimer timer, cmfTimer;

		/**
		 * getfplid - get float polygon list id
		 * @param array pointer to std::vector<PolygonList<float>>
		 * @param pcount in.pcount
		 * @return if list with in.pcount exist returns id of PolygonList, where pcount = in.pcount, otherwise returns -1
	 	 */
        int getfplid(std::vector<PolygonList<float>> *array, int pcount) {
            for (size_t i = 0; i < array->size(); i++) if (array->at(i).pcount == pcount) return i;
		    return -1;
        }

		/**
	 	 * getfpl - get float polygon list
		 * @param array pointer to std::vector<PolygonList<float>>
	 	 * @param pcount in.pcount
	 	 * @return if polygon list with in.pcount exist returns pointer to it, otherwise creates it and returns pointer to it
	 	 */
        PolygonList<float> *getfpl(std::vector<PolygonList<float>> *array, int pcount) {
            int id = getfplid(array, pcount);
            if (id == -1) {
                PolygonList<float> pl;
                pl.set_pcount(pcount);
			    array->push_back(pl);
			    return &array->at(array->size() - 1);
		    }
		    return &array->at(id);
        }

		/**
		 * Adds vertices (XYZ or XY) to list
		 * @param list pointer to std::vector<float>
		 * @param index vertex index
		 * @param isTex if true adds XY vertex otherwise adds XYZ vertex
		 * @param array vertices, pointer to FloatArrayHelper
		 */
        void add(std::vector<float> *list, size_t index, bool isTex, FloatArrayHelper *array) {
		    if (isTex) {
			    list->push_back(array->array[index * 2 + 0]);
			    list->push_back(array->array[index * 2 + 1]);
		    } else {
			    list->push_back(array->array[index * 3 + 0]);
			    list->push_back(array->array[index * 3 + 1]);
			    list->push_back(array->array[index * 3 + 2]);
		    }
	    }

		/**
		 * @param polygons pointer to std::vector<Polygon>
		 * @param pcount in.pcount
		 * @return if Polygon with in.pcount exist returns id of Polygon, where pcount = in.pcount, otherwise returns -1
		 */
        int getPolygonId(std::vector<Polygon> *polygons, int pcount) {
		    for (size_t i = 0; i < polygons->size(); i++) if (polygons->at(i).pcount == pcount) return i;
		    return -1;
	    }

		/** 
		 * @param polygons pointer to std::vector<Polygon>
		 * @param pcount in.pcount
		 * @return if Polygon with in.pcount exist returns pointer on it, otherwise creates it and returns pointer on it
		 */
        Polygon *getPolygon(std::vector<Polygon> *polygons, int pcount) {
		    int id = getPolygonId(polygons, pcount);
		    if (id == -1) {
			    polygons->push_back(Polygon(pcount));
			    return &polygons->at(polygons->size() - 1);
		    }
		    return &polygons->at(id);
	    }

    public:
        /* --- output data --- */
        FloatArrayHelper outVertices, outNormals, outTexCoords;
	    PolygonArrayHelper polygons;
		/* --- logging --- */
		bool logging = false;

        /**
	     * The method that must be called first
	     * @param vertices [x, y, z, ... , xn, yn, zn]
	     * @param normals [x, y, z, ... , xn, yn, zn]
	     * @param texCoords [x, y, ... , xn, yn]
	     * @param faces [[face], ... , [face]]
	    **/
        void init(FloatArrayHelper *vertices, FloatArrayHelper *normals, FloatArrayHelper *texCoords, UInt2DArrayHelper *faces) {
            this->vertices = vertices;
            this->normals = normals;
            this->texCoords = texCoords;
            this->faces = faces;
        }

		/**
		 * Makes arrays
		 * @param mkVertices if true makes an array (FloatArrayHelper.array) of vertices otherwise outVertices.array == nullptr
		 * @param mkNormals if true makes an array of normals otherwise outNormals.length == 0
		 * @param mkTexCoords if true makes an array of texCoords otherwise outTexCoords.length == 0
		**/
        void make(bool mkVertices, bool mkNormals, bool mkTexCoords) {
			if (logging) cmfTimer.start("Arrays maker start");

            std::vector<PolygonList<float>> vertices, normals, texCoords;
            
			if (logging) timer.start("Sorting start");

            /* --- sorting begin --- */
			/* --- Sorts by polygons: triangle vertices to triangles vertices array, quad vertices to quads vertices array, etc --- */
            {
                PolygonList<float> *plistV, *plistN, *plistT;
                for (size_t i = 0; i < faces->size; i++) {
			        /* --- divide on 3 because vertex/normal/texCoord index length is 3 --- */
			        plistV = getfpl(&vertices, faces->sizes[i] / 3);
			        plistN = getfpl(&normals, faces->sizes[i] / 3);
			        plistT = getfpl(&texCoords, faces->sizes[i] / 3);
			
			        for (size_t j = 0; j < faces->sizes[i]; j += 3) {
				        add(plistV, faces->array[i][j + 0], false, this->vertices);
				        add(plistN, faces->array[i][j + 1], false, this->normals);
				        add(plistT, faces->array[i][j + 2], true, this->texCoords);
			        }
		        }
            }
            /* --- sorting end --- */

			if (logging) {
				timer.stop("Sorting done in ");
				timer.start("Creating Polygons array");
			}

            std::vector<float> outVertices, outNormals, outTexCoords;
            std::vector<Polygon> polygons;

            Polygon *p;
		    int pointer = 0;
		
		    for (size_t i = 0; i < vertices.size(); i++) {
			    p = getPolygon(&polygons, vertices[i].pcount);
			    p->count += vertices[i].size() / 3;  // divide xyz.length (3)
			    if (p->start == -1) {
				    p->start = pointer;
				    if (p->pcount == 3) p->type = Polygon::TYPE_TRIANGLE;
				    else if (p->pcount == 4) p->type = Polygon::TYPE_QUAD;
				    else p->type = Polygon::TYPE_POLYGON;
			    }
			
				/* --- copy from std::vector<PolygonList<float>> to std::vector<float> --- */
			    if (mkVertices) outVertices.insert(outVertices.end(), vertices[i].begin(), vertices[i].end());
			    if (mkNormals) outNormals.insert(outNormals.end(), normals[i].begin(), normals[i].end());
			    if (mkTexCoords) outTexCoords.insert(outTexCoords.end(), texCoords[i].begin(), texCoords[i].end());;
			
			    pointer += vertices[i].size() / 3; // divide xyz.length (3)
		    }

			if (logging) {
				timer.stop("Polygons array created in ");
				timer.start("Copying std::vector s to ArrayHelper s");
			}
            
            this->outVertices.size = outVertices.size();
            this->outNormals.size = outNormals.size();
            this->outTexCoords.size = outTexCoords.size();
			this->polygons.size = polygons.size();

            this->outVertices.array = new float[outVertices.size()];
            this->outNormals.array = new float[outNormals.size()];
            this->outTexCoords.array = new float[outTexCoords.size()];
			this->polygons.array = new Polygon[polygons.size()];

            for (size_t i = 0; i < outVertices.size(); i++) this->outVertices.array[i] = outVertices[i];
            for (size_t i = 0; i < outNormals.size(); i++) this->outNormals.array[i] = outNormals[i];
            for (size_t i = 0; i < outTexCoords.size(); i++) this->outTexCoords.array[i] = outTexCoords[i];
			for (size_t i = 0; i < polygons.size(); i++) this->polygons.array[i] = polygons[i];

			if (logging) {
				timer.stop("Copying done in ");
				cmfTimer.stop("Arrays maker done job in ");
			}
        }

		/**
		 * Deletes creates arrays
		 */
        void cleanup() {
            delete[] outVertices.array;
            delete[] outNormals.array;
            delete[] outTexCoords.array;
			delete[] polygons.array;
        }
};

#endif /* CMF_ARRAYSMAKER_CPP */