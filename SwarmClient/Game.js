function Game(playerId) {
    this.playerId = playerId;

    this.scene = new THREE.Scene();
    this.camera = new THREE.PerspectiveCamera(this.VIEW_ANGLE, 1, this.NEAR, this.FAR);
    this.renderer = (Detector.webgl) ? new THREE.WebGLRenderer( {antialias:true, alpha:true} ) : new THREE.CanvasRenderer(); 
    this.controls = new THREE.OrbitControls(this.camera, this.renderer.domElement );
    this.timeLast = Date.now();
    this.raycaster = new THREE.Raycaster();
    this.ships = [];
    this.planets = [];

    this.requestId = null;
    this.glow = null;
    this.glowMaterial = new THREE.MeshLambertMaterial({color:0x699999, opacity:0.15, transparent:true});

    this.planetSelected = -1;
    this.FPS = 60;
    
	this.scene.add(this.camera);
	this.camera.position.set(0,100,400);
	this.camera.lookAt(this.scene.position);
    
	this.renderer.setClearColor(0x000000, 0);

	this.container.append(this.renderer.domElement); //Put the actual game element on the page

    //Add lights in cube formation
    for(var x = -1000; x < 3000; x += 2000) {
        for(var y = -1000; y < 3000; y += 2000) {
            for(var z = -1000; z < 3000; z += 2000) {
	            var light = new THREE.PointLight(0xffffff);
	            light.position.set(x, y, z);
	            this.scene.add(light);
            }
        }
    }

    this.setClickHandler();
    this.setResizeHandler();

    window.addEventListener('resize', this.resize, false);
}

Game.prototype.VIEW_ANGLE = 45;
Game.prototype.NEAR = 0.1;
Game.prototype.FAR = 20000;

$(document).ready(function() { Game.prototype.container = $('#container'); });

Game.prototype.start = function() {
    var me = this; //_start will execute in the context of window on subsequent calls

    var _start = function() {
        me.requestId = window.requestAnimationFrame(_start);
	    me.renderer.render(me.scene, me.camera);		
	    me.update();

        me.container.show();
    }
    _start();
};

Game.prototype.stop = function() { 
    window.cancelAnimationFrame(this.requestId);
    for(var i = 0; i < this.planets.length; i++) { this.planets[i].destroy(); }

    this.container.html('');
    this.container.hide();
};

Game.prototype.screenPosition = function(position) {
	var v = position.clone().project(this.camera);
	return {x:(v.x + 1) / 2 * window.innerWidth, y:(1 - v.y) / 2 * window.innerHeight}
};

Game.prototype.update = function() {
	var now = Date.now();
	var deltaT = now - this.timeLast;
	this.timeLast = now;

	for (var planet in this.planets){
		this.planets[planet].update(deltaT);
	}

	for(var index = this.ships.length - 1; index >= 0; index--){
		if(!this.ships[index].update(deltaT)) { 
			this.scene.remove(this.ships[index])
			this.ships.splice(index, 1);
		} 
	}

	this.controls.update();
};

Game.prototype.addPlanet = function(planet) {
    this.planets.push(planet);
    this.scene.add(planet);
};

Game.prototype.setClickHandler = function() {
    var me = this; //_click will be called in the context of the canvas

    var _click = function(event) {
	    event.preventDefault();

	    var vector = new THREE.Vector3();
	    vector.set( (event.clientX / window.innerWidth) * 2 - 1, -(event.clientY / window.innerHeight) * 2 + 1, 0.5 );
	    vector.unproject(me.camera);

	    me.raycaster.ray.set(me.camera.position, vector.sub(me.camera.position).normalize());
	    var intersects = me.raycaster.intersectObjects(me.planets);

	    if (intersects.length > 0 && me.planetSelected != intersects[0].object.index) { //If a player clicked something and not the same planet twice
		    if(me.planetSelected == -1 && intersects[0].object.player == me.playerId) { 
                me.planetSelected = intersects[0].object.index; 
                me.setGlow(intersects[0].object);
                return;
            }

            else if(me.planetSelected >= 0) {
                console.log('{"deployment":{"source":' + me.planetSelected + ', "destination":' + intersects[0].object.index + '}}');
			    socket.send('{"deployment":{"source":' + me.planetSelected + ', "destination":' + intersects[0].object.index + '}}');
		    }
	    }

        if(me.planetSelected >= 0) {
            me.removeGlow();
            me.planetSelected = -1;
        }

    }
    me.renderer.domElement.addEventListener('click', _click, false); 
};

Game.prototype.setResizeHandler = function() {
    var me = this; //_resize will be called in the context of window

    var _resize = function() {
        //Render paramters
	    me.renderer.setSize(window.innerWidth, window.innerHeight);
	    me.camera.aspect = window.innerWidth / window.innerHeight;
	    me.camera.updateProjectionMatrix();

        //Canvas parameters
        me.renderer.domElement.width = window.innerWidth;
        me.renderer.domElement.height = window.innerHeight
    }
    _resize();
    window.addEventListener('resize', _resize, false);
};

Game.prototype.setGlow = function(planet) {
    this.removeGlow();

    this.glow = new THREE.Mesh(new THREE.SphereGeometry(planet.usedRad + 5, 64, 64), this.glowMaterial); //Instantiate the new mesh slightly larger than the planet
    for(var cord in planet.position) { this.glow.position[cord] = planet.position[cord]; } //Position the glow at the same location
    this.scene.add(this.glow);
};

Game.prototype.removeGlow = function() { this.scene.remove(this.glow); }

Game.prototype.sendDeployment = function(deployment) {
    var MAX_TROOPS = 10;

    var src = this.planets[deployment.source].position;
    var dest = this.planets[deployment.destination].position;
    var troops = deployment.troops;
    var count = Math.ceil(troops / MAX_TROOPS);
    var rotAngle = Math.PI * 2 / count;
    var side = (deployment.player == this.playerId) ? 'friend' : 'enemy';

	this.planets[deployment.source].troops -= troops; //Reduce stationed troops

    var mid = (new THREE.Vector3()).addVectors(dest, src).multiplyScalar(0.5);
    var dir = (new THREE.Vector3()).subVectors(dest, src).normalize(); //Unit source to dest
    var e1 = new THREE.Vector3(-dir.y, dir.x, 0); //Orthonormal to dir
    var e2 = (new THREE.Vector3()).crossVectors(dir, e1); //Orthonormal to dir and e1

    function getControl(index) {
        var RADIUS = 100;
        return (new THREE.Vector3()).copy(e1).multiplyScalar(Math.sin(index * rotAngle))
            .add((new THREE.Vector3()).copy(e2).multiplyScalar(Math.cos(index * rotAngle)))
            .multiplyScalar(RADIUS).add(mid);
    }

    for(var i = 0; i < count; i++) {
        var ship = new Ship(src, getControl(i), dest, deployment.duration, deployment.player, side);
		
        this.ships.push(ship);
        this.scene.add(ship);
    }
};

Game.prototype.updatePlanet = function(update) {
	var planet = this.planets[update.id];

    planet.troops = update.troops;
    planet.player = update.player;
};
