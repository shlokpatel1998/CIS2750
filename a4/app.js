'use strict'

// C library API
const ffi = require('ffi-napi');

const mysql = require('mysql2/promise');

// Express App (Routes)
const express = require("express");
const app     = express();
const path    = require("path");
const fileUpload = require('express-fileupload');

app.use(fileUpload());
app.use(express.static(path.join(__dirname+'/uploads')));

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

// Send HTML at root, do not change
app.get('/',function(req,res){
  res.sendFile(path.join(__dirname+'/public/index.html'));
});

// Send Style, do not change
app.get('/style.css',function(req,res){
  //Feel free to change the contents of style.css to prettify your Web app
  res.sendFile(path.join(__dirname+'/public/style.css'));
});

// Send obfuscated JS, do not change
app.get('/index.js',function(req,res){
  fs.readFile(path.join(__dirname+'/public/index.js'), 'utf8', function(err, contents) {
    const minimizedContents = JavaScriptObfuscator.obfuscate(contents, {compact: true, controlFlowFlattening: true});
    res.contentType('application/javascript');
    res.send(minimizedContents._obfuscatedCode);
  });
});

//Respond to POST requests that upload files to uploads/ directory
app.post('/upload', function(req, res) {
  if(!req.files) {
    return res.status(400).send('No files were uploaded.');
  }
 
  let uploadFile = req.files.uploadFile;
 
  // Use the mv() method to place the file somewhere on your server
  uploadFile.mv('uploads/' + uploadFile.name, function(err) {
    if(err) {
      return res.status(500).send(err);
    }

    res.redirect('/');
  });
});

//Respond to GET requests for files in the uploads/ directory
app.get('/uploads/:name', function(req , res){
  fs.stat('uploads/' + req.params.name, function(err, stat) {
    if(err == null) {
      res.sendFile(path.join(__dirname+'/uploads/' + req.params.name));
    } else {
      console.log('Error in file downloading route: '+err);
      res.send('');
    }
  });
});

//******************** Your code goes here ******************** 

let dbConf = {
	host     : 'dursley.socs.uoguelph.ca',
	user     : 'shlok',
	password : '0979966',
	database : 'shlok'
};


let lib = ffi.Library('./libgpxparser', {
 'gpx_to_html': ['string', ['string'] ],

 'route_list': ['string', ['string'] ],

 'track_list': ['string', ['string'] ],

 'gpxRouteDataListToJSON': ['string', ['string', 'int'] ],

 'gpxTrackDataListToJSON': ['string', ['string', 'int'] ],

 'change_route': ['void', ['string', 'int', 'string'] ],

 'change_track': ['void', ['string', 'int', 'string'] ],

 'create_gpx_file': ['void', ['string', 'string'] ],

 'route_add': ['void', ['string', 'string'] ],

 'wpt_list_html': ['string', ['string'] ] 

});

app.get('/gpxfiles', function(req , res){
    
  let a = [];

  let files = fs.readdirSync('./uploads');

  if(req.query.creategpx === true) {
    let len = files.length;
    let h = lib.gpx_to_html(files[len-1]);
    
    if(h == "Invalid") {
    }
    
    else {
      let obj = JSON.parse(h);
      obj["file"] = files[i];

      let str = JSON.stringify(obj);
      a[0] = str;
    }
    res.send(a);
  }

  
  for(var i = 0; i < files.length; i++) {
    
    let h = lib.gpx_to_html(files[i]);

    if(h == "Invalid") continue;
  
    else {
      let obj = JSON.parse(h);
      obj["file"] = files[i];

      let str = JSON.stringify(obj);
      a[i] = str;
    }
  }

  a = a.filter(function (el) { //to filter out null array elements
    return el != null;
  });

  res.send(a);
});

app.get('/viewfiles', function(req, res) {
  let arr = [];
  let filename = req.query.filename;
  let routelist = lib.route_list(filename);
  arr[0] = routelist;

  let tracklist = lib.track_list(filename);
  arr[1] = tracklist;

  res.send(arr);
});

app.get('/otherdata', function(req,res) {
  let x = req.query.path.split(" "); 
  let file_name = req.query.filename;

  let num = parseInt(x[1]);
  let path = x[0];

  if(path.toUpperCase() == "TRACK") {
    let other_data = lib.gpxTrackDataListToJSON(file_name, num);
    res.send(other_data);
  }
  if(path.toUpperCase() == "ROUTE") {
    let other_data = lib.gpxRouteDataListToJSON(file_name, num);
    res.send(other_data);
  }
});

//Sample endpoint
app.get('/endpoint1', function(req , res){
  let retStr = req.query.data1 + " " + req.query.data2;
  res.send(
    {
      somethingElse: retStr
    }
  );
});

app.get('/changeroutetrack', function(req, res){
  let file_name = req.query.filename;
  let x = req.query.path.split(" "); 
  let num = parseInt(x[1]);
  let path = x[0];

  let changevalue = req.query.tochange;

  if(path.toUpperCase() == "TRACK") {
    lib.change_track(file_name, num, changevalue);
    res.send({success: path});
  }

  if(path.toUpperCase() == "ROUTE") {
    lib.change_route(file_name, num, changevalue);
    res.send({success: path});
  }

});


app.get('/creategpx', function(req, res) {
    let filename = req.query.filename;
    let creator = req.query.value;

    let valid = lib.create_gpx_file(filename, creator);

    if(valid) {
      res.redirect('/');
      res.send({success: true});
    }
    res.send({success: false});
});

app.get('/addroute', function(req, res){
  let filename = req.query.filename;
  let waypoints = req.query.waypoints;

  lib.route_add(filename, waypoints);

  res.send({success: true});

});

let credentials;
let file_query = `CREATE TABLE IF NOT EXISTS FILE (
	gpx_id INT AUTO_INCREMENT, 
	file_name VARCHAR(60) NOT NULL, 
	ver DECIMAL(2,1) NOT NULL, 
	creator VARCHAR(256) NOT NULL, 
  	PRIMARY KEY (gpx_id))`;

let route_query = `CREATE TABLE IF NOT EXISTS ROUTE (
	route_id INT AUTO_INCREMENT, 
	route_name VARCHAR(256), 
	route_len FLOAT(15,7) NOT NULL, 
	gpx_id INT NOT NULL,
	PRIMARY KEY (route_id),
	FOREIGN KEY (gpx_id) REFERENCES FILE(gpx_id) ON DELETE CASCADE)`;

let point_query = `CREATE TABLE IF NOT EXISTS POINT (
	point_id INT AUTO_INCREMENT, 
	point_index INT NOT NULL, 
	latitude DECIMAL(11,7) NOT NULL, 
	longitude DECIMAL(11,7) NOT NULL, 
	point_name VARCHAR(256), 
	route_id INT NOT NULL,
	PRIMARY KEY (point_id), 
	FOREIGN KEY (route_id) REFERENCES ROUTE(route_id) ON DELETE CASCADE)`;


app.get('/asynStore', async function(req, res) {
	let connected = false;
	let connection;
	try{
		credentials = {
			host     : 'dursley.socs.uoguelph.ca',
			user     : req.query.user,
			password : req.query.password,
			database : req.query.database
		};

		connection = await mysql.createConnection(credentials);

		await connection.execute("DELETE FROM POINT");
		await connection.execute("DELETE FROM ROUTE");
		await connection.execute("DELETE FROM FILE");


		let files = fs.readdirSync('./uploads');

		for(var i = 0; i < files.length; i++) {
    
			let h = lib.gpx_to_html(files[i]);
		
			if(h == "Invalid") continue;
		  
			else {
				let obj = JSON.parse(h);
				obj["file"] = files[i];

				let check_dup = 'SELECT file_name FROM FILE WHERE file_name = "' + files[i] + '"';

				let [rows, cols] = await connection.execute(check_dup);

				if(rows.length == 0) {
					let insert_file = 'INSERT IGNORE INTO FILE (gpx_id, file_name, ver, creator) VALUES (null,"' + 
							files[i] + '",' + obj["version"] + ',"' + obj["creator"] + '")';

					await connection.execute(insert_file);

					let get_gpxid = 'SELECT gpx_id FROM FILE WHERE file_name = "' + files[i] + '"';
					
					let [rows2, col2] = await connection.execute(get_gpxid);

					let id = rows2[0].gpx_id;

					let r = lib.route_list(files[i]);

					r = JSON.parse(r);

					let c = lib.wpt_list_html(files[i]);
					c = JSON.parse(c);

					for(let j = 0; j < r.length; j++) {
						let parsed = r[j];
						
						let insert_row  = 'INSERT IGNORE INTO ROUTE (route_id, route_name, route_len, gpx_id) VALUES (null,"' +
										parsed["name"] + '",' + parsed["len"] + ',' + id + ')';

						await connection.execute(insert_row);

						let route_name = parsed["name"];
						
						let get_routeid = 'SELECT route_id FROM ROUTE WHERE route_name = "' + route_name + '"';

						let [rows3, col3] = await connection.execute(get_routeid);

						let rt_id;
	
						rt_id = rows3[0].route_id;
						

						for(let i = 0; i < c.length; i++) {
	
							for(let k = 0; k < c[i].length; k++) {
								let insert_point = 'INSERT IGNORE INTO POINT (point_id, point_index, latitude, longitude' +
										', point_name, route_id) VALUES (null,' + (k+1) + ',' + c[i][k]["lat"] + ',' + c[i][k]["lon"] +
										',"' + c[i][k]["name"] + '",' + rt_id + ")";
																
								await connection.execute(insert_point);

							}

						}
					}
				}
			}
		}
		
		connected = true;

	}catch(e){
		console.log("Query error: "+e);

	}finally {
		if (connection && connection.end) connection.end();
		
		if (connected == true) {
			res.send({success: true});
		}
		else{
			res.send({success: false});
		}
	}

});


app.get('/asynDBFiles', async function(req, res) {
	let connected = false;
	let connection;

	try{
		credentials = {
			host     : 'dursley.socs.uoguelph.ca',
			user     : req.query.user,
			password : req.query.password,
			database : req.query.database
		};

		connection = await mysql.createConnection(credentials);

		await connection.execute(file_query);
		await connection.execute(route_query);
		await connection.execute(point_query);

		connected = true;

	}catch(e){
		console.log("Query error: "+e);

	}finally {
		if (connection && connection.end) connection.end();
		
		if (connected == true) {
			res.send({success: true});
		}
		else{
			res.send({success: false});
		}
	}

});

app.get('/asyncWPT', async function(req, res) {
	let connected = false;
	let connection;
	try{
		credentials = {
			host     : 'dursley.socs.uoguelph.ca',
			user     : req.query.user,
			password : req.query.password,
			database : req.query.database
		};

		connection = await mysql.createConnection(credentials);

		let filename = req.query.filename;
  		let waypoints = req.query.waypoints;
		
		//get gpx id
		let get_gpxid = 'SELECT gpx_id FROM FILE WHERE file_name = "' + filename + '"';
				
		let [rows, col] = await connection.execute(get_gpxid);

		let id = rows[0].gpx_id;

		let rt_list = lib.route_list(filename);
		
		rt_list = JSON.parse(rt_list);

		let index = rt_list.length - 1;

		let route_len = rt_list[index]["len"];

		let insert_route = 'INSERT IGNORE INTO ROUTE (route_name, route_len, gpx_id) VALUES ("' +
			'None' + '",' + route_len + ',' + id + ')';
		
		await connection.execute(insert_route);

		let get_routeid = 'SELECT route_id FROM ROUTE WHERE route_name = "' + 'None' + '"';

		let [rows3, col3] = await connection.execute(get_routeid);

		let rt_id = rows3[0].route_id;

		waypoints = waypoints.split(" ");
	
		let k = 0;

		for(let i = 0; i < waypoints.length-1; i = i+2) {
			
			let lat = parseFloat(waypoints[i]);
			let lon = parseFloat(waypoints[i+1]);

			let insert_point = 'INSERT IGNORE INTO POINT (point_index, latitude, longitude' +
								', point_name, route_id) VALUES (' + (k+1) + ',' + lat + ',' + lon +
								',"' + 'None' + '",' + rt_id + ")";

			await connection.execute(insert_point);

			k++;
		}

		connected = true;

	}catch(e){
		console.log("Query error: "+e);

	}finally {
		if (connection && connection.end) connection.end();
		
		if (connected == true) {
			res.send({success: true});
		}
		else{
			res.send({success: false});
		}
	}
	


});


app.get('/asynRT', async function(req, res) {
	let connected = false;
	let connection;
	try{
		credentials = {
			host     : 'dursley.socs.uoguelph.ca',
			user     : req.query.user,
			password : req.query.password,
			database : req.query.database
		};

		connection = await mysql.createConnection(credentials);

		let file_name = req.query.filename;
		let x = req.query.path.split(" "); 
		let num = parseInt(x[1]);
		let path = x[0];

		let changevalue = req.query.tochange;


		if(path.toUpperCase() == "ROUTE") {
			let get_gpxid = 'SELECT gpx_id FROM FILE WHERE file_name = "' + file_name + '"';
						
			let [rows, col] = await connection.execute(get_gpxid);

			let id = rows[0].gpx_id;

			let get_rts = 'SELECT route_id FROM ROUTE WHERE gpx_id = ' + id;

			let [rows2, col2] = await connection.execute(get_rts);

			let route_id = rows2[num-1].route_id;

			let update_route = 'UPDATE ROUTE SET route_name = "' + changevalue + '" WHERE route_id =' + route_id; 

			await connection.execute(update_route);
		}

		connected = true;

	}catch(e){
		console.log("Query error: "+e);

	}finally {
		if (connection && connection.end) connection.end();
		
		if (connected == true) {
			res.send({success: true});
		}
		else{
			res.send({success: false});
		}
	}

});

app.get('/deleteData', async function(req, res) {
	let connected = false;
	let connection;
	try{
		credentials = {
			host     : 'dursley.socs.uoguelph.ca',
			user     : req.query.user,
			password : req.query.password,
			database : req.query.database
		};

		connection = await mysql.createConnection(credentials);

		await connection.execute("DELETE FROM POINT");
		await connection.execute("DELETE FROM ROUTE");
		await connection.execute("DELETE FROM FILE");

		connected = true;

	}catch(e){
		console.log("Query error: "+e);

	}finally {
		if (connection && connection.end) connection.end();
		
		if (connected == true) {
			res.send({success: true});
		}
		else{
			res.send({success: false});
		}
	}



});


app.get('/DBStatus', async function(req, res) {
	let connected = false;
	let connection;
	var nums = [];
	try{
		credentials = {
			host     : 'dursley.socs.uoguelph.ca',
			user     : req.query.user,
			password : req.query.password,
			database : req.query.database
		};

		connection = await mysql.createConnection(credentials);

		let [row1, col1] = await connection.execute('SELECT (SELECT COUNT(*) FROM FILE) AS f');
		let [row2, col2] = await connection.execute('SELECT (SELECT COUNT(*) FROM ROUTE) AS r');
		let [row3, col3] = await connection.execute('SELECT (SELECT COUNT(*) FROM POINT) AS p');

		nums[0] = row1[0].f;
		nums[1] = row2[0].r;
		nums[2] = row3[0].p;

		connected = true;

	}catch(e){
		console.log("Query error: "+e);

	}finally {
		if (connection && connection.end) connection.end();
		
		if (connected == true) {
			res.send({'success' : true, 'values': nums});
		}
		else{
			res.send({'success' : false});
		}
	}

});


app.get('/query1', async function(req, res) {
	let connected = false;
	let connection;
	try{
		credentials = {
			host     : 'dursley.socs.uoguelph.ca',
			user     : req.query.user,
			password : req.query.password,
			database : req.query.database
		};

		connection = await mysql.createConnection(credentials);

		if(req.query.q == 1) {
			if(req.query.sort == 'length') {
				let sort = 'SELECT route_id, route_name, route_len, gpx_id  FROM ROUTE ORDER BY route_len';
				var [rows, cols] = await connection.execute(sort);
			}
			
			if(req.query.sort == 'name') {
				let sort = 'SELECT route_id, route_name, route_len, gpx_id  FROM ROUTE ORDER BY route_name';
				var [rows, cols] = await connection.execute(sort);
			}
		}

		if(req.query.q == 2) {
			let file = req.query.filename;

			let get_gpxid = 'SELECT gpx_id FROM FILE WHERE file_name = "' + file + '"';
						
			let [rows2, col2] = await connection.execute(get_gpxid);

			let id = rows2[0].gpx_id;

			if(req.query.sort == 'length') {
				let sort = 'SELECT route_id, route_name, route_len, gpx_id FROM ROUTE WHERE gpx_id =' + id + ' ORDER BY route_len';
				var [rows, cols] = await connection.execute(sort);
			}

			if(req.query.sort == 'name') {
				let sort = 'SELECT route_id, route_name, route_len, gpx_id FROM ROUTE WHERE gpx_id =' + id + ' ORDER BY route_name';
				var [rows, cols] = await connection.execute(sort);
			}

		}

		if(req.query.q == 3) {
			let route_id = req.query.route;
			let sort = 'SELECT point_id, point_index, latitude, longitude, point_name, route_id FROM POINT WHERE route_id=' + route_id + ' ORDER BY point_index';
			var [rows, cols] = await connection.execute(sort);
		}

		connected = true;

	}catch(e){
		console.log("Query error: "+e);

	}finally {
		if (connection && connection.end) connection.end();
		
		if (connected == true) {
			res.send({success: true, row: rows, col: cols});
		}
		else{
			res.send({success: false});
		}
	}

});


app.listen(portNum);
console.log('Running app at localhost: ' + portNum);
