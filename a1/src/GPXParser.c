#include "GPXParser.h"
#include "GPXHelper.h"
#include <stdlib.h>

// Shlok Patel Student # 0979966

// Module 2 functions

int getNumWaypoints(const GPXdoc * doc) {
    if(doc == NULL || doc->waypoints == NULL) {
        return -1; //FIX 
    }

    int total = 0;
    total += getLength(doc->waypoints);

    return total;
}

int getNumRoutes(const GPXdoc * doc) {
    if(doc == NULL || doc->routes == NULL) {
        return -1; //FIX 
    } 
    int total = 0;
    total += getLength(doc->routes);

    return total;
}

int getNumTracks(const GPXdoc * doc) {
    if(doc == NULL || doc->tracks == NULL) {
        return -1; //FIX 
    } 
    int total = 0;
    total += getLength(doc->tracks);

    return total;
}

int getNumSegments(const GPXdoc * doc) {
    if(doc == NULL) {
        return -1;
    }
    int total = 0;
    ListIterator track = createIterator(doc->tracks);
    Track * elem = NULL;
    while((elem = nextElement(&track)) != NULL) {
        List * seg = elem->segments;

        ListIterator ts = createIterator(seg);
        TrackSegment * tsElem = NULL;
        while((tsElem = nextElement(&ts)) != NULL) {
            total += 1;
        }
    }
    return total;
}

int getNumGPXData(const GPXdoc * doc) {
    if(doc == NULL) {
        return -1;
    }
    int total = 0;
    
    ListIterator track = createIterator(doc->tracks);
    Track * elem1 = NULL;
    while((elem1 = nextElement(&track)) != NULL) {
        
        if(checkStringEmpty(elem1->name) == 0) {
            total++;
        }
        
        List * o = elem1->otherData;
        total += getLength(o);

        List * s = elem1->segments;
        ListIterator e = createIterator(s);
        TrackSegment * ts = NULL;
        while((ts = nextElement(&e)) != NULL) {
            
            List * tsWaypoints = ts->waypoints;
            ListIterator l = createIterator(tsWaypoints);
            Waypoint * w = NULL;

            while((w = nextElement(&l)) != NULL) {
                
                if(checkStringEmpty(w->name) == 0) {
                    total++;
                }
                
                List * otherDataWaypoint = w->otherData;
                total += getLength(otherDataWaypoint);
            }
        }
    }
    
    ListIterator route = createIterator(doc->routes);
    Route * elem2 = NULL;
    while((elem2 = nextElement(&route)) != NULL) {
        
        if(checkStringEmpty(elem2->name) == 0) {
            total++;
        }
        
        List * o = elem2->otherData;
        total += getLength(o);

        List * waypoints = elem2->waypoints;
        ListIterator l = createIterator(waypoints);
        Waypoint * w = NULL;

        while((w = nextElement(&l)) != NULL) {
            
            if(checkStringEmpty(w->name) == 0) {
                total++;
            }

            List * otherData = w->otherData;
            total += getLength(otherData);
        }
    }

    ListIterator waypoint = createIterator(doc->waypoints);
    Waypoint * elem3 = NULL;
    while((elem3 = nextElement(&waypoint)) != NULL) {
        
        if(checkStringEmpty(elem3->name) == 0) {
            total++;
        }
        
        List * o  = elem3->otherData;
        total += getLength(o);
    }
    return total;
}

Waypoint * getWaypoint(const GPXdoc * doc, char * name) {

    if(doc == NULL || name == NULL) {
        return NULL;
    }
    
    if(doc->waypoints != NULL) {

        ListIterator iter = createIterator(doc->waypoints);    
        Waypoint * w = NULL;

        while((w = nextElement(&iter)) != NULL) {
            if(strcmp(name, w->name) == 0) {
                return w;
            }
        }
        return NULL;
    }
    else {
        return NULL;
    }
    
}

Track * getTrack(const GPXdoc * doc, char * name) {

    if(doc == NULL || name == NULL) {
        return NULL;
    }
    
    if(doc->tracks != NULL) {

        ListIterator iter = createIterator(doc->tracks);
        Track * t = NULL;

        while((t = nextElement(&iter)) != NULL) {
            if(strcmp(name, t->name) == 0) {
                return t;
            }
        }
        return NULL;
    }
    else {
        return NULL;
    }

}

Route * getRoute(const GPXdoc * doc, char * name) {
    if(doc == NULL || name == NULL) {
        return NULL;
    }

    if(doc->routes != NULL) {

        ListIterator iter = createIterator(doc->routes);
        Route * r = NULL;

        while((r = nextElement(&iter)) != NULL) {
            if(strcmp(name, r->name) == 0) {
                return r;
            }
        }
        return NULL;
    }
    else {
        return NULL;
    }
}

//GPXdoc helper functions

void deleteGPXdoc(GPXdoc * toBeDeleted) {
    if(toBeDeleted != NULL) {
        free(toBeDeleted->creator);
        freeList(toBeDeleted->waypoints);
        freeList(toBeDeleted->routes);
        freeList(toBeDeleted->tracks);
        free(toBeDeleted); 
    }
}

char * GPXdocToString(GPXdoc * doc) {
    
    if(doc == NULL) {
        return NULL;
    }
       
    char * temp = createEmptyString();

    catString("Namespace: ", &temp);
    catString(doc->namespace, &temp);

    char tempDouble[50];
    sprintf(tempDouble, "\n\nVersion: %.4f\n", doc->version);
    catString(tempDouble, &temp);

    catString("\nCreator: ", &temp);
    catString(doc->creator, &temp);
    catString("\n\n", &temp);

    char * t = toString(doc->tracks);
    char * w = toString(doc->waypoints);
    char * r = toString(doc->routes);
    catString("TRACKS:\n", &temp);
    catString(t, &temp);
    catString("WAYPOINTS:\n", &temp);
    catString(w, &temp);
    catString("\nROUTES:\n", &temp);
    catString(r, &temp);

    free(t);
    free(w);
    free(r);

    return temp;
}

//GPXData helper functions

void deleteGpxData(void * delete) {
    GPXData * temp = (GPXData*) delete;
    if(temp!=NULL) {
        free(temp);
    }   
}

char * gpxDataToString(void * data) {
    GPXData * d = (GPXData*) data;
    char * temp = createEmptyString();
    catString("GPXData name: ", &temp);
    catString(d->name, &temp);
    catString("\nGPXData value: ", &temp);
    catString(d->value, &temp);
    catString("\n", &temp);

    return temp;
}

int compareGpxData(const void *first, const void *second) {
    return 0; 
}

//Waypoint helper functions

void deleteWaypoint(void * data) {
    Waypoint * wp = (Waypoint*) data;
    if(wp != NULL) {
        free(wp->name);
        freeList(wp->otherData); 
        free(wp);
    }
}

int compareWaypoints(const void *first, const void *second) {
    return 0; 
}

char * waypointToString(void *data) {
    Waypoint * wpt = (Waypoint*) data;
    char * temp = createEmptyString();
    
    catString("Waypointname: ", &temp);
    catString(wpt->name, &temp);
    catString("\n", &temp);
    
    char tempDouble[50];
    sprintf(tempDouble, "longitude: %.4f latitude: %.4f\n", wpt->longitude, wpt->latitude);
    catString(tempDouble, &temp);
    
    char * otherData = toString(wpt->otherData);
    catString(otherData, &temp);
    free(otherData);

    return temp;
}

//Route helper functions

void deleteRoute(void *data) {
    Route * rt = (Route*) data;
    if(rt != NULL) {
        free(rt->name);
        freeList(rt->waypoints);
        freeList(rt->otherData);
        free(rt);
    }
}

char * routeToString(void *data) {
    Route * rt = (Route*) data;
    char * temp = createEmptyString();
    
    catString("Route name: ", &temp);
    catString(rt->name, &temp);
    catString("\n", &temp);
    char * wptTemp = toString(rt->waypoints);
    char * otherDataTemp = toString(rt->otherData);

    catString("Route waypoints:\n", &temp);
    catString(wptTemp, &temp);
    catString("Route otherdata:\n", &temp);
    catString(otherDataTemp, &temp);

    free(wptTemp);
    free(otherDataTemp);

    return temp;
}

int compareRoutes(const void *first, const void *second) {
    return 0;
}

//TrackSegment helper functions

void deleteTrackSegment(void * data) {
    TrackSegment * ts = (TrackSegment*) data;
    if(ts != NULL) {
        freeList(ts->waypoints);
        free(ts);
    }
}

char * trackSegmentToString(void * data) {
    TrackSegment * ts = (TrackSegment*) data;
    char * temp = createEmptyString();

    catString("TrackSegment waypoints:\n", &temp);
    char * wpList = toString(ts->waypoints);
    catString(wpList, &temp);
    free(wpList);

    return temp;
}

int compareTrackSegments(const void *first, const void *second) {
    return 0;
}

//Track helper functions

void deleteTrack(void * data) {
    Track * t = (Track*) data;
    if(t != NULL) {
        free(t->name);
        freeList(t->segments);
        freeList(t->otherData);
        free(t);
    }
}

char * trackToString(void * data) {
    Track * t = (Track*) data;
 
    char * temp = createEmptyString();

    catString("Track name: ", &temp);
    catString(t->name, &temp);
    catString("\n", &temp);

    char * segList = toString(t->segments);
    catString(segList, &temp);
    
    catString("\nTrack otherData:\n", &temp);
    char * dataList = toString(t->otherData);
    catString(dataList, &temp);

    free(segList);
    free(dataList);

    return temp;
}

int compareTracks(const void *first, const void *second) {
    return 0;
}

GPXdoc* createGPXdoc(char* fileName) {
    xmlDoc *doc = NULL;
    xmlNode *rootElement = NULL;
   
    doc = xmlReadFile(fileName, NULL, 0);
    if (doc == NULL) {
        xmlCleanupParser();
        return NULL;
    }

    //this is the gpx node - checks if the root node is valid
    rootElement = xmlDocGetRootElement(doc);
    if(rootElement == NULL) {
        return NULL;
    }

    GPXdoc * gDoc = newGPXdoc();
    if(gDoc == NULL) {
        return NULL;
    }

    //this loops through each gpx->attribute
    xmlAttr * rootAttr = rootElement->properties;
    xmlAttr * curAttr = NULL;

    for (curAttr = rootAttr; curAttr != NULL; curAttr = curAttr->next) {
        if(curAttr->type == XML_ATTRIBUTE_NODE) {
        
            char * cName = (char*)(curAttr->name); //type casts attribute name to char
            
            //compares name to verions
            if(strcmp(cName, "version") == 0) {
                xmlNode * temp = curAttr->children;               
                char * xC = (char*)(temp->content);
                double d = strtod(xC, NULL);
                gDoc->version = d;
            }

            //compares name to creator
            if(strcmp(cName, "creator") == 0) {
                xmlNode * temp = curAttr->children;               
                char * xC = (char*)(temp->content);
                copyNewString(xC, &(gDoc->creator));
            }
        }
    }
    //check if creator is initialized
    if(checkStringEmpty(gDoc->creator) == 1) {
        return NULL;
    }

    //check if version initialized
    if(gDoc->version == -100.0) {
        return NULL;
    }

    //assigning the namespace and checking if present
    xmlNs * nameSpace = rootElement->ns;
    if(nameSpace == NULL) {
        return NULL;
    }
    char * n = (char*)(nameSpace->href);
    if(n != NULL) {
        strcpy(gDoc->namespace, n);
    }
    else{
        return NULL;
    }
    
    //parse through root elements children
    xmlNode * childLink = rootElement->children;
    xmlNode * currNode = NULL;

    for(currNode = childLink; currNode != NULL; currNode = currNode->next) {
        char * cName = (char*)(currNode->name);

        if(strcmp(cName, "wpt") == 0) {
            Waypoint * newWaypoint = parseWaypoint(currNode);
            if(newWaypoint == NULL) {
                deleteGPXdoc(gDoc);
                xmlFreeDoc(doc);
                xmlCleanupParser();
                return NULL;
            }
            insertBack(gDoc->waypoints, (void*)newWaypoint);
        }

        if(strcmp(cName, "trk") == 0) {
            Track * newTrack = parseTrack(currNode);
            if(newTrack == NULL) {
                deleteGPXdoc(gDoc);
                xmlFreeDoc(doc);
                xmlCleanupParser();
                return NULL;
            }
            insertBack(gDoc->tracks, (void*)newTrack);
        }

        if(strcmp(cName, "rte") == 0) {
            Route * newRoute = parseRoute(currNode);
            if(newRoute == NULL) {
                deleteGPXdoc(gDoc);
                xmlFreeDoc(doc);
                xmlCleanupParser();
                return NULL;
            }
            insertBack(gDoc->routes, (void*)newRoute);
        }
    }
    xmlFreeDoc(doc);
    xmlCleanupParser();
    return gDoc;
}
