function resizeWindow(){
  //riduco a zero la dimensione delle immagini e di canvas per ridimensionare correttamente tutti gli elementi della pagina
  $('#lockscreen').height(0).width(0);
  $('#fullscreen').height(0).width(0);
  $('canvas').height(0).width(0);
  
  //definisco le variabili e le assegno una dimensione
  var height = parseInt($(window).innerHeight() * 0.99),
      width = $(window).innerWidth(),
      infoHeight = $('#info').height(),
      linksHeight = height - infoHeight,
      linksWidth = parseInt(width * 0.1),
      contHeight = height - infoHeight,
      contWidth = width - (2 * linksWidth),
      historyHeight = height - infoHeight,
      historyWidth = width - (2 * linksWidth),
      liHeight = parseInt(linksHeight * 0.34),
      liWidth = linksWidth,
      aHeight = parseInt(liWidth * 0.7),
      aWidth = parseInt(liHeight * 0.85);
  
  //assegno le dimensioni agli elementi della pagina
  $('body').height(height).width(width);
  $('#info').width(width);
  $('#lockscreen').height($('#info').height()).width(parseInt($('#lockscreen').height() * 1.25));
  $('#fullscreen').height($('#info').height()).width($('#fullscreen').height());
  $('#left').height(linksHeight).width(linksWidth);
  $('#right').height(linksHeight).width(linksWidth);
  $('#contenuto').height(contHeight).width(contWidth);
  $('#history').height(historyHeight).width(historyWidth);
  $('li').height(liHeight).width(liWidth);
  $('a').css({
    width: aWidth,
    height: aHeight,
    top: parseInt((liHeight - aHeight) / 2) - 5,
    left: parseInt((liWidth - aWidth) / 2) - 4,
    'line-height': parseInt(aHeight * 0.95) + 'px'
  });
  $('#info').css({
    position: 'absolute',
    left: '0px',
    top: '0px'
  });
  $('#left').css({
    position: 'absolute',
    left: '0px',
    top: infoHeight
  });
  $('#right').css({
    position: 'absolute',
    right: '0px',
    top: infoHeight
  });
  $('#contenuto').css({
    position: 'absolute',
    left: linksWidth,
    top: infoHeight
  });
  $('#history').css({
    position: 'absolute',
    left: linksWidth,
    top: infoHeight
  });
}