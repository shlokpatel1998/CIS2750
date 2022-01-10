#include <stdio.h>
#include "GPXParser.h"

// Shlok Patel Student # 0979966
//main to run and test functions

int main(int argc, char **argv) {
    
    GPXdoc * newDoc = createValidGPXdoc(argv[1], argv[2]);
    
    if(newDoc != NULL) {
      
      bool valid = validateGPXDoc(newDoc, argv[2]);

      if(valid) {
        bool r = writeGPXdoc(newDoc, "first.gpx"); 
        printf("%d\n", r);
        /*
        float round =  round10(555.45);
        printf("round10: %f\n", round);

        float rlen = getRouteLen(getFromFront(newDoc->routes));
        printf("getRouteLen: %f\n", rlen);

        float tlen = getTrackLen(getFromFront(newDoc->tracks));
        printf("getTrackLen: %f\n", tlen);

        int num_routes = numRoutesWithLength(newDoc, 150, 10.5);
        printf("numRoutesWithLength: %d\n", num_routes);

        int num_tracks = numTracksWithLength(newDoc, 330, 4);
        printf("numTracksWithLength: %d\n", num_tracks);

        Track * track = getTrack(newDoc, "kashmir");
        printf("isLoopTrack: %d\n", isLoopTrack(track, 500));
        
        
        List * route_between = getRoutesBetween(newDoc, 44.595860, -123.620950, 44.597150, -123.620220, 100);
        if(route_between != NULL) {
          printf("len routes: %d\n", getLength(route_between));
        }
        freeList(route_between);


        List * track_between = getTracksBetween(newDoc, 49.595850, -123.620950, 49.598350, -123.619520, 100);
        if(track_between != NULL) {
          printf("len tracks: %d\n", getLength(track_between));
        }
        freeList(track_between);

        Route * rt = getFromFront(newDoc->routes);
        char * routeJ = routeToJSON(rt);
        printf("%s\n", routeJ);
        free(routeJ);

        Track * tr = getFromFront(newDoc->tracks);
        char * trackJ = trackToJSON(tr);
        printf("%s\n", trackJ);
        free(trackJ);
        
        char * rts = routeListToJSON(newDoc->routes);
        printf("%s\n", rts);
        free(rts);

        char * ts = trackListToJSON(newDoc->tracks);
        printf("%s\n", ts);
        free(ts);

        char * f = GPXtoJSON(newDoc);
        printf("%s\n", f);
        free(f);
        */
      
      }
      deleteGPXdoc(newDoc);


    } 

    return 0;
}