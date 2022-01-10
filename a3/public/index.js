// Put all onload AJAX calls here, and event listeners
jQuery(document).ready(function() {
    
    // On page-load AJAX Example
    jQuery.ajax({
        type: 'get',           
        dataType: 'json',       
        url: '/gpxfiles',
        success: function (data) {
            if(data.length == 0) {
                alert("No files");
            }
            for(var i = 0; i < data.length; i++) {
                let parsed = JSON.parse(data[i]);
                $('#files').append('"<tr><td><a href="uploads/'+parsed["file"]+'">'+parsed["file"]+"</a></td>"+
                                    "<td>"+parsed["version"]+"</td><td>"+parsed["creator"]+"</td><td>"+
                                    parsed["numWaypoints"]+"</td><td>"+parsed["numRoutes"]+"</td><td>"+parsed["numTracks"]+"</td></tr>");
                
                $('#dropdown').append('"<option value="'+parsed["file"]+'">'+parsed["file"]+"</option"); 
                $('#dropdown_addroutes').append('"<option value="'+parsed["file"]+'">'+parsed["file"]+"</option"); 
                

                alert("File: "+parsed["file"]+" successfully added!");
            }

        },
        fail: function(error) {
            alert("File could not be added!");
        }
    });



    $('#dropdown').on('change', function(event) {
        var file = this.value;
        $('#dropdown_rename').empty();
        $('#viewtable tbody').empty();
        $.ajax({
            type: 'get',
            dataType: 'json',
            url: '/viewfiles',
            data: {
                filename: file
            },

            success: function(data) {

                let routes = JSON.parse(data[0]);
                let tracks = JSON.parse(data[1]);

                for(let i = 0; i < routes.length; i++) {
                    let parsed = routes[i];
                    if (parsed["name"] == "None") {
                        parsed["name"] = "";
                    }                                           
                    $('#viewtable').find('tbody').append('"<tr class="rows"><td>'+"Route "+(i+1)+"</td><td>"+parsed["name"]
                                                        +"</td><td>"+parsed["numPoints"]+"</td><td>"
                                                        +parsed["len"]+"m</td><td>"+parsed["loop"]+"</td>"+
                                                        '"<td><button type="button" class="btn2"></button></td></tr>"');  
                    $('#dropdown_rename').append('"<option value="Route '+(i+1)+'">Route '+(i+1)+"</option");   
                }
                
                
                for(let i = 0; i < tracks.length; i++) {
                    let parsed = tracks[i];
                    if (parsed["name"] == "None") {
                        parsed["name"] = "";
                    }                                              
                    $('#viewtable').find('tbody').append('"<tr class="rows"><td>'+"Track "+(i+1)+"</td><td>"+parsed["name"]
                                                +"</td><td>"+parsed["numPoints"]+"</td><td>"
                                                +parsed["len"]+"m</td><td>"+parsed["loop"]+"</td>"+
                                                '"<td><button type="button" class="btn2"></button></td></tr>"');   
                     
                    $('#dropdown_rename').append('"<option value="Track '+(i+1)+'">Track '+(i+1)+"</option"); 

                }
                console.log("Displayed all routes/files");
                
            },

            fail: function(error) {
                console.log("error: /viewfiles");
                alert(error);
            }
        });
        
    });


    $("#viewtable").on('click','.btn2',function(){
        console.log(this.value);

        var currentRow=$(this).closest("tr"); 
        var col1=currentRow.find("td:eq(0)").text();

        var file = $("#dropdown :selected").text();

        $.ajax({
           type: 'get',
           dataType: 'json',
           url: '/otherdata',

           data: {
               filename: file,
               path: col1
           },

           success: function(data) {
                var a = [];
                for(let i  = 0; i < data.length; i++) {
                    let parsed = data[i];
                    a[i] = parsed["name"] + ": " + parsed["value"] + "\n";
                }
                alert(a);
           },

           fail: function(e) {
                alert(e);
           }
            
        });
    
    });


    $('#renameForm').submit(function(e){
        let input = $('#entryBox').val();
        e.preventDefault();

        var being_changed = $("#dropdown_rename :selected").text();

        var file = $("#dropdown :selected").text();

        console.log(file + being_changed + input);

        $.ajax({
            type: 'get',
            dataType: 'json',
            url: '/changeroutetrack',

            data: {
                tochange: input,
                filename: file,
                path: being_changed
            },
             success: function(data) {
                $('#dropdown :selected').trigger('change');
                console.log( data["success"] + " successfully changed");
             },
            
             fail: function(e) {
                alert(e);
            }
        });
    
    });

    $('#creategpx_forms').submit(function(e){
        e.preventDefault();

        let file = "uploads/" + $('#file_name_gpx').val() + ".gpx";

        let file_disp = $('#file_name_gpx').val() + ".gpx";

        let creator = $('#creator_gpx').val();

        $.ajax({
            type: 'get',
            dataType: 'json',
            url: '/creategpx',

            data: {
                value: creator,
                filename: file,
            },
             success: function(data) {
                console.log("Create new gpx successful");
                jQuery.ajax({
                    type: 'get',           
                    dataType: 'json',       
                    url: '/gpxfiles',

                    data: {
                        creategpx: true
                    },

                    success: function (data) {
                        console.log(data);

                        for(let i = 0; i < data.length; i++) {
                            let parsed = JSON.parse(data[i]);
                            if(parsed["file"] == file_disp) {
                                $('#files').append('"<tr><td><a href="uploads/'+parsed["file"]+'">'+parsed["file"]+"</a></td>"+
                                "<td>"+parsed["version"]+"</td><td>"+parsed["creator"]+"</td><td>"+
                                parsed["numWaypoints"]+"</td><td>"+parsed["numRoutes"]+"</td><td>"+parsed["numTracks"]+"</td></tr>");
            
                                $('#dropdown').append('"<option value="'+parsed["file"]+'">'+parsed["file"]+"</option"); 

                            }
                        }
                        
                        alert("File: " + file_disp +" successfully added!");
                    },

                    fail: function(error) {
                        alert("File could not be added!");
                    }
                });

             },
            
             fail: function(e) {
                alert(e);
            }
        });
        


    });

    var x = [];

    $("#addpt").on('click', function(e) {
        e.preventDefault();

        let lat = $('#lat').val();
        let lon = $('#lon').val();

        if( (!lat || lat.length === 0) ||  (!lon || lon.length === 0) ) {
            alert('Both lat and lon values needed');
        }

        else if( (isNaN(parseFloat(lat))) || (isNaN(parseFloat(lon))) ) {
            alert('Enter numerical values for longitude and latitude');
        }

        else if ( parseFloat(lat) < -90 || parseFloat(lat) > 90) {     
            alert('Wrong range lat(-90 to 90)');    
        }

        else if(parseFloat(lon) < -180 || parseFloat(lon) > 180) {
            alert('Wrong range lon(-180 to 180)');
        }

        else{
            x.push(lat + " " + lon);
            alert('Waypoint added');
        }

        $('#createnewpoint').trigger("reset");
        
    });


    $("#addrt").on('click', function(e) {
        e.preventDefault();
        var arr = "";

        var file = $("#dropdown_addroutes :selected").text();

        for(let i = 0; i < x.length; i++) {
            let y = x[i].split(" ");
            let lat = y[0];
            let lon = y[1];

            arr = arr.concat(lat + " "+ lon + " ");
        }

        $.ajax({
            type: 'get',           
            dataType: 'json',       
            url: '/addroute',
            data: {
                filename: file,
                waypoints: arr
            },

            success: function(data) {
                alert("Route added");


                
            },

            fail: function(e) {
                console.log("Route failed to be added");
                alert("Route failed to be added");
            }

        });

        x.splice(0, x.length);
    });
    

    
});