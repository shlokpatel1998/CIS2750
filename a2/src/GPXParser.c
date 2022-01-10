#include "GPXParser.h"
#include "GPXHelper.h"
#include <stdlib.h>

// Shlok Patel Student # 0979966

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

// A2 Module 1 Functions

GPXdoc * createValidGPXdoc(char * fileName, char * gpxSchemaFile) {
    if( (fileName == NULL) | (gpxSchemaFile == NULL) ) {
        return NULL;
    }
    LIBXML_TEST_VERSION;

    bool valid = is_valid_tree(fileName, gpxSchemaFile);

    if(!valid) {
        return NULL;
    }

    xmlDoc * doc = xmlReadFile(fileName, NULL, 0);
      
    //this is the gpx node - checks if the root node is valid
    xmlNode * rootElement = xmlDocGetRootElement(doc);
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

    //assigning the namespace 
    xmlNs * nameSpace = rootElement->ns;
    char * n = (char*)(nameSpace->href);
    strcpy(gDoc->namespace, n);

    
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

bool writeGPXdoc(GPXdoc * doc, char * fileName) {
    if( (doc == NULL) | (fileName == NULL) ) {
        return false;
    }
    
    LIBXML_TEST_VERSION;

    bool valid = false;

    xmlDocPtr xml_doc = NULL;
    xmlNodePtr root = NULL;

    xml_doc = xmlNewDoc(BAD_CAST "1.0");
    root = xmlNewNode(NULL, BAD_CAST "gpx");
    xmlDocSetRootElement(xml_doc, root);

    valid = doc_to_tree(&root, doc);
    if(!valid) {
        xmlFreeDoc(xml_doc);
        xmlCleanupParser();
        xmlMemoryDump();
        return false;
    }
   
    int result = xmlSaveFormatFileEnc(fileName, xml_doc, "UTF-8", 1); //FIX format field

    xmlFreeDoc(xml_doc);
    xmlCleanupParser();
    xmlMemoryDump();

    if( (result == -1) ) {
        return false;
    }
    return true;
}

bool validateGPXDoc(GPXdoc* doc, char* gpxSchemaFile) {
    if( (doc == NULL) | (gpxSchemaFile == NULL) ) {
        return NULL;
    }
    LIBXML_TEST_VERSION;

    bool valid = false;

    xmlDocPtr xml_doc = NULL;
    xmlNodePtr root = NULL;

    xml_doc = xmlNewDoc(BAD_CAST "1.0");
    root = xmlNewNode(NULL, BAD_CAST "gpx");
    xmlDocSetRootElement(xml_doc, root);

    valid = doc_to_tree(&root, doc);

    if(valid) {
        valid = is_valid_xmlDoc(xml_doc, gpxSchemaFile);
    }

    xmlFreeDoc(xml_doc);
    xmlCleanupParser();
    xmlMemoryDump();
    
    return valid;
}

// A2 Module 2 Functions

float round10(float len) {
    if(len >= 0) {
        int i = (int)(len);
        int rem = i % 10;
       
        if(rem >=0 && rem <=4) {
            i = i - rem;
        }
        else {
            i = i + (10-rem);
        }
        
       float t = (float)(i);
       return t;
    }
    else return -1;
}

float getRouteLen(const Route *rt) {
    if(rt!= NULL) {

        ListIterator waypoints = createIterator(rt->waypoints);
        Waypoint * wpt = NULL;
        int length = getLength(rt->waypoints);
        double arr[length][2];
        float total = 0.0;
        int i = 0;
        while((wpt = nextElement(&waypoints)) != NULL) {
            arr[i][0] = wpt->latitude;
            arr[i][1] = wpt->longitude;
            i++;
        }

        for(int j = 1; j < length; j++) {
            double lat1 = arr[j-1][0];
            double lat2 = arr[j][0];
            double lon1 = arr[j-1][1];
            double lon2 = arr[j][1];

            total += haversine(lat1, lat2, lon1, lon2);
        }
        return total;
    }
    else return 0;
}

float getTrackLen(const Track * tr) {
    if(tr!= NULL) {
    
        ListIterator segments = createIterator(tr->segments);
        TrackSegment * seg = NULL;
        int length_segments = getLength(tr->segments);
        int segment_num = 0;
        double endpoints[length_segments][2];

        float total = 0.0;
        while((seg = nextElement(&segments)) != NULL) {
            ListIterator waypoints = createIterator(seg->waypoints);
            Waypoint * wpt = NULL;
            int length = getLength(seg->waypoints);
            double arr[length][2];
            
            int i = 0;

            while((wpt = nextElement(&waypoints)) != NULL) {
                arr[i][0] = wpt->latitude;
                arr[i][1] = wpt->longitude;
                i++;
            }
            for(int j = 1; j < length; j++) {
                double lat1 = arr[j-1][0];
                double lat2 = arr[j][0];
                double lon1 = arr[j-1][1];
                double lon2 = arr[j][1];

                if(length_segments > 1) {
                    if( (segment_num % 2 == 0) && (j == length - 1) ) {
                        endpoints[segment_num][0] = lat2;
                        endpoints[segment_num][1] = lon2;
                    }

                    if( (segment_num % 2 == 1) && (j == 1) ) {
                        endpoints[segment_num][0] = lat1;
                        endpoints[segment_num][1] = lon1;
                    }
                }
             
                total += haversine(lat1, lat2, lon1, lon2);

            }
            segment_num++;
        }

        for(int k = 0; k < length_segments-1; k++) {
            double lat1 = endpoints[k][0];
            double lat2 = endpoints[k+1][0];
            double lon1 = endpoints[k][1];
            double lon2 = endpoints[k+1][1];   
            
            total += haversine(lat1, lat2, lon1, lon2);
        }
        return total;
    }
    else return 0;
}

int numRoutesWithLength(const GPXdoc * doc, float len, float delta) {
    if( (doc == NULL) | (len < 0) | (delta < 0) ) {
        return 0;
    }
    int total = 0;
    ListIterator routes = createIterator(doc->routes);
    Route * rt = NULL;
    while((rt = nextElement(&routes)) != NULL) {
        float len_route = getRouteLen(rt);
        if( fabs(len_route - len) <= delta ) {
            total++;
        } 
    }
    return total;
}

int numTracksWithLength(const GPXdoc * doc, float len, float delta) {
    if( (doc == NULL) | (len < 0) | (delta < 0) ) {
        return 0;
    }
    int total = 0;
    ListIterator tracks = createIterator(doc->tracks);
    Track * tr = NULL;
    while((tr = nextElement(&tracks)) != NULL) {
        float len_track = getTrackLen(tr);
        if( fabs(len_track - len) <= delta ) {
            total++;
        } 
    }
    return total;
}

bool isLoopRoute(const Route * route, float delta) {
    if( (route == NULL) | (delta < 0) ) {
        return false;
    }
    int num_waypoints = getLength(route->waypoints);
    
    if(num_waypoints >= 4) {
       
        double arr[2][2];
        Waypoint * front = getFromFront(route->waypoints);
        Waypoint * back = getFromBack(route->waypoints);
        arr[0][0] = front->latitude; //lat1
        arr[0][1] = front->longitude; //lon1

        arr[1][0] = back->latitude; //lat2
        arr[1][1] = back->longitude; //lon2

        float length = haversine(arr[0][0], arr[1][0], arr[0][1], arr[1][1]);

        if(fabs(length) <= delta) { //FIX: is it <= or < 
            return true;
        }
        else return false;
    }
    else return false;
}

bool isLoopTrack(const Track * tr, float delta) {
    if( (tr == NULL) | (delta < 0) ) return false;

    int num_segments = getLength(tr->segments);

    ListIterator segments = createIterator(tr->segments);
    TrackSegment * ts = NULL;
    double arr[2][2];

    if(num_segments >= 1) {
        while((ts = nextElement(&segments)) != NULL) {
            
            int num_waypoints = getLength(ts->waypoints);

            if(num_waypoints >= 4) {
                
                Waypoint * front = getFromFront(ts->waypoints);
                Waypoint * back = getFromBack(ts->waypoints);

                arr[0][0] = front->latitude; //lat1
                arr[0][1] = front->longitude; //lon1

                arr[1][0] = back->latitude; //lat2
                arr[1][1] = back->longitude; //lon2
            }
        }
    }
    else return false;

    float length = haversine(arr[0][0], arr[1][0], arr[0][1], arr[1][1]);
    
    if(fabs(length) <= delta) { //FIX: is it <= or < 
        return true;
    }
    else return false;
}

List* getRoutesBetween(const GPXdoc* doc, float sourceLat, float sourceLong, float destLat, float destLong, float delta) {
    List * routes = NULL;
    
    if((doc == NULL)) return NULL;
     
    char * (*rtStr)(void*) = &routeToString;
    void (*rtDel)(void*) = &delete_route_dummy;
    int (*rtCmp)(const void*, const void*) = &compareRoutes;
    routes = initializeList(rtStr, rtDel, rtCmp);

    ListIterator rts = createIterator(doc->routes);
    Route * rt = NULL;

    while((rt = nextElement(&rts)) != NULL) {
        int num_waypoints = getLength(rt->waypoints);
        
        if(num_waypoints > 1) {
            Waypoint * front = getFromFront(rt->waypoints);
            Waypoint * back = getFromBack(rt->waypoints);

            float lat1 = front->latitude; //lat1
            float lon1 = front->longitude; //lon1
         
            float lat2 = back->latitude; //lat2
            float lon2 = back->longitude; //lon2   

            float len_between_source = haversine(lat1, sourceLat, lon1, sourceLong);
            float len_between_dest = haversine(lat2, destLat, lon2, destLong);
       
            if( (fabs(len_between_source) <= delta) ) {
                if( (fabs(len_between_dest) <= delta) ) {
                    insertBack(routes, (void*)(rt)); 
                }
            }        
        }
    }
    if(getLength(routes) == 0) {
        freeList(routes);
        return NULL;
    } 
    else return routes;
}

List* getTracksBetween(const GPXdoc* doc, float sourceLat, float sourceLong, float destLat, float destLong, float delta) {
    List * tracks = NULL;
    
    if((doc == NULL)) return NULL;

    char * (*trStr)(void*) = &trackToString;
    void (*trDel)(void*) = &delete_track_dummy;
    int (*trCmp)(const void*, const void*) = &compareTracks;
    tracks = initializeList(trStr, trDel, trCmp);

    ListIterator trs = createIterator(doc->tracks);
    Track * tr = NULL;
    
    while((tr = nextElement(&trs)) != NULL) {
        int num_segments = getLength(tr->segments);
        Waypoint * front_waypoint = NULL;
        Waypoint * back_waypoint = NULL;

        if(num_segments > 1) {
            TrackSegment * front_seg = getFromFront(tr->segments);
            TrackSegment * back_seg = getFromBack(tr->segments);
            
            front_waypoint = getFromFront(front_seg->waypoints);
            back_waypoint = getFromBack(back_seg->waypoints);
        }
        else if(num_segments == 1) {
            TrackSegment * front_seg = getFromFront(tr->segments);

            front_waypoint = getFromFront(front_seg->waypoints);
            back_waypoint = getFromBack(front_seg->waypoints);   
        }

        float lat1 = front_waypoint->latitude; //lat1
        float lon1 = front_waypoint->longitude; //lon1
        
        float lat2 = back_waypoint->latitude; //lat2
        float lon2 = back_waypoint->longitude; //lon2   

        float len_between_source = haversine(lat1, sourceLat, lon1, sourceLong);
        float len_between_dest = haversine(lat2, destLat, lon2, destLong);

        if( (fabs(len_between_source) <= delta) ) {
            if( (fabs(len_between_dest) <= delta) ) {
                insertBack(tracks, (void*)(tr)); 
            }
        }        
    }
    if(getLength(tracks) == 0) {
        freeList(tracks);
        return NULL;
    } 
    else return tracks;
}

// A2 Module 3 Functions

char* routeToJSON(const Route *rt) {
    if(rt != NULL) {
        char * temp = createEmptyString();
        char * name = createEmptyString();
        char * numVal = createEmptyString();
        char * len = createEmptyString();
        char * loop = createEmptyString();

        if(checkStringEmpty(rt->name)) catString("None", &name);
        
        else catString(rt->name, &name);
    
        if(isLoopRoute(rt, 10)) catString("true", &loop);
        
        else catString("false", &loop);

        char d[50];
        sprintf(d, "%.1f", round10(getRouteLen(rt)));
        catString(d, &len);

        char d2[50];
        sprintf(d2, "%d", getLength(rt->waypoints));
        catString(d2, &numVal);

        catString("{\"name\":\"", &temp);
        catString(name, &temp);

        catString("\",\"numPoints\":", &temp);
        catString(numVal, &temp);

        catString(",\"len\":", &temp);
        catString(len, &temp);

        catString(",\"loop\":", &temp);
        catString(loop, &temp);
        catString("}", &temp);

        free(numVal);
        free(name);
        free(len);
        free(loop);

        return temp;
    }
    else{
        char * temp = createEmptyString();
        catString("{}", &temp);
        return temp;
    }
}

char * trackToJSON(const Track *tr) {
    if(tr != NULL) {
        char * temp = createEmptyString();
        char * name = createEmptyString();
        char * len = createEmptyString();
        char * loop = createEmptyString();

        if(checkStringEmpty(tr->name)) catString("None", &name);
        
        else catString(tr->name, &name);

        if(isLoopTrack(tr, 10)) catString("true", &loop);
        
        else catString("false", &loop);

        char d[50];
        sprintf(d, "%.1f", round10(getTrackLen(tr)));
        catString(d, &len);

        catString("{\"name\":\"", &temp);
        catString(name, &temp);

        catString("\",\"len\":", &temp);
        catString(len, &temp);

        catString(",\"loop\":", &temp);
        catString(loop, &temp);
        catString("}", &temp);

        free(name);
        free(len);
        free(loop);

        return temp;
    }
    else {
        char * temp = createEmptyString();
        catString("{}", &temp);
        return temp;
    }
}

char* routeListToJSON(const List *list) {
    if(list != NULL) {
        char * temp = createEmptyString();
        int num_routes = getLength((List*)(list));
        
        if( (num_routes >= 1) ) {
            ListIterator rts = createIterator((List*)(list));
            Route * rt = NULL;
            catString("[", &temp);
            int i = 0;
            
            while((rt = nextElement(&rts)) != NULL) {
                if( (i == (num_routes - 1)) ) {
                    char * r = routeToJSON(rt);
                    catString(r, &temp);
                    free(r);
                }
                else {
                    char * r = routeToJSON(rt);
                    catString(r, &temp);
                    catString(",", &temp);
                    free(r);
                }
                i++;
            }
            catString("]", &temp);
            return temp;
        }
        else{
            catString("[]", &temp);
            return temp;
        }
    }
    else {
        char * temp = createEmptyString();
        catString("[]", &temp);
        return temp;
    }
}

char* trackListToJSON(const List *list) {
    if(list != NULL) {
        char * temp = createEmptyString();
        int num_tracks = getLength((List*)(list));

        if( (num_tracks >= 1) ) {
            ListIterator trs = createIterator((List*)(list));
            Track * tr = NULL;
            catString("[", &temp);
            int i = 0;

            while((tr = nextElement(&trs)) != NULL) {
                if( (i == (num_tracks - 1)) ) {
                    char * t = trackToJSON(tr);
                    catString(t, &temp);
                    free(t);
                }
                else{
                    char * t = trackToJSON(tr);
                    catString(t, &temp);
                    catString(",", &temp);
                    free(t);
                }
                i++;
            }
            catString("]", &temp);
            return temp;
        }
        else {
            catString("[]", &temp);
            return temp;
        }
    }
    else{
        char * temp = createEmptyString();
        catString("[]", &temp);
        return temp;
    }
}

char* GPXtoJSON(const GPXdoc* gpx) {
    if(gpx != NULL) {
        char * temp = createEmptyString();
        char * version = createEmptyString();
        char * creator = createEmptyString();
        char * waypoint_num = createEmptyString();
        char * track_num = createEmptyString();
        char * route_num = createEmptyString();

        catString(gpx->creator, &creator);
        
        char d[50];
        sprintf(d, "%.1f", gpx->version);
        catString(d, &version);

        char w[20];
        sprintf(w, "%d", getNumWaypoints(gpx));
        catString(w, &waypoint_num);

        char r[20];
        sprintf(r, "%d", getNumRoutes(gpx));
        catString(r, &route_num);

        char t[20];
        sprintf(t, "%d", getNumTracks(gpx));
        catString(t, &track_num);

        catString("{\"version\":", &temp);
        catString(version, &temp);

        catString(",\"creator\":\"", &temp);
        catString(creator, &temp);

        catString("\",\"numWaypoints\":", &temp);
        catString(waypoint_num, &temp);

        catString(",\"numRoutes\":", &temp);
        catString(route_num, &temp);

        catString(",\"numTracks\":", &temp);
        catString(track_num, &temp);
        catString("}", &temp);

        free(version);
        free(creator);
        free(waypoint_num);
        free(track_num);
        free(route_num);

        return temp;
    }
    else {
        char * temp = createEmptyString();
        catString("{}", &temp);
        return temp;
    }
}

// A2 Module 3 Bonus Functions

void addWaypoint(Route *rt, Waypoint *pt) {}

void addRoute(GPXdoc* doc, Route* rt) {}

GPXdoc* JSONtoGPX(const char* gpxString) {return NULL;}

Waypoint* JSONtoWaypoint(const char* gpxString) {return NULL;}

Route* JSONtoRoute(const char* gpxString) {return NULL;}
