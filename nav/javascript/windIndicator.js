function wind(angle, id, parent, n){
  var $cont = $(parent),
      $name = $('.name')[n],
      canvas = document.getElementById(id),
      height = $cont.height() - $name.clientHeight,
      width = $cont.width(),
      size = parseInt(Math.min(height, width));
  
  //rendo pari la dimensione di canvas
  if(size % 2 != 0)
    size-=1;
  
  canvas.setAttribute('width', size);
  canvas.setAttribute('height', size);
  canvas.style.width = size + 'px';
  canvas.style.height = size + 'px';
  
  var c = canvas.getContext('2d'),
      pos = (size / 2),
      raggio = parseInt(pos * 0.95);
  draw();
  
  //funzione principale di disegno
  function draw(){
    //pulisco l'area di disegno
    c.clearRect(0, 0, size, size);
    //disegno la circonferenza
    circonferenza();
    
    var smallLine = parseInt(raggio * 0.03),
        smallStart = parseInt(raggio  * 0.9),
        bigLine = parseInt(raggio * 0.05),
        bigStart = parseInt(raggio  * 0.85);
    
    //disegno gli indicatori
    for(var i = 0; i < 360; i+=10){
      !(i % 10) && radialLineAtAngle((i / 360.0), smallLine,  smallStart, raggio, 'grey');
      !(i % 30) && radialLineAtAngle((i / 360.0), bigLine, bigStart, raggio, 'grey');
    }
    
    //disegno la barca
    boat();
    
    var smallFont = parseInt(pos * 0.18),
        bigFont = parseInt(raggio * 0.75);
    //visualizzo i quattro indicatori numerici
    info(0, smallFont, pos, parseInt(raggio * 0.36), 'black');
    info(90, smallFont, parseInt(raggio * 1.8), parseInt(pos + (smallFont * 0.4)), 'black');
    info(90, smallFont, parseInt(raggio * 0.32), parseInt(pos +  (smallFont * 0.4)), 'black');
    info(180, smallFont, pos, parseInt(raggio * 1.9), 'black');
    info(((angle < 180)? angle : (360 - angle)), bigFont, pos, parseInt(pos + (bigFont * 0.4)), 'red');
    
    //visualizzo l'indicatore
    indicatorOut((angle / 360.0));
    
    //funzione che disegna gli indicatori
    function radialLineAtAngle(angleFraction, line, start, end, color){
      c.save();
      c.translate(pos, pos);
      c.rotate(Math.PI * (2.0 * angleFraction - 0.5));
      c.beginPath();
      c.lineWidth = line;
      c.moveTo(start,0);
      c.lineTo(end,0);
      c.strokeStyle = color;
      c.stroke();
      c.closePath();
      c.restore();
    }
    //funzione che disegna l'indicatore rosso
    function indicatorOut(angleFraction){
      var x = parseInt(raggio * 1.05),
          y = parseInt(raggio * 0.15),
          z = parseInt(raggio * 0.65);
      c.save();
      c.translate(pos, pos);
      c.rotate(Math.PI * (2.0 * angleFraction - 0.5));
      c.beginPath();
      c.lineJoin = 'miter';
      c.strokeStyle = 'round';
      c.fillStyle = 'red';
      c.moveTo(z, 0);
      c.lineTo(x, -y);
      c.lineTo(x, y);
      c.lineTo(z, 0);
      c.fill();
      c.stroke();
      c.closePath();
      c.restore();
    }
    //funzione che disegna la circonferenza
    function circonferenza(){
      c.save();
      c.beginPath();
      c.arc(pos, pos, raggio, 0, (2 * Math.PI), false);
      c.lineWidth = parseInt(raggio * 0.04);
      c.strokeStyle = "grey";
      c.stroke(); 
      c.closePath();
      c.restore();
    }
    //funzione che visualizza i numeri
    function info(text, font, posX, posY, color){
      c.save();
      c.font = font + 'px arial';
      c.fillStyle = color;
      var tSize = c.measureText(text);
      c.fillText(text, (posX - (tSize.width / 2)), posY);
      c.restore();
    }
    //funzione che disegna la barca
    function boat(){
      var x = parseInt(raggio * 0.28),
          y = parseInt(raggio * 0.68),
          x1 = parseInt(raggio * 0.8),
          y1 = parseInt(raggio * 0.1);
      c.save();
      c.beginPath();
      c.globalAlpha = 0.8;
      c.translate(pos, pos);
      c.moveTo(x, y);
      c.quadraticCurveTo(x1, -y1, 0, -y);
      c.moveTo(-x, y);
      c.quadraticCurveTo(-x1, -y1, 0, -y);
      c.moveTo(-x, y);
      c.lineTo(x, y);
      c.lineWidth = parseInt(raggio * 0.02);
      c.strokeStyle = "grey";
      c.stroke(); 
      c.closePath();
      c.restore();
    }
  }
}