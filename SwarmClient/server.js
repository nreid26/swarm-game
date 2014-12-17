var express		= require('express');
var port = 80;
var app = express();
app.listen(port);

app.get('/',function(req,res){
	console.log('test');
	res.sendfile("index.html");
});

app.get('/:var',function(req,res){
	res.sendfile(req.param('var'));
});

app.get('/images/:var',function(req,res){
	res.sendfile('images/'+req.param('var'));
});