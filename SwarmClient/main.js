var socket = null;
var playerId = null;
var $lobby;
var game = null;
var handlerMap = {};

var lobbyMap = {};
var nameMap = {};

$(document).ready(function() {
    $lobby = $('#lobby');

    socket = new WebSocket("ws://104.131.16.145:6113/");

    socket.onopen = function(event) {
	console.log('WebSocket Opened');

    var person = null;
        while(person == null) { person = prompt('Please Enter Your Name', 'Player'); }
	    socket.send('{"name":"'+person+'"}');
    };

    socket.onmessage = function(event) {
        console.log(event.data);
	    var obj = JSON.parse(event.data);

        for(var prop in handlerMap) {
            if(obj.hasOwnProperty(prop)) {
                return handlerMap[prop](obj);
            }
        }
    };
});

handlerMap.playerId = function(obj) { playerId = obj.playerId; };

handlerMap.challenge = function(obj) {
    var name = nameMap[obj.challenge];
	if(name && confirm('Challenge Received From ' + name + ', Accept?')) {
		socket.send('{"accept":' + obj.challenge + '}');
	}
};

handlerMap.player = function(obj) {
    var $lobbyElement;
	if(obj.player.state == 'joining') {
		$lobbyElement = $('<li class="lobby_element">' + obj.player.name + '</li>');

		var $button  = $('<input type="button" value="Challenge" />');
		$button.click(function() {
            socket.send('{"challenge":' + obj.player.id + '}');
        });

		$lobbyElement.append($button);
		$lobby.append($lobbyElement);

        lobbyMap[obj.player.id] = $lobbyElement;
        nameMap[obj.player.id] = obj.player.name;
	} 
    else if(obj.player.state == 'leaving') {
		$lobbyElement = lobbyMap[obj.player.id];
        lobbyMap[obj.player.id] = null;
        if($lobbyElement) { $lobbyElement.remove(); }
	}

};

handlerMap.createPlanet = function(obj) { //Getting planet data
	if(game == null) {
		console.log('Started Game');
		$lobby.hide();
        game = new Game(playerId);
    }

	var planet = new Planet(obj.createPlanet, 8, game);
    game.addPlanet(planet);
};

handlerMap.radius = function(obj) { game.start(); };

handlerMap.deployment = function(obj) {
    console.log("Deployment Sent");
    game.sendDeployment(obj.deployment)
};

handlerMap.updatePlanet = function(obj) { game.updatePlanet(obj.updatePlanet); };

handlerMap.winner = function(obj){
	if(obj.winner == playerId) {
		alert('You Won!');
	} 
    else {
		alert('' + nameMap[obj.winner] + " won");
	}

    nameMap = {};
    lobbyMap = {};
    $lobby.html('');

    game.stop();
    game = null;
	$lobby.show();
};