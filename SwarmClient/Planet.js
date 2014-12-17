function Planet(properties, scale, game) {
    this.usedRad = properties.capacity/4;

    //Create an instance of Mesh on each Planet
    THREE.Mesh.call(this, new THREE.SphereGeometry(this.usedRad, 64, 64 ), new THREE.MeshLambertMaterial({emissive: 0x222222, map:this.texture}));

    properties.index = properties.id; //id is used in the Mesh
    delete properties.id;

    for(var cord in properties.position) { this.position[cord] = properties.position[cord] * scale; }
    delete properties.position; //Position must be referenced internallly... ?

    //Copy all the properties onto the planet itself
    for(var key in properties) { this[key] = properties[key];}
    this.capacity = Math.floor(properties.capacity);

    this.game = game;
    this.lastPlayer = -2;

	//Create troop tag
	this.troopTag = this.baseTag.clone();
    this.game.container.append(this.troopTag);

    this.update(0);
}

Planet.prototype = Object.create(THREE.Mesh.prototype);

Planet.prototype.update = function(delta) {
    //Increment troops
	if(this.player >= 0) { this.troops += this.growth * delta / 1000; } //Let troops grow as a double because ticks are not in seconds
    this.troops = (this.troops > this.capacity) ? this.capacity : this.troops; //Troops = min(cap, expected)

    //Reposition troop tag
	var data = this.game.screenPosition(this.position);
	this.troopTag.offset({top:data.y, left:data.x});
    this.troopTag.html('' + Math.round(this.troops) + '/' + this.capacity);

    //Set color
    if(this.lastPlayer != this.player) {
        this.material.color.setHex(this.getColor());
        this.lastPlayer = this.player;
    }
};

Planet.prototype.baseTag = $('<div class="troop_tag"></div>');

Planet.prototype.getColor = function() {
    if(this.player == game.playerId){ return 0x3333FF; }
	else if (this.player == -1) { return 0xA39E80; }
	else { return 0xFF3333; }
};

Planet.prototype.destroy = function() { this.troopTag.remove(); };

Planet.prototype.texture = THREE.ImageUtils.loadTexture('./images/texture.png');