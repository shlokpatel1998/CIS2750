// Put all onload AJAX calls here, and event listeners
var files_in_server = '';
localStorage.setItem("file_names", files_in_server);

jQuery(document).ready(function() {
    
    // On page-load AJAX Example

    jQuery.ajax({
        type: 'get',           
        dataType: 'json',       
        url: '/gpxfiles',
        success: function (data) {
            
            $("#store_all").prop('disabled', true);
            $("#clear_all").prop('disabled', true);
            $("#db_status").prop('disabled', true);

            $("#q1_a").prop('disabled', true);
            $("#q1_b").prop('disabled', true);
            $("#q2_a").prop('disabled', true);
            $("#q2_b").prop('disabled', true);
            $("#q3").prop('disabled', true);
            
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
                $('#dropdown_query').append('"<option value="'+parsed["file"]+'">'+parsed["file"]+"</option"); 
                
        
                let n = localStorage.getItem('file_names');
                localStorage.setItem('file_names', n + parsed["file"] + " ");

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

        //only update database if already logged on 
        
        if($('#login_bt').text() == 'Logout') {
            $.ajax({
                type: 'get',
                dataType: 'json',
                url: '/asynRT',
                
                data: {
                    tochange: input, 
                    filename: file,
                    path: being_changed,
                    user     : username_db,
                    password : password_db,
                    database : database_db
                },
    
                success: function(data) {
                    if (data["success"] == true) {
                        alert("Route name successfully updated in database");
                    }
                    else {
                        alert("Failed to update route name in database");
                    }
                },
                fail: function(error) {
                    alert("Failed to update route name in database");
                }
    
            });
        }
            
        


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
                                $('#dropdown_addroutes').append('"<option value="'+parsed["file"]+'">'+parsed["file"]+"</option"); 
                                $('#dropdown_query').append('"<option value="'+parsed["file"]+'">'+parsed["file"]+"</option"); 
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

        if(file == 'Selections') {
            return false;
        }

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

                if($('#login_bt').text() == 'Logout') {
        
                    $.ajax({
                        type: 'get',           
                        dataType: 'json',       
                        url: '/asyncWPT',
                        data: {
                            filename: file,
                            waypoints: arr,
                            user     : username_db,
                            password : password_db,
                            database : database_db
                        },

                        success: function(data) {
                            if (data["success"] == true) {
                                alert("New route successfully added to database");
                                $('#db_status').trigger('click');
                                $('#dropdown').val(file).trigger('change');
                            }
                            else {
                                alert("Failed to add new route to database");
                            }
                        },
                        fail: function(er) {
                            alert("Failed to add new route to database");
                        }
                    });
                
                }
                else {
                    $('#dropdown').val(file).trigger('change');
                }

            },

            fail: function(e) {
                console.log("Route failed to be added");
                alert("Route failed to be added");
            }
        });

        x.splice(0, x.length);
    });

    $("#clear_all").on('click', function(e) {
        e.preventDefault();

        $.ajax({ 
            type: 'get',           
            dataType: 'json',       
            url: '/deleteData',
            data: { 
                user     : username_db,
                password : password_db,
                database : database_db
            },

            success: function(data) {
                if (data["success"] == true) {
                    alert("All data cleared!");
                    $('#db_status').trigger('click');
                }
                else {
                    alert("Failed to clear data from database");
                }
            },


            fail: function(error) {
                alert("Failed to clear data from database");
            }
        });

    });

    $("#store_all").on('click', function(e) {
        e.preventDefault();

        $.ajax({ 
            type: 'get',           
            dataType: 'json',       
            url: '/asynStore',
            data: { 
                user     : username_db,
                password : password_db,
                database : database_db
            },

            success: function(data) {
                if (data["success"] == true) {
                    alert("Store all files successful");
                    $('#db_status').trigger('click');
                }
                else {
                    alert("Error in storing all files: fail");
                }
             },

            fail: function(error) {
                alert("Error in storing all files");
             }


        });
    
    });

    $("#db_status").on('click', function(e) {
        e.preventDefault();
        let a = [];

        $.ajax({ 
            type: 'get',           
            dataType: 'json',       
            url: '/DBStatus',
            data: { 
                user     : username_db,
                password : password_db,
                database : database_db
            },
            success: function(data) {
                if (data["success"] == true) {
                    let numbers = data["values"];

                    alert("The database contains: #FILES:" + numbers[0] + " #ROUTES:" + numbers[1] + " #POINTS:" + numbers[2]);
                    
                }
                else {
                    alert("DB Status Error");
                }
            },
            fail: function(error) {
                alert("DB Status Error");
            }
        });

    
    });


    var num_files;
    var num_routes;
    var num_points;

    var username_db;
    var password_db;
    var database_db;

    $("#login_bt").on('click', function(e) {
        e.preventDefault();

        let username = $('#name').val();
        let pass = $('#password').val();
        let db_name = $('#db_name').val();

        if( (!username || username.length === 0) ||  (!pass || pass.length === 0) || (!db_name || db_name.length === 0) ) {
            alert('All 3 fields need to be filled');
        }

        else {
            //if logged in already -> logout

            $.ajax({
                type: 'get',           
                dataType: 'json',       
                url: '/asynDBFiles',
                data: {
                    user     : username,
                    password : pass,
                    database : db_name
                },
                success: function(data) {
                    if (data["success"] == true) {
                           
                        alert("Login Successful!");

                        $('#login_bt').html('Logout');

                        $("#name").prop("disabled", true);
                        $("#password").prop("disabled", true);
                        $("#db_name").prop("disabled", true);

                        $("#store_all").prop("disabled", false);
                        $("#clear_all").prop('disabled', false);
                        $("#db_status").prop('disabled', false);

                        $("#q1_a").prop('disabled', false);
                        $("#q1_b").prop('disabled', false);
                        $("#q2_a").prop('disabled', false);
                        $("#q2_b").prop('disabled', false);
                        $("#q3").prop('disabled', false);

                        username_db = username;
                        password_db = pass;
                        database_db = db_name;

                        $('#db_status').trigger('click');
                

                    }
                    else {
                        alert("Login failed, please try again!")
                    }
                
                },

                fail: function(error) {
                    alert("login failed, try again");
                }

            
            });

        }

        $('#creds').trigger("reset");

    });


    $("#q1_a").on('click', function(e) {
        e.preventDefault();

        $('#qtable tbody').empty();
        $('#qtable thead').empty();

        $.ajax({ 
            type: 'get',           
            dataType: 'json',       
            url: '/query1',
            data: { 
                user     : username_db,
                password : password_db,
                database : database_db,
                sort: 'length',
                q: 1
            },

            success: function(data) { 
                let rows = data["row"];
                $('#qtable').find('thead').append('<tr><th>route_id</th><th>route_name</th><th>route_len</th><th>gpx_id</th></tr>'); 
                for(let i = 0; i < rows.length; i++) {
                    $('#qtable').find('tbody').append('<tr><td>' + rows[i]["route_id"] + '</td><td>' + rows[i]["route_name"] +
                                                        '</td><td>' + rows[i]["route_len"] + '</td><td>' + rows[i]["gpx_id"] + '</td></tr>'); 
                }
                alert("Success: Check Query Table")
            },

            fail: function(error) { 
                console.log("Failed to execute query 1a");
                alert("Failed to execute query 1a");
            }

        });
    });

    $("#q1_b").on('click', function(e) {
        e.preventDefault();

        $('#qtable tbody').empty();
        $('#qtable thead').empty();

        $.ajax({ 
            type: 'get',           
            dataType: 'json',       
            url: '/query1',
            data: { 
                user     : username_db,
                password : password_db,
                database : database_db,
                sort: 'name',
                q: 1
            },

            success: function(data) { 
                let rows = data["row"];
                $('#qtable').find('thead').append('<tr><th>route_id</th><th>route_name</th><th>route_len</th><th>gpx_id</th></tr>'); 
                for(let i = 0; i < rows.length; i++) {
                    $('#qtable').find('tbody').append('<tr><td>' + rows[i]["route_id"] + '</td><td>' + rows[i]["route_name"] +
                                                        '</td><td>' + rows[i]["route_len"] + '</td><td>' + rows[i]["gpx_id"] + '</td></tr>'); 
                }
                alert("Success: Check Query Table")
            },

            fail: function(error) { 
                console.log("Failed to execute query 1b");
                alert("Failed to execute query 1b");
            }

        });
    });

    $("#q2_a").on('click', function(e) {
        e.preventDefault();

        $('#qtable tbody').empty();
        $('#qtable thead').empty();

        var file = $("#dropdown_query :selected").text();

        if(file == 'Selections') {
            return false;
        }
        
        $.ajax({ 
            type: 'get',           
            dataType: 'json',       
            url: '/query1',
            data: { 
                user     : username_db,
                password : password_db,
                database : database_db,
                sort: 'length',
                filename: file,
                q: 2
            },

            success: function(data) {
                let rows = data["row"];
                $('#qtable').find('thead').append('<tr><th>route_id</th><th>route_name</th><th>route_len</th><th>gpx_id</th></tr>'); 
                for(let i = 0; i < rows.length; i++) {
                    $('#qtable').find('tbody').append('<tr><td>' + rows[i]["route_id"] + '</td><td>' + rows[i]["route_name"] +
                                                        '</td><td>' + rows[i]["route_len"] + '</td><td>' + rows[i]["gpx_id"] + '</td></tr>'); 
                }
                alert("Success: Check Query Table");
            },

            fail: function(error) { 
                console.log("Failed to execute query 2a");
                alert("Failed to execute query 2a");
            }
        });

    });

    $("#q2_b").on('click', function(e) {
        e.preventDefault();

        $('#qtable tbody').empty();
        $('#qtable thead').empty();

        var file = $("#dropdown_query :selected").text();

        if(file == 'Selections') {
            return false;
        }

        $.ajax({ 
            type: 'get',           
            dataType: 'json',       
            url: '/query1',
            data: { 
                user     : username_db,
                password : password_db,
                database : database_db,
                sort: 'name',
                filename: file,
                q: 2
            },

            success: function(data) {
                let rows = data["row"];
                $('#qtable').find('thead').append('<tr><th>route_id</th><th>route_name</th><th>route_len</th><th>gpx_id</th></tr>'); 
                for(let i = 0; i < rows.length; i++) {
                    $('#qtable').find('tbody').append('<tr><td>' + rows[i]["route_id"] + '</td><td>' + rows[i]["route_name"] +
                                                        '</td><td>' + rows[i]["route_len"] + '</td><td>' + rows[i]["gpx_id"] + '</td></tr>'); 
                }
                alert("Success: Check Query Table");
            },

            fail: function(error) { 
                console.log("Failed to execute query 2b");
                alert("Failed to execute query 2b");
            }

        });
    });

    var route_id = "empty";

    $("#q3").on('click', function(e) { 
        e.preventDefault();

        $('#qtable tbody').empty();
        $('#qtable thead').empty();

        if(route_id == 'empty') {
            return false;
        }

        $.ajax({ 
            type: 'get',           
            dataType: 'json',       
            url: '/query1',
            data: { 
                user     : username_db,
                password : password_db,
                database : database_db,
                route: route_id,
                q: 3
            },

            success: function(data) {
                let rows = data["row"];
                $('#qtable').find('thead').append('<tr><th>point_id</th><th>point_index</th><th>latitude</th><th>longitude</th><th>point_name</th><th>route_id</th></tr>'); 
                for(let i = 0; i < rows.length; i++) {
                    $('#qtable').find('tbody').append('<tr><td>' + rows[i]["point_id"] + '</td><td>' + rows[i]["point_index"] +
                                                        '</td><td>' + rows[i]["latitude"] + '</td><td>' + rows[i]["longitude"] + '</td><td>' + rows[i]["point_name"] +
                                                        '</td><td>' + rows[i]["route_id"]  +  '</td></tr>'); 
                }
                alert("Success: Check Query Table");
            },

            fail: function(error) { 
                console.log("Failed to execute query 2a");
                alert("Failed to execute query 2a");
            }
        });

    });

    $('#choose_route_q').submit(function(e){ 
        let input = $('#entryRoute').val();
        e.preventDefault();
        route_id = input;

        $("#choose_route_q").trigger("reset");
    });

    
});