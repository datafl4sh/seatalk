(function($){
  $(document).ready(function(){
    //visualizzo l'ora
    displayTime();
    
    //funzione di visualizzazione dell'ora
    function displayTime(){
      var now = new Date(),
          hh = now.getHours(),
          mm = now.getMinutes(),
          ss = now.getSeconds();
      $('#clock').text(((hh < 10) ? ('0' + hh) : hh) + ':' + ((mm < 10) ? ('0' + mm) : mm) + ':' + ((ss < 10) ? ('0' + ss) : ss));
      setTimeout(displayTime, 1000);
    }
    
    var $fulls = $('#fullscreen'),
        $locks = $('#lockscreen');
    
    //funzione di blocco/sblocco dei link
    $locks.click(function(){
      if($locks.hasClass('enabled')){
        $locks.removeClass('enabled').addClass('disabled').attr('src', 'img/lock.png');
        $('a').each(function(){
          $(this).data('href', $(this).attr('href')).removeAttr('href').removeClass('active');
        });
      }else{
        $locks.removeClass('disabled').addClass('enabled').attr('src', 'img/un_lock.png');
        $('a').each(function(){
          $(this).attr('href', $(this).data('href')).addClass('active');
        });
      }
    });
    
    //funzione di full screen
    $fulls.click(function(){
      if(!document.fullscreenElement && !document.mozFullScreenElement && !document.webkitFullscreenElement && !document.msFullscreenElement){
        $fulls.attr('src', 'img/fullscreen_close.png');
        if(document.documentElement.requestFullscreen){
          document.documentElement.requestFullscreen();
        }else if(document.documentElement.mozRequestFullScreen){
          document.documentElement.mozRequestFullScreen();
        }else if(document.documentElement.webkitRequestFullscreen){
          document.documentElement.webkitRequestFullscreen();
        }else if(document.documentElement.msRequestFullscreen){
          document.documentElement.msRequestFullscreen();
        }
      }else{
        $fulls.attr('src', 'img/fullscreen_open.png');
        if(document.exitFullscreen){
          document.exitFullscreen();
        }else if(document.mozCancelFullScreen){
          document.mozCancelFullScreen();
        }else if(document.webkitExitFullscreen){
          document.webkitExitFullscreen();
        }else if(document.msExitFullscreen){
          document.msExitFullscreen();
        }
      }
    });
    
    //carico le immagini
    $locks.attr('src', 'img/un_lock.png');
    if(document.fullscreenEnabled || document.mozFullScreenEnabled || document.webkitFullscreenEnabled || document.msFullscreenEnabled || false)
      $fulls.attr('src', 'img/fullscreen_open.png');
    else
      $fulls.remove();
  });
})(jQuery);