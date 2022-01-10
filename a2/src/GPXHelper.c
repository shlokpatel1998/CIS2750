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

bool is_valid_tree(char * fileName, char * gpxSchemaFile) {
    LIBXML_TEST_VERSION;

    xmlDoc *doc = NULL;
    xmlSchema *schema = NULL;
    xmlSchemaParserCtxtPtr ctxt;
    xmlLineNumbersDefault(1); 
    
    ctxt = xmlSchemaNewParserCtxt(gpxSchemaFile);
    xmlSchemaSetParserErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);

    schema = xmlSchemaParse(ctxt);
    xmlSchemaFreeParserCtxt(ctxt);

    doc = xmlReadFile(fileName, NULL, 0);

    if(doc == NULL) { //FIX
        xmlSchemaFree(schema);
        xmlCleanupParser();
        return false;
    }
    else {
        xmlSchemaValidCtxtPtr ctxt;
        int ret;

        ctxt = xmlSchemaNewValidCtxt(schema);
        xmlSchemaSetValidErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
        ret = xmlSchemaValidateDoc(ctxt, doc);

        if(ret != 0) { //if invalid, free everything and return NULL;
            xmlSchemaFreeValidCtxt(ctxt);
            xmlFreeDoc(doc);
            xmlSchemaFree(schema);
            xmlSchemaCleanupTypes();
            xmlCleanupParser();
            xmlMemoryDump();
            return false;
        }
        xmlSchemaFreeValidCtxt(ctxt);
        xmlFreeDoc(doc);
    }

    // free the resource
    if(schema != NULL) {
        xmlSchemaFree(schema);
        xmlSchemaCleanupTypes();
        xmlCleanupParser();
        xmlMemoryDump();
    }
    return true;
}

bool is_valid_xmlDoc(xmlDoc * doc, char * gpxSchemaFile) {
    LIBXML_TEST_VERSION;

    xmlSchema *schema = NULL;
    xmlSchemaParserCtxtPtr ctxt;
    xmlLineNumbersDefault(1);
    
    ctxt = xmlSchemaNewParserCtxt(gpxSchemaFile);
    xmlSchemaSetParserErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);

    schema = xmlSchemaParse(ctxt);
    xmlSchemaFreeParserCtxt(ctxt);
    
    if(true) {
        xmlSchemaValidCtxtPtr ctxt;
        int ret;

        ctxt = xmlSchemaNewValidCtxt(schema);
        xmlSchemaSetValidErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
        ret = xmlSchemaValidateDoc(ctxt, doc);

        if(ret != 0) { //if invalid, free everything and return NULL;
            xmlSchemaFreeValidCtxt(ctxt);
            xmlSchemaFree(schema);
            xmlSchemaCleanupTypes();
            xmlCleanupParser();
            xmlMemoryDump();
            return false;
        }
        xmlSchemaFreeValidCtxt(ctxt);
    }
    
    // free the resource
    if(schema != NULL) {
        xmlSchemaFree(schema);
        xmlSchemaCleanupTypes();
        xmlCleanupParser();
        xmlMemoryDump();
    }
    return true;
}

bool is_valid_waypoints(xmlNodePtr * root, GPXdoc * doc) {
    xmlNodePtr root_node = *root;

    ListIterator waypoints = createIterator(doc->waypoints);
    Waypoint * elem_1 = NULL;
    while((elem_1 = nextElement(&waypoints)) != NULL) {
        
        xmlNodePtr wpt = xmlNewChild(root_node, NULL, BAD_CAST "wpt", NULL);
        
        if(elem_1->latitude >= -90 && elem_1->latitude <= 90) {
            char temp[50];
            sprintf(temp, "%f", elem_1->latitude); 
            xmlNewProp(wpt, BAD_CAST "lat", BAD_CAST temp);
        }
        else return false;

        if(elem_1->longitude >= -180 && elem_1->longitude <= 180) {
            char temp[50];
            sprintf(temp, "%f", elem_1->longitude); 
            xmlNewProp(wpt, BAD_CAST "lon", BAD_CAST temp);
        }
        else return false;

        if(elem_1->name == NULL) return false;
        else xmlNewChild(wpt, NULL, BAD_CAST "name", BAD_CAST elem_1->name); //FIX ID:1

        if(elem_1->otherData != NULL) {
            ListIterator wpt_data = createIterator(elem_1->otherData);
            GPXData * data = NULL;
            while((data = nextElement(&wpt_data)) != NULL) {
                if(checkStringEmpty(data->name)) return false;
                if(checkStringEmpty(data->value)) return false;
                
                xmlNewChild(wpt, NULL, BAD_CAST data->name, BAD_CAST data->value);
            }
        }
        else return false;
    }
    return true;
}

bool is_valid_routes(xmlNodePtr * root, GPXdoc * doc) {
    xmlNodePtr root_node = *root;
    
    ListIterator routes = createIterator(doc->routes);
    Route * elem_2 = NULL;
    while((elem_2 = nextElement(&routes)) != NULL) {
        
        xmlNodePtr rte = xmlNewChild(root_node, NULL, BAD_CAST "rte", NULL);
    
        if(elem_2->name == NULL) return false;
        else xmlNewChild(rte, NULL, BAD_CAST "name", BAD_CAST elem_2->name);

        if(elem_2->otherData != NULL) {
            ListIterator rt_data = createIterator(elem_2->otherData);
            GPXData * data = NULL;
            while((data = nextElement(&rt_data)) != NULL) {
                if(checkStringEmpty(data->name)) return false;
                if(checkStringEmpty(data->value)) return false;
                
                xmlNewChild(rte, NULL, BAD_CAST data->name, BAD_CAST data->value);
            }
        }
        else return false;

        if(elem_2->waypoints != NULL) {
            ListIterator route_wpts = createIterator(elem_2->waypoints);
            Waypoint * rte_wpt = NULL;
            while((rte_wpt = nextElement(&route_wpts)) != NULL) {
                
                xmlNodePtr rpt = xmlNewChild(rte, NULL, BAD_CAST "rtept", NULL);
                
                if(rte_wpt->latitude >= -90 && rte_wpt->latitude <= 90) {
                    char temp[50];
                    sprintf(temp, "%f", rte_wpt->latitude); 
                    xmlNewProp(rpt, BAD_CAST "lat", BAD_CAST temp);
                }
                else return false;

                if(rte_wpt->longitude >= -180 && rte_wpt->longitude <= 180) {
                    char temp[50];
                    sprintf(temp, "%f", rte_wpt->longitude); 
                    xmlNewProp(rpt, BAD_CAST "lon", BAD_CAST temp);
                }
                else return false;

                if(rte_wpt->name == NULL) return false;
                else xmlNewChild(rpt, NULL, BAD_CAST "name", BAD_CAST rte_wpt->name); //FIX ID:1

                if(rte_wpt->otherData != NULL) {
                    ListIterator wpt_data = createIterator(rte_wpt->otherData);
                    GPXData * data = NULL;
                    while((data = nextElement(&wpt_data)) != NULL) {
                        if(checkStringEmpty(data->name)) return false;
                        if(checkStringEmpty(data->value)) return false;
                    
                        xmlNewChild(rpt, NULL, BAD_CAST data->name, BAD_CAST data->value);
                    }
                }
                else return false;
            }
        }
        else return false;
    }
    return true;
}

bool is_valid_tracks(xmlNodePtr * root, GPXdoc * doc) {
    xmlNodePtr root_node = *root;

    ListIterator tracks = createIterator(doc->tracks);
    Track * elem_3 = NULL;
    while((elem_3 = nextElement(&tracks)) != NULL) {

        xmlNodePtr trk = xmlNewChild(root_node, NULL, BAD_CAST "trk", NULL);

        if(elem_3->name == NULL) return false;
        else xmlNewChild(trk, NULL, BAD_CAST "name", BAD_CAST elem_3->name); //FIX ID:1

        if(elem_3->otherData != NULL) {
            
            ListIterator tr_data = createIterator(elem_3->otherData);
            GPXData * data = NULL;
            while((data = nextElement(&tr_data)) != NULL) {
                if(checkStringEmpty(data->name)) return false;
                if(checkStringEmpty(data->value)) return false;
                
                xmlNewChild(trk, NULL, BAD_CAST data->name, BAD_CAST data->value);
            }
        }
        else return false;

        if(elem_3->segments != NULL) {
            ListIterator trk_seg = createIterator(elem_3->segments);
            TrackSegment * seg = NULL;
            while((seg = nextElement(&trk_seg)) != NULL) {

                xmlNodePtr trkseg = xmlNewChild(trk, NULL, BAD_CAST "trkseg", NULL);

                if(seg->waypoints != NULL) {

                    ListIterator seg_wpt = createIterator(seg->waypoints);
                    Waypoint * wpt = NULL;
                    while((wpt = nextElement(&seg_wpt)) != NULL) {
                        
                        xmlNodePtr trkpt = xmlNewChild(trkseg, NULL, BAD_CAST "trkpt", NULL);
                        
                        if(wpt->latitude >= -90 && wpt->latitude <= 90) {
                            char temp[50];
                            sprintf(temp, "%f", wpt->latitude); 
                            xmlNewProp(trkpt, BAD_CAST "lat", BAD_CAST temp);
                        }
                        else return false;
        
                        if(wpt->longitude >= -180 && wpt->longitude <= 180) {
                            char temp[50];
                            sprintf(temp, "%f", wpt->longitude); 
                            xmlNewProp(trkpt, BAD_CAST "lon", BAD_CAST temp);
                        }
                        else return false;

                        if(wpt->name == NULL) return false;
                        else xmlNewChild(trkpt, NULL, BAD_CAST "name", BAD_CAST wpt->name); //FIX ID:1
                        
                        if(wpt->otherData != NULL) {
                            ListIterator wpt_data = createIterator(wpt->otherData);
                            GPXData * data = NULL;
                            while((data = nextElement(&wpt_data)) != NULL) {
                                if(checkStringEmpty(data->name)) return false;
                                if(checkStringEmpty(data->value)) return false;
                            
                                xmlNewChild(trkpt, NULL, BAD_CAST data->name, BAD_CAST data->value);
                            }
                        }
                        else return false;
                    }
                }
                else return false;
            }
        }
        else return false;
    }
    return true;
}

bool doc_to_tree(xmlNodePtr * root, GPXdoc * doc) {
    xmlNodePtr root_node = *root;

    char tempDouble[50];
    sprintf(tempDouble, "%.1f", doc->version);   
    xmlNewProp(root_node, BAD_CAST "version", BAD_CAST tempDouble);

    if(doc->creator != NULL) {
        xmlNewProp(root_node, BAD_CAST "creator", BAD_CAST doc->creator);
    }
    else return false; //null creator

    if(checkStringEmpty(doc->namespace)) {
        return false;
    }
    xmlNsPtr ns = xmlNewNs(root_node, BAD_CAST doc->namespace, NULL);
    xmlSetNs(root_node, ns);


    bool valid_gpx_waypoints = is_valid_waypoints(&root_node, doc);
    if(!valid_gpx_waypoints) return false;
    
    bool valid_gpx_routes = is_valid_routes(&root_node, doc);
    if(!valid_gpx_routes) return false;
    
    bool valid_gpx_tracks = is_valid_tracks(&root_node, doc);
    if(!valid_gpx_tracks) return false;

    //if parses everthing then it is valid
    return true;
}

float haversine(double lat1, double lat2, double lon1, double lon2) {
    double R = 6371000;
    double lat1_rad = lat1 * (M_PI / 180);
    double lat2_rad = lat2 * (M_PI / 180);
    double deltaLat = lat2_rad - lat1_rad;
    double deltaLon = (lon2 - lon1) * (M_PI / 180);

    double a = pow(sin(deltaLat/2), 2) + ( cos(lat1_rad)*cos(lat2_rad)*pow(sin(deltaLon/2), 2));

    double c = 2 * atan2(sqrt(a), sqrt(1-a));

    double d = R * c;

    return (float)(d);
}

void delete_route_dummy(void*data) {}

void delete_track_dummy(void*data) {}



