#include "GPXParser.h"
#include <stdlib.h>

// Shlok Patel Student # 0979966

//string helper functions

int checkStringEmpty(char * string) {
    if(strcmp(string, "") == 0) {
        return 1; //is empty
    }
    else {
        return 0; //is not empty
    }
}

void copyNewString(char * stringToCopy, char ** string) {
    int space = (int)(strlen(stringToCopy)) + 1;
    free(*string);
    *string = malloc(space);
    if(*string == NULL) {
        return;
    }
    strcpy(*string, stringToCopy);
}

void catString(char * stringToCat, char ** string) {
    int length1 = (int)(strlen(stringToCat)) + 1;
    int length2 = (int)(strlen(*string)) + 1;
    *string = (char*)(realloc(*string, length1+length2));
    if(*string == NULL) {
        return;
    }
    else {
        strcat(*string, stringToCat);
    }
}

char * createEmptyString() {
    char * string = malloc(sizeof(char));
    if(string == NULL) {
        return NULL;
    }
    string[0] = '\0';
    return string;
}

void freeString(char * stringToFree) {
    free(stringToFree);
}

// Initialize functions

GPXdoc * newGPXdoc() {
    GPXdoc * new = malloc(sizeof(GPXdoc));
    if(new == NULL) {
        return NULL;
    }
    new->version = -100.0; //init value for version
    new->creator = createEmptyString(); //empty string

    //initialize track and routes and waypoints variables
    char * (*wptStr)(void*) = &waypointToString;
    void (*wptDel)(void*) = &deleteWaypoint;
    int (*wptCmp)(const void*,const void*) = &compareWaypoints;
    new->waypoints = initializeList(wptStr, wptDel, wptCmp);

    char * (*rtStr)(void*) = &routeToString;
    void (*rtDel)(void*) = &deleteRoute;
    int (*rtCmp)(const void*, const void*) = &compareRoutes;
    new->routes = initializeList(rtStr, rtDel, rtCmp);

    char * (*trStr)(void*) = &trackToString;
    void (*trDel)(void*) = &deleteTrack;
    int (*trCmp)(const void*, const void*) = &compareTracks;
    new->tracks = initializeList(trStr, trDel, trCmp);

    return new;
}

GPXData * newGPXData(int space) {
    GPXData * new = malloc(sizeof(GPXData) + space + 6); //6 extra bytes for init value
    if(new == NULL) {
        return NULL;
    }
    //initialize both fields
    strcpy(new->name, "name");
    strcpy(new->value, "value");
    return new;
}

Waypoint * newWaypoint() {
    Waypoint * new = malloc(sizeof(Waypoint));
    if(new == NULL) {
        return NULL;
    }
    new->name = createEmptyString();
    new->longitude = -200.0; //invalid longitude value: for testing
    new->latitude = -200.0;
    
    void (*dataDel)(void *) = &deleteGpxData;
    char * (*dataStr)(void *) = &gpxDataToString;
    int (*dataCmp)(const void *, const void*) = &compareGpxData;

    new->otherData = initializeList(dataStr, dataDel, dataCmp);

    return new;
}

Route * newRoute() {
    Route * new = malloc(sizeof(Route));
    if(new == NULL) {
        return NULL;
    }
    new->name = createEmptyString();

    void (*dataDel)(void *) = &deleteGpxData;
    char * (*dataStr)(void *) = &gpxDataToString;
    int (*dataCmp)(const void *, const void*) = &compareGpxData;

    new->otherData = initializeList(dataStr, dataDel, dataCmp);

    char* (*wptStr)(void*) = &waypointToString;
    void (*wptDel)(void*) = &deleteWaypoint;
    int (*wptCmp)(const void*,const void*) = &compareWaypoints;
    
    new->waypoints = initializeList(wptStr, wptDel, wptCmp);

    return new;
}

TrackSegment * newTrackSegment() {
    TrackSegment * new = malloc(sizeof(TrackSegment));
    if(new == NULL) {
        return NULL;
    }

    char* (*wptStr)(void*) = &waypointToString;
    void (*wptDel)(void*) = &deleteWaypoint;
    int (*wptCmp)(const void*,const void*) = &compareWaypoints;
    
    new->waypoints = initializeList(wptStr, wptDel, wptCmp);

    return new; 
}

Track * newTrack() {
    Track * new = malloc(sizeof(Track));
    if(new == NULL) {
        return NULL;
    }
    new->name = createEmptyString();

    void (*tsDel)(void *) = &deleteTrackSegment;
    char * (*tsStr)(void *) = &trackSegmentToString;
    int (*tsCmp)(const void*, const void*) = &compareTrackSegments;

    new->segments = initializeList(tsStr, tsDel, tsCmp);

    void (*dataDel)(void *) = &deleteGpxData;
    char * (*dataStr)(void *) = &gpxDataToString;
    int (*dataCmp)(const void *, const void*) = &compareGpxData;

    new->otherData = initializeList(dataStr, dataDel, dataCmp);

    return new;
}

// Parsing functions

GPXData * parseGPXData(char * name, xmlNode * node) {
    if(node == NULL) {
        return NULL;
    }

    char * content = (char*)(node->content);
    if(checkStringEmpty(content) == 0) { 
        int length = (int)(strlen(content)) + 1; //length of content needed for malloc purpose
        GPXData * new = newGPXData(length);
        if(new == NULL) {
            return NULL;
        }
        strcpy(new->name, name);
        strcpy(new->value, content);
        return new;
    }
    return NULL;
}

Waypoint * parseWaypoint(xmlNode * node) {
    Waypoint * wpt = newWaypoint();
    if(wpt == NULL) {
        return NULL;
    }
  
    xmlAttr * attribute  = node->properties;
    
    for(xmlAttr * iter = attribute; iter != NULL; iter= iter->next) {
        char * nodeName = (char*)(iter->name);
        if(strcmp(nodeName, "lat") == 0) {
            xmlNode * iterNode = iter->children;
            char * content = (char*)(iterNode->content);
            double d = strtod(content, NULL);
            wpt->latitude = d;
        }
        else if(strcmp(nodeName, "lon") == 0) {
            xmlNode * iterNode = iter->children;
            char * content = (char*)(iterNode->content);
            double d = strtod(content, NULL);
            wpt->longitude = d;
        }
        else{
            deleteWaypoint(wpt);
            return NULL; //cannot be any other attribute 
        }
    }

    /*
    if(wpt->latitude == -200.0) { 
        deleteWaypoint(wpt);
        return NULL; //invalid waypoint -> no latitude FIX
    }
    if(wpt->longitude == -200.0) {
        deleteWaypoint(wpt);
        return NULL; //invalid waypoint -> no longitude FIX
    }
    
    */

    xmlNode * child = node->children;

    for(xmlNode * it = child; it != NULL; it=it->next) {
        if(it->type == XML_ELEMENT_NODE) {
            char * name = (char*)(it->name);
            
            if(strcmp(name, "name") == 0) {
                xmlNode * nodeC = it->children;
                if(nodeC != NULL) {
                    char * nodeContent = (char*)(nodeC->content);
                    copyNewString(nodeContent, &(wpt->name));
                }
            }
            else{
                xmlNode * extra = it->children;

                GPXData * newData = parseGPXData(name, extra);
                if(newData == NULL) {
                    deleteWaypoint(wpt);
                    return NULL;
                }
                insertBack(wpt->otherData, (void*)newData);
            }
        }
    }
    return wpt;
}

Route * parseRoute(xmlNode * node) {
    Route * rt = newRoute();
    if(rt == NULL) {
        return NULL;
    }
    
    xmlNode * child = node->children;

    for(xmlNode * iter = child; iter!= NULL; iter=iter->next) {
        
        if(iter->type == XML_ELEMENT_NODE) {
            char * nodeName = (char*)(iter->name);
            if(strcmp(nodeName, "name") == 0) {
                xmlNode * nodeC = iter->children;
                
                if(nodeC != NULL) {
                    char * childContent = (char*)(nodeC->content);
                    copyNewString(childContent, &(rt->name));
                }
            }
            
            else if(strcmp(nodeName, "rtept") == 0) {
                Waypoint * newWpt = parseWaypoint(iter);
                if(newWpt == NULL) {
                    deleteRoute(rt);
                    return NULL;
                }
                insertBack(rt->waypoints, (void*)newWpt);
            }
            
            else {
                xmlNode * extra = iter->children;

                GPXData * newData = parseGPXData(nodeName, extra);
                if(newData == NULL) {
                    deleteRoute(rt);
                    return NULL;
                }
                insertBack(rt->otherData, (void*)newData);
            }
        }
    }
    return rt;
}

TrackSegment * parseTrackSegment(xmlNode * node) {
    TrackSegment * ts = newTrackSegment();
    if(ts == NULL) {
        return NULL;
    }

    xmlNode * child = node->children;
    
    for(xmlNode * iter = child; iter!= NULL; iter=iter->next) {
        char * nodeName = (char*) (iter->name);
        
        if(iter->type == XML_ELEMENT_NODE) {
            if(strcmp(nodeName, "trkpt") == 0){
                Waypoint * newWaypoint = parseWaypoint(iter);
                if(newWaypoint == NULL) {
                    deleteTrackSegment(ts);
                    return NULL;
                }
                insertBack(ts->waypoints, (void*)newWaypoint);
            }
            else{
                deleteTrackSegment(ts);
                return NULL;
            }
        }
    }
    return ts;
}

Track * parseTrack(xmlNode * node) {
    Track * tr = newTrack();
    if(tr == NULL) {
        return NULL;
    }

    xmlNode * child = node->children;

    for(xmlNode * iter = child; iter!= NULL; iter=iter->next) {
        
        if(iter->type == XML_ELEMENT_NODE) {
            
            char * nodeName = (char*)(iter->name);
            
            if(strcmp(nodeName, "name") == 0) {
                
                xmlNode * nodeChild = iter->children;
        
                if(nodeChild != NULL) {
                    char * childContent = (char*)(nodeChild->content);
                    copyNewString(childContent, &(tr->name));
                }
            }
            
            else if(strcmp(nodeName, "trkseg") == 0) {
                TrackSegment * newTS = parseTrackSegment(iter);
                if(newTS == NULL) {
                    deleteTrack(tr);
                    return NULL;
                }
                insertBack(tr->segments, (void*)newTS);
            }
            else {
                xmlNode * extra = iter->children; 
              
                GPXData * newData = parseGPXData(nodeName, extra);
                if(newData == NULL) {
                    deleteTrack(tr);
                    return NULL;
                }  
                insertBack(tr->otherData, (void*)newData);
            }
        }
    }
    return tr;
}






