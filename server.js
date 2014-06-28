var	http = require('http'),
	fs = require('fs');

var server = http.createServer(function(request, response) {
	var filePath = null;
	if (request.url == '/') {
		filePath = './index.html';
	} else {
		filePath = '.'+request.url;
	}
	if (filePath) {
	fs.readFile(filePath, function(err, data){
			if (err){ console.log(err); return; }
			console.log((new Date()) + " Received request for " + request.url);
			response.end(data);
		});
	} else {
		response.end("Bad request.");
	}
});

server.listen(8080, function() {
	console.log((new Date()) + " Server is listening on port 8080");
});
