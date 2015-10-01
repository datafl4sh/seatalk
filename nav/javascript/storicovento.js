function showHistory(){
  var canvas = document.getElementById('history'),
      c = canvas.getContext('2d'),
      h = $('#history').height(),
      w = $('#history').width();
  canvas.setAttribute('width', w);
  canvas.setAttribute('height', h);
  canvas.style.width = w + 'px';
  canvas.style.height = h + 'px';
  //pulisco l'area di disegno
  c.clearRect(0, 0, w, h);
  c.beginPath();
  //disegno la linea verticale a meta' area di disegno
  c.moveTo(parseInt(w / 2), 0);
  c.lineTo(parseInt(w / 2), h);
  c.lineWidth = 4;
  c.stroke();
  c.closePath();
  c.beginPath();
  var steph = (h / 180),
      stepw = (w / 360);
  hisV = sessionStorage.getItem("hisV").split(",");
  //disegno lo storico del vento
  for(var i = 0; i <= 180; i++){
    c.lineTo((hisV[i] * stepw), (i * steph));
    
  }
  c.lineWidth = 2;
  c.stroke();
  c.closePath();
}