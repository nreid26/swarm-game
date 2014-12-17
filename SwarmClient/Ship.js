function Ship(source, control, destination, duration, player) {
    //Create the instance members of a Mesh on each ship
    //if(shipClass < 10) { THREE.Mesh.call(this, this.smallGeometry, this.smallTexture); }
	//else { THREE.Mesh.call(this, this.bigGeometry, this.bigTexture); }

    THREE.Mesh.call(this, this.smallGeometry, this.smallTexture); //Small ship

    //Set ship properties
	this.lifetime = duration;
	this.dest = destination;
	this.src = source;
	this.ctrl = control;
    this.existance = 0;

    //Set the relevant Mesh properties
    for(var cord in this.src) { this.position[cord] = this.src[cord]; }
	this.scale.set(5, 5, 5);
	this.material.materials[0].shader = THREE.FlatShader;
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

(function() {
    var loader = new THREE.JSONLoader();
    loader.load( "./shipSmall.json", function(geometry, materials) {
    	Ship.prototype.smallGeometry = geometry;
    	Ship.prototype.smallTexture = new THREE.MeshFaceMaterial(materials);
	});
	loader.load( "./shipBig.json", function(geometry, materials) {
    	Ship.prototype.bigGeometry = geometry;
    	Ship.prototype.bigTexture = new THREE.MeshFaceMaterial(materials);
	});
})();