var gzippo = require('gzippo');
var express = require('express');
var package = require('./package.json')
var app = module.exports = express();
var port = process.env.PORT || 5000;

// Get date current version was deployed
app.get('/api/info/deploy_date', function(req, res){
  res.send(JSON.stringify(process.env.DEPLOY_DATE));
});

app.get('/api/info/version', function(req, res){
  res.send(JSON.stringify(package.version));
});

// Serve static content when in production
app.use(gzippo.staticGzip("" + __dirname + "/dist"));

app.listen(port);
console.log("Listening on port: " + port);
