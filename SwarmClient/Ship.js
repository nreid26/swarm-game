function Ship(source, control, destination, duration, player, side) {
    THREE.Mesh.call(this, this.geometry, this.textures[side]); //Small ship

    //Set ship properties
	this.lifetime = duration;
	this.dest = destination;
	this.src = source;
	this.ctrl = control;
    this.existance = 0;

    //Set the relevant Mesh properties
    for(var cord in this.src) { this.position[cord] = this.src[cord]; }
	this.scale.set(5, 5, 5);
	this.lookAt(this.dest);
}

Ship.prototype = Object.create(THREE.Mesh.prototype); //Inherit all the methods of a Mesh into a Ship

Ship.prototype.bezier = function(cord, t) {
	this.position[cord] = (1-t)*(1-t)*this.src[cord] + 2*(1-t)*t*this.ctrl[cord] + t*t*this.dest[cord];
};

Ship.prototype.update = function(delta) {
	this.existance += delta;
	var t = this.existance / this.lifetime;

	if(t > 1) {return false;}

	this.bezier('x', t);
	this.bezier('y', t);
	this.bezier('z', t);
	return true;
};

Ship.prototype.loader =  new THREE.JSONLoader();

Ship.prototype.resetResources = function() {
    this.loader.load( "./shipSmall.json", function(geometry, materials) {
     	Ship.prototype.geometry = geometry;
        Ship.prototype.textures = {};

        Ship.prototype.textures.enemy = new THREE.MeshFaceMaterial(materials);
        Ship.prototype.textures.enemy.materials[0].color = new THREE.Color(0x3333FF);
        Ship.prototype.textures.enemy.materials[0].shader = THREE.FlatShader;

        Ship.prototype.textures.friend = Ship.prototype.textures.enemy.clone();
        Ship.prototype.textures.enemy.materials[0].color = new THREE.Color(0xFF3333);
	});
}

//+++++++++++++++++++++++++

Ship.prototype.resetResources();
