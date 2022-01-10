'use strict'

// C library API
const ffi = require('ffi-napi');

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


let lib = ffi.Library('./libgpxparser', {
 'gpx_to_html': ['string', ['string'] ],

 'route_list': ['string', ['string'] ],

 'track_list': ['string', ['string'] ],

 'gpxRouteDataListToJSON': ['string', ['string', 'int'] ],

 'gpxTrackDataListToJSON': ['string', ['string', 'int'] ],

 'change_route': ['void', ['string', 'int', 'string'] ],

 'change_track': ['void', ['string', 'int', 'string'] ],

 'create_gpx_file': ['void', ['string', 'string'] ],

 'route_add': ['void', ['string', 'string'] ]

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



app.listen(portNum);
console.log('Running app at localhost: ' + portNum);