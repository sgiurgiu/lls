var Circle = function(radius) {
    this.radius = radius;
};

Circle.PI = 3.14

Circle.prototype = {
    area: function () {
        return Circle.PI * this.radius * this.radius;
    }
}

var bla = function(ar) {
  log(ar);
  sleep(1150);
  log(ar);
};
